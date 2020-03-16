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
#include <tofu/util/TypeInfo.hpp>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	struct A {};
	class B; // 宣言のみ
}

// 無名名前空間
namespace
{
	class C;
}

IUTEST(util, TypeInfo)
{
	IUTEST_ASSERT(!tofu::GetTypeInfo<int>().isConst());
	IUTEST_ASSERT(!tofu::GetTypeInfo<int>().isVolatile());

	IUTEST_ASSERT( tofu::GetTypeInfo<const int>().isConst());
	IUTEST_ASSERT(!tofu::GetTypeInfo<const int>().isVolatile());

	IUTEST_ASSERT(!tofu::GetTypeInfo<volatile int>().isConst());
	IUTEST_ASSERT( tofu::GetTypeInfo<volatile int>().isVolatile());

	IUTEST_ASSERT( tofu::GetTypeInfo<const volatile int>().isConst());
	IUTEST_ASSERT( tofu::GetTypeInfo<const volatile int>().isVolatile());

	// add cv, remove cv
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>().makeAddConst(), tofu::MakeTypeId<const int>());
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>(), tofu::MakeTypeId<const int>().makeRemoveConst());
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>().makeAddVolatile(), tofu::MakeTypeId<volatile int>());
	IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>(), tofu::MakeTypeId<volatile int>().makeRemoveVolatile());

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
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<const int>();
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<void>();
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<test::A>();
		std::cout << info.getName() << std::endl;
		IUTEST_ASSERT_EQ( info.getName(), std::string("test::A") );
	}
	{
		auto& info = tofu::GetTypeInfo<test::B>();
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<C>();
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<int&>();
		std::cout << info.getName() << std::endl;
	}
	{
		auto& info = tofu::GetTypeInfo<int*>();
		std::cout << info.getName() << std::endl;
	}
}
