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

//#include <tofu.h>

#include <string>
#include <string_view>
#include <format>

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
	static constexpr size_t MaxLength = N;
	static constexpr size_t BufferSize = N+1;

public:
	char data[BufferSize] = {};

public:
	consteval StringData() = default;

	consteval StringData(const char (&str)[BufferSize])
	{
		std::copy_n(str, BufferSize, data);
	};

	constexpr size_t length() const noexcept
	{
		return std::string_view(data).size();
	}

	constexpr size_t size() const noexcept { return length(); }
	
    constexpr std::size_t capacity() const noexcept { return MaxLength; }

	// 部分文字列取得
	template <size_t Pos, size_t Count = std::string_view::npos>
	consteval auto substr() const noexcept
	{
		StringData<std::min(Count, MaxLength - Pos)> ret;
		for(size_t i=0; i<ret.MaxLength; ++i)
		{
			ret[i] = data[Pos + i];
		}
		return ret;
	}
	
	// 文字列位置検索
	template <auto str1>
	consteval auto find() const noexcept
	{
		return view().find(str1.view());
	}
	
	template <size_t N1>
	consteval auto find(const char (&str1)[N1]) const noexcept
	{
		return view().find(str1);
	}

	// string_viewで参照
	constexpr std::string_view view() const noexcept {
		return std::string_view(data);
	}

	constexpr explicit operator std::string_view() const noexcept {
		return view();
	}

	// string化
	constexpr explicit operator std::string() const noexcept {
		return std::string(data, length());
	}

	[[nodiscard]] consteval auto operator[](size_t n) const {
		return data[n];
	}

	consteval char& operator[](size_t n) {
		return data[n];
	}

	friend auto operator<=>(const StringData&, const StringData&) = default;

	// format文字列を書き込む
	template <::tofu::StringData fmt, typename... Args>
	StringData& Format(Args&&... args) noexcept
	{
		const auto result = std::format_to_n(data, MaxLength, fmt.data, std::forward<Args>(args)...);
		const auto n = std::min<size_t>(result.size, MaxLength);
		data[n] = '\0';
		return *this;
	}
};

// StringData: 文字配列からのtemplate推論
template <size_t N>
StringData(const char (&)[N]) -> StringData<N - 1>;

// StringData: 結合
template <size_t N1, size_t N2>
inline consteval auto operator+(const StringData<N1>& str1, const StringData<N2>& str2)
{
	StringData<N1 + N2> ret;
	const auto len1 = str1.length();
	const auto len2 = str2.length();
	size_t n = 0;
	for(size_t i=0; i<len1; ++i) { ret[n++] = str1[i]; }
	for(size_t i=0; i<len2; ++i) { ret[n++] = str2[i]; }
	ret[n] = '\0';
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
