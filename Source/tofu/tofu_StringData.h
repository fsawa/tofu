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
/// @tparam CharT 文字型
/// @note
/// 	- コンパイル時に文字列データを生成することができる
/// 	- テンプレート引数として文字列を渡すことができる
template <size_t N, class CharT>
struct BasicStringData
{
	static constexpr size_t MaxLength = N;
	static constexpr size_t BufferSize = N+1;

public:
	using value_type = CharT; // 文字型

public:
	value_type data[BufferSize] = {};

public:
	consteval BasicStringData() = default;

	consteval BasicStringData(const value_type (&str)[BufferSize])
	{
		std::copy_n(str, BufferSize, data);
	};
	
	template <size_t SrcN>
	consteval BasicStringData(const value_type (&str)[SrcN], size_t n)
	{
		if(n > 0)
		{
			n = std::min(std::min(MaxLength, SrcN-1), n);
			std::copy_n(str, n, data);
			data[n] = '\0';
		}
	};
	
	consteval size_t length() const noexcept
	{
        if constexpr (MaxLength == 0) return 0;
        for (std::size_t i = 0; i < MaxLength; ++i) if (data[i] == '\0') return i;
        return MaxLength;
		//return std::string_view(data).size();
	}

	consteval size_t size() const noexcept { return length(); }
	
    consteval std::size_t capacity() const { return MaxLength; }

	// 部分文字列取得
	template <size_t Pos, size_t Count = std::string_view::npos>
	consteval auto substr() const noexcept
	{
		if constexpr (Pos >= MaxLength)
		{
			return BasicStringData("");
		}
		else
		{
			BasicStringData<std::min(Count, MaxLength - Pos), value_type> ret;
			for(size_t i=0; i<ret.MaxLength; ++i)
			{
				ret[i] = data[Pos + i];
			}
			//ret[ret.MaxLength] = '\0';
			return ret;
		}
	}
	
	// 文字列位置検索
	template <::tofu::BasicStringData str1>
	consteval auto find() const
	{
		return view().find(str1.view());
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
		return std::string(data);
	}

	[[nodiscard]] consteval auto operator[](size_t n) const {
		return data[n];
	}

	consteval value_type& operator[](size_t n) {
		return data[n];
	}

	friend auto operator<=>(const BasicStringData&, const BasicStringData&) = default;
	
	// format文字列を書き込む
	template <::tofu::BasicStringData fmt, typename... Args>
	BasicStringData& Format(Args&&... args) noexcept
	{
		const auto result = std::format_to_n(data, MaxLength, fmt.data, std::forward<Args>(args)...);
		const auto n = std::min<size_t>(result.size, MaxLength);
		data[n] = '\0';
		return *this;
	}
};

// BasicStringData: 文字配列からのtemplate推論
template <size_t N, class CharT>
BasicStringData(const CharT (&)[N]) -> BasicStringData<N - 1, CharT>;

// BasicStringData: 結合
template <size_t N1, size_t N2, class CharT>
inline consteval auto operator+(const BasicStringData<N1, CharT>& str1, const BasicStringData<N2, CharT>& str2)
{
	BasicStringData<N1 + N2, CharT> ret;
	const auto len1 = str1.length();
	const auto len2 = str2.length();
	size_t n = 0;
	for(size_t i=0; i<len1; ++i) { ret[n++] = str1[i]; }
	for(size_t i=0; i<len2; ++i) { ret[n++] = str2[i]; }
	//ret[n] = '\0'; // 初期化時に0fillされているから不要
	return ret;
}
// BasicStringData: 結合
template <size_t N1, size_t N2, class CharT>
inline consteval auto operator+(const BasicStringData<N1, CharT>& src1, const CharT (&src2)[N2])
{
	return src1 + BasicStringData(src2);
}
// BasicStringData: 結合
template <size_t N1, size_t N2, class CharT>
inline consteval auto operator+(const CharT (&src1)[N1], const BasicStringData<N2, CharT>& src2)
{
	return BasicStringData(src1) + src2;
}

// null終端までの文字列だけのBasicStringDataに変換
template <BasicStringData Str>
inline consteval auto ShrinkToFit()
{
	constexpr auto len = Str.length();
	return Str.template substr<0, len>();
}

template <size_t N> using StringData = BasicStringData<N, char>;
template <size_t N> using u8StringData = BasicStringData<N, char8_t>;
template <size_t N> using u16StringData = BasicStringData<N, char16_t>;
template <size_t N> using u32StringData = BasicStringData<N, char32_t>;
template <size_t N> using WStringData = BasicStringData<N, wchar_t>;

inline namespace string_literals
{
	// BasicStringDataのユーザー定義リテラル
	template <BasicStringData data>
	constexpr auto operator""_sd() {
		return data;
	}
}

//------------------------------------------------------------------------------

} // tofu
