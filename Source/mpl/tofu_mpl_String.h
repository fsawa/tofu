//------------------------------------------------------------------------------
/**
 * @file   mpl_String.h
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
#include <tofu_mpl_Common.h>

namespace tofu {
namespace mpl {

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
