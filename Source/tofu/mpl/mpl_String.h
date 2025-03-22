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

#include <tofu/mpl/mpl_Common.h>

namespace tofu {
namespace mpl {

//------------------------------------------------------------------------------

// コンパイル時文字列データクラス
template <size_t N>
struct StringData
{
	static constexpr size_t Length = N;
	static constexpr size_t BufferSize = N+1;
	
public:
	char data[BufferSize] = {};

public:
	consteval StringData() = default;

	consteval StringData(const char (&str)[BufferSize])
	{
		std::copy_n(str, BufferSize, data);
	};

	consteval size_t length() const {
		return N;
	}

	consteval size_t size() const {
		return N;
	}

	// 部分文字列取得
	template <size_t Pos, size_t Count = std::string_view::npos>
	consteval auto substr() const
	{
		StringData<std::min(Count, Length - Pos)> ret;
		for(size_t i=0; i<ret.Length; ++i)
		{
			ret[i] = data[Pos + i];
		}
		return ret;
	}
	
	// 文字列位置検索
	template <auto str1>
	consteval auto find() const
	{
		return view().find(str1.view());
	}
	
	template <size_t N1>
	consteval auto find(const char (&str1)[N1]) const
	{
		return view().find(str1);
	}

	// string_viewで参照
	constexpr std::string_view view() const {
		return std::string_view(data, N);
	}

	constexpr explicit operator std::string_view() const {
		return std::string_view(data, N);
	}

	// string化
	constexpr explicit operator std::string() const {
		return std::string(data, N);
	}

	[[nodiscard]] consteval auto operator[](size_t n) const {
		return data[n];
	}

	consteval auto operator[](size_t n) -> char& {
		return data[n];
	}

	friend auto operator<=>(const StringData&, const StringData&) = default;
};

// StringData: 文字配列からのtemplate推論
template <size_t N>
StringData(const char (&)[N]) -> StringData<N - 1>;

// StringData: 結合
template <size_t N1, size_t N2>
inline consteval auto operator+(const StringData<N1>& str1, const StringData<N2>& str2)
{
	StringData<N1 + N2> ret;
	for(size_t i=0; i<N1; ++i)
	{
		ret[i] = str1[i];
	}
	for(size_t i=0; i<N2; ++i)
	{
		ret[N1 + i] = str2[i];
	}
	return ret;
}
// StringData: 結合
template <size_t N1, size_t N2>
inline consteval auto operator+(const StringData<N1>& src1, const char (&src2)[N2])
{
	return src1 + StringData(src2);
}
// StringData: 結合
template <size_t N1, size_t N2>
inline consteval auto operator+(const char (&src1)[N1], const StringData<N2>& src2)
{
	return StringData(src1) + src2;
}

inline namespace string_literals
{
	// StringDataのユーザー定義リテラル
	template <StringData data>
	constexpr auto operator""_sd() {
		return data;
	}
}

//------------------------------------------------------------------------------

/// コンパイル時文字列クラス
template <StringData str>
struct String
{
	using data_t = decltype(str);

	// 文字列データ
	static constexpr auto Data = str;
	
	// 文字列の長さ
	static constexpr size_t Length = data_t::Length;
	
public:
	// string_view形式で文字列取得
	static constexpr std::string_view GetView() { return Data.view(); }

    // C言語styleのNULL終端文字列ポインタ取得
    static inline const char* GetPtr()        { return Data.data; }
	
	// 文字列位置検索
	template <StringData str1>
	static consteval auto Find()
	{
		return Data.template find<str1>();
	}
	
	template <size_t Pos, size_t Count = std::string_view::npos>
	static consteval auto Substr()
	{
		return String<Data.template substr<Pos, Count>()>();
	}
	
public:
	consteval String() = default;
	
	constexpr std::string_view view() const { return Data.view(); }
};

// String: 結合
template <StringData data1, StringData data2>
inline consteval auto operator+(const String<data1>& str1, const String<data2>& str2)
{
	return String<data1 + data2>();
}

namespace detail
{
	// 文字列データ取り出し
	template <StringData str>
	inline consteval auto operator|(const String<str>&, const ToData&)
	{
		return str;
	}
}

namespace detail
{
	// 文字列置換
	template <StringData str1, StringData str2>
	struct ReplaceString{};

	template <StringData str, StringData str1, StringData str2>
	inline consteval auto operator|(const String<str>&, const ReplaceString<str1, str2>&)
	{
		constexpr size_t pos = str.template find<str1>();
		if constexpr (pos == std::string_view::npos)
		{
			return String<str>();
		}
		else
		{
			return String<str.template substr<0, pos>() + str2 + str.template substr<std::min(pos + str1.Length, str.Length)>()>();
		}
	}
}

// 文字列置換
template <StringData str1, StringData str2>
inline constexpr auto ReplaceString = detail::ReplaceString<str1, str2>();

namespace detail
{
	// 部分文字列
	template <size_t Pos, size_t Count = std::string_view::npos>
	struct SubString{};

	template <StringData str, size_t Pos, size_t Count>
	inline consteval auto operator|(const String<str>&, const SubString<Pos, Count>&)
	{
		return String<str.template substr<Pos, Count>()>();
	}
}

// 部分文字列
template <size_t Pos, size_t Count = std::string_view::npos>
inline constexpr auto SubString = detail::SubString<Pos, Count>();

//------------------------------------------------------------------------------

} // mpl
} // tofu
