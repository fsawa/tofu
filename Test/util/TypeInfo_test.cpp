//------------------------------------------------------------------------------
/**
 * @file    TypeInfo_test.cpp
 * @brief   型情報テスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/TypeInfo.h>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	struct A {};
	class B; // 宣言のみ

	enum E1{};
	enum class E2{};
}

// 無名名前空間
namespace
{
	class C;

	template <typename T>
	void PrintDebugTypeName()
	{
		std::cout << tofu::TypeName<T>::TestValue.view() << std::endl;
		std::cout << "    " << tofu::TypeName<T>::Value.view() << std::endl;
	}
}

IUTEST(util, TypeInfo)
{
	std::cout << "-- Print --" << std::endl;
	PrintDebugTypeName<int>();
	PrintDebugTypeName<int&>();
	PrintDebugTypeName<int*>();
	PrintDebugTypeName<int**>();
	PrintDebugTypeName<int[]>();
	PrintDebugTypeName<int[3]>();
	PrintDebugTypeName<const int[3][4]>();
	PrintDebugTypeName<const volatile int&>();
	PrintDebugTypeName<test::A>();
	PrintDebugTypeName<const volatile test::A* const>();
	PrintDebugTypeName<C>();
	PrintDebugTypeName<test::E1>();
	PrintDebugTypeName<test::E2>();
	PrintDebugTypeName<void(int)>();
	std::cout << "----" << std::endl;

	// mpl::StringData
	static_assert(tofu::mpl::String<"hoge">::Length == 4);
	static_assert(tofu::mpl::StringData("hoge").Length == 4);
	static_assert((tofu::mpl::StringData("123") + tofu::mpl::StringData("4567")).Length == 7);
	static_assert((tofu::mpl::StringData("123") + "4567").Length == 7);
	static_assert(("123" + tofu::mpl::StringData("4567")).Length == 7);

	//static_assert(tofu::TypeInfoOf<test::A>::TypeName.size() == 7);
	//IUTEST_ASSERT_EQ( tofu::TypeInfoOf<test::A>::TypeName(), std::string_view("test::A") );
	//IUTEST_ASSERT_EQ( tofu::TypeInfoOf<C>::TypeName(), std::string_view("::C") );

	IUTEST_ASSERT(!tofu::GetTypeInfo<int>().isConst());
	//IUTEST_ASSERT(!tofu::GetTypeInfo<int>().isVolatile());

	IUTEST_ASSERT( tofu::GetTypeInfo<const int>().isConst());
	//IUTEST_ASSERT(!tofu::GetTypeInfo<const int>().isVolatile());

	//IUTEST_ASSERT(!tofu::GetTypeInfo<volatile int>().isConst());
	//IUTEST_ASSERT( tofu::GetTypeInfo<volatile int>().isVolatile());

	//IUTEST_ASSERT( tofu::GetTypeInfo<const volatile int>().isConst());
	//IUTEST_ASSERT( tofu::GetTypeInfo<const volatile int>().isVolatile());

	// add cv, remove cv
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>().makeAddConst(), tofu::MakeTypeId<const int>());
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>(), tofu::MakeTypeId<const int>().makeRemoveConst());
	//IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>().makeAddVolatile(), tofu::MakeTypeId<volatile int>());
	//IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>(), tofu::MakeTypeId<volatile int>().makeRemoveVolatile());

	// assignとclear
	{
		tofu::TypeId id;
		IUTEST_ASSERT(id.empty());
		id.assign<void>();
		IUTEST_ASSERT(!id.empty());
		id.clear();
		IUTEST_ASSERT(id.empty());
	}

	{
		auto& info = tofu::GetTypeInfo<int>();
		std::cout << "-- int -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<const int>();
		std::cout << "-- const int -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<void>();
		std::cout << "-- void -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<test::A>();
		std::cout << "-- test::A -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
		IUTEST_ASSERT_EQ( info.GetNamePtr(), std::string("test::A") );
	}
	{
		auto& info = tofu::GetTypeInfo<test::B>();
		std::cout << "-- test::B -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<const C&>();
		std::cout << "-- const C& -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
		std::cout << info.GetName() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<int&>();
		std::cout << "-- int& -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<int*>();
		std::cout << "-- int* -- " << std::endl;
		std::cout << info.DBG_GetTypeNameSample() << std::endl;
		std::cout << info.GetNamePtr() << std::endl;
	}
}
