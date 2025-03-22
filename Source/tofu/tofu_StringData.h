//------------------------------------------------------------------------------
/**
 * @file   tofu_StringData.h
 * @brief  文字列データクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <string>
#include <string_view>

namespace tofu {

//------------------------------------------------------------------------------

/// @brief 文字列データクラス
/// @tparam N 文字列長
/// @note
/// 	- コンパイル時に文字列データを生成することができる
/// 	- テンプレート引数として文字列を渡すことができる
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

} // tofu
