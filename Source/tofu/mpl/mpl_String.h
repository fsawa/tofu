//------------------------------------------------------------------------------
/**
 * @file   mpl_String.h
 * @brief  コンパイル地文字列ライブラリ
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <type_traits>
#include <string>
#include <string_view>

namespace tofu {
namespace mpl {

//------------------------------------------------------------------------------

// コンパイル時文字列クラス（NULL終端を含まない）
template <size_t N>
struct String
{
	static constexpr size_t Length = N;
	static constexpr size_t BufferSize = std::max<size_t>(N,1);
	
	char data[BufferSize] = {};

	String() = default;

	consteval String(const char (&str)[N + 1])
	{
		for (size_t i = 0; i < N; i++) {
			data[i] = str[i];
		}
	};

	template <size_t M>
	consteval String(const char(&src)[M], size_t s)
	{
		for (size_t i = 0; i < N; ++i)
		{
			data[i] = src[s + i];
		}
	}
	
	consteval explicit String(const std::string_view src)
	{
		//static_assert(N == src.size(), "Length is not match.");
		for (size_t i = 0; i < N; ++i)
		{
			data[i] = src[i];
		}
	}
	
	// 結合
	template <size_t N1, size_t N2>
	consteval explicit String(const String<N1> src1, const String<N2> src2)
	{
		static_assert(N == src1.Length + src2.Length, "Length is not match.");
		size_t dst = 0;
		for (size_t i = 0; i < src1.Length; ++i)
		{
			data[dst++] = src1.data[i];
		}
		for (size_t i = 0; i < src2.Length; ++i)
		{
			data[dst++] = src1.data[i];
		}
	}

	constexpr explicit operator std::string() const {
		return std::string(data, N);
	}

	constexpr explicit operator std::string_view() const {
		return std::string_view(data, N);
	}

	[[nodiscard]] consteval auto operator[](size_t n) const {
		return data[n];
	}

	consteval auto operator[](size_t n) -> char& {
		return data[n];
	}
};

// String: 文字配列からのtemplate推論
template <size_t N>
String(const char (&)[N]) -> String<N - 1>;

// String: String結合の推論
template <size_t N1, size_t N2>
String(const String<N1>, const String<N2>) -> String<N1 + N2>;

//------------------------------------------------------------------------------

// コンパイル時NULL終端文字列クラス
template <size_t N>
struct NullTerminatedString
{
	static constexpr size_t Length = N;
	static constexpr size_t BufferSize = N+1;
	char data[BufferSize] = {};

	NullTerminatedString() = default;

	// 文字列リテラルから
	consteval NullTerminatedString(const char (&str)[BufferSize])
	{
		//static_assert(str[N] == '\0', "Not null terminated.");
		for (size_t i = 0; i < BufferSize; i++) {
			data[i] = str[i];
		}
		data[N] = '\0'; // 安全対策
	};
	
	// Stringから
	template <size_t N1>
	consteval explicit NullTerminatedString(const String<N1> src1)
	{
		static_assert(Length == src1.Length, "Length is not match.");
		size_t dst = 0;
		for (size_t i = 0; i < src1.Length; ++i)
		{
			data[dst++] = src1.data[i];
		}
		data[dst] = '\0';
	}

	constexpr explicit operator std::string() const {
		return std::string(data, N);
	}

	constexpr explicit operator std::string_view() const {
		return std::string_view(data, N);
	}

	[[nodiscard]] consteval auto operator[](size_t n) const {
		return data[n];
	}

	consteval auto operator[](size_t n) -> char& {
		return data[n];
	}
};

template <size_t N>
NullTerminatedString(const char (&)[N]) -> NullTerminatedString<N-1>;

template <size_t N>
NullTerminatedString(const String<N>) -> NullTerminatedString<N>;


/// 文字列リテラルクラス
template <NullTerminatedString str>
struct StringLiteralFromNullTerminated
{
	static constexpr decltype(str) String = str;

	// 文字列の長さ
	static consteval size_t Length()         { return decltype(String)::Length; }

	// 文字列の長さ
	static consteval size_t Size()           { return decltype(String)::Length; }
	
	// string_view形式で文字列取得
    static constexpr std::string_view View() { return std::string_view(String); }
    
    // C言語styleのNULL終端文字列ポインタ取得
    static inline const char* c_str()        { return String.data; }
};

/// 文字列リテラルクラス
template <String str>
struct StringLiteral : public StringLiteralFromNullTerminated<NullTerminatedString(str)> {};

} // mpl
} // tofu
