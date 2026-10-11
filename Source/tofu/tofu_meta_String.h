//------------------------------------------------------------------------------
/**
 * @file   tofu_meta_String.h
 * @brief  コンパイル時文字列処理ライブラリ
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

#include <tofu_StringData.h>

namespace tofu {
namespace meta {

//------------------------------------------------------------------------------

/// コンパイル時文字列クラス
template <StringData TData>
struct String
{
	using DataType = decltype(TData);

	// 文字列データ
	static constexpr auto Data = ShrinkToFit<TData>();
	
	// 文字列View
	static constexpr auto View = Data.view();
	
	// 文字列の長さ
	static constexpr size_t Length = Data.MaxLength;
	
public:

	// 静的StringData取得
	static consteval const DataType& GetData() { return Data; }

	// 静的文字列をstring_view形式で取得
	static constexpr std::string_view GetView() noexcept { return View; }

    // 静的なNULL終端文字列ポインタ取得
    static constexpr const char* GetPtr() noexcept { return Data.data; }
	
	//--------------------------------------------------------------------------------

	// 文字列位置検索
	template <StringData str1>
	static consteval auto Find()
	{
		return Data.template find<str1>();
	}
	
	// 部分文字列取得
	template <size_t Pos, size_t Count = std::string_view::npos>
	static consteval auto Substr()
	{
		return String<Data.template substr<Pos, Count>()>();
	}
	
	// 文字列置換
	template <StringData str1, StringData str2>
	static consteval auto Replace()
	{
		constexpr size_t pos = Find<str1>();
		if constexpr (pos == std::string_view::npos)
		{
			return String<Data>();
		}
		else
		{
			constexpr auto s = Data.template substr<0, pos>()
				+ str2
				+ Data.template substr<pos + str1.length()>();
			return String<s>();
		}
	}
	
	// 文字列追加
	template <StringData str1>
	static consteval auto Add()
	{
		return String<Data + str1>();
	}
	
public:
	consteval String() = default;
};

// String: 結合
template <StringData data1, StringData data2>
inline consteval auto operator+(const String<data1>&, const String<data2>&)
{
	return String<data1 + data2>();
}


namespace detail
{
	// データ取り出し
	struct ToData {};

	// 文字列データ取り出し
	template <StringData str>
	inline consteval auto operator|(const String<str>&, const ToData&)
	{
		return String<str>::Data;
	}
}

// データ取り出し
inline constexpr auto ToData = detail::ToData();

namespace detail
{
	// 文字列置換
	template <StringData str1, StringData str2>
	struct ReplaceString{};

	template <StringData str, StringData str1, StringData str2>
	inline consteval auto operator|(String<str>, ReplaceString<str1, str2>)
	{
		constexpr size_t pos = str.template find<str1>();
		if constexpr (pos == std::string_view::npos)
		{
			return String<str>();
		}
		else
		{
			return String<str.template substr<0, pos>() + str2 + str.template substr<std::min(pos + str1.MaxLength, str.MaxLength)>()>();
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

} // meta

inline namespace string_literals
{
	// MetaStringのユーザー定義リテラル
	template <StringData data>
	constexpr auto operator""_ms() {
		return meta::String<data>();
	}
}

} // tofu
