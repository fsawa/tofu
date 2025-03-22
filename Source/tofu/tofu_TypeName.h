//------------------------------------------------------------------------------
/**
 * @file   tofu_TypeName.h
 * @brief  型名文字列
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <type_traits>
#include <string>
#include <string_view>

#include <tofu_mpl_String.h>

namespace tofu {
	
////////////////////////////////////////////////////////////////////////////////
/// @brief      名前空間を含む形名文字列を表すクラス
/// 
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TypeName
{
	static consteval auto Make();
	
	// 環境依存確認用のテスト関数
	static consteval auto Test()
	{
		constexpr StringData name{TOFU_FUNCTION_NAME};
		return name;
	}

public:

	/// 型名文字列を表すStringData
	static constexpr auto Value = Make();
	
	/// 環境依存確認用のテスト値
	static constexpr auto TestValue = Test();
};

// 型名文字列の生成
template <typename T>
consteval auto TypeName<T>::Make()
{
	// 参照ならまず参照だけ外す
	if constexpr (std::is_reference_v<T>)
	{
		constexpr auto name_ = TypeName<std::remove_reference_t<T>>::Value;
		return name_ + " &";
	}
	// const外す
	else if constexpr (std::is_const_v<T>)
	{
		constexpr auto name_ = TypeName<std::remove_const_t<T>>::Value;
		return name_ + " const";
	}
	// volatile外す
	else if constexpr (std::is_volatile_v<T>)
	{
		constexpr auto name_ = TypeName<std::remove_volatile_t<T>>::Value;
		return name_ + " volatile";
	}
	// pointer外す
	else if constexpr (std::is_pointer_v<T>)
	{
		constexpr auto name_ = TypeName<std::remove_pointer_t<T>>::Value;
		return name_ + " *";
	}
	else
	{
// clang
#if defined(TOFU_COMPILER_CLANG)
		// static auto tofu::TypeName<int>::Debug() [T = int]
		// static auto tofu::TypeName<const int[3][4]>::Test() [T = const int[3][4]]
		// static auto tofu::TypeName<test::A>::Debug() [T = test::A]
		// static auto tofu::TypeName<const test::A *const>::Debug() [T = const test::A *const]
		// static auto tofu::TypeName<(anonymous namespace)::C>::Debug() [T = (anonymous namespace)::C]
		// static auto tofu::TypeName<test::F<const int, 1>>::Test() [T = test::F<const int, 1>]
		// tofu::TypeName<void (int)>::Test
			
		constexpr auto name = mpl::String<TOFU_FUNCTION_NAME>();

		constexpr size_t pos_s = name.view().find_first_of('<') + 1;
		constexpr size_t pos_eq = name.view().find_last_of('=');
		constexpr size_t pos_e = name.view().find_last_of('>', pos_eq);
// Microsoft Visual C++
#elif defined(_MSC_VER)
		// tofu::TypeName<int>::Debug
		// tofu::TypeName<struct test::A>::Debug
		// tofu::TypeName<struct test::A const * const>::Debug
		// tofu::TypeName<class `anonymous namespace'::C>::Debug
		// tofu::TypeName<void __cdecl(int)>::Test
			
		// structやclassを除外
		constexpr auto name = mpl::String<TOFU_FUNCTION_NAME>()
			| mpl::ReplaceString<"class ", "">
			| mpl::ReplaceString<"struct ", "">
			| mpl::ReplaceString<"enum ", "">
			| mpl::ReplaceString<"__cdecl", "">
			| mpl::ReplaceString<"`anonymous namespace'", "(anonymous namespace)">;

		constexpr size_t pos_s = name.view().find_first_of('<') + 1;
		constexpr size_t pos_e = name.view().find_last_of('>');
#else
		/// @todo 
		return name;
#endif
		return name | mpl::SubString<pos_s, pos_e - pos_s> | mpl::ToData;
	}
};

// 名前空間を含む型名文字列の取得
template <typename T>
static constexpr std::string_view GetTypeName()
{
	return TypeName<T>::Value.view();
}

} // tofu
