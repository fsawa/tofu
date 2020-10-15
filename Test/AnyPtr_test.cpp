//------------------------------------------------------------------------------
/**
 * @file    AnyPtr_test.cpp
 * @brief   AnyPtrテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/AnyPtr.hpp>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	struct A {};
	class B {};
}

IUTEST(util, AnyPtr)
{
	test::A a;
	test::B b;

	tofu::AnyPtr ptr;
	
	IUTEST_ASSERT_EQ(ptr.get(), nullptr);
	IUTEST_ASSERT_EQ(ptr.tryCast<int>(), nullptr);
	IUTEST_ASSERT_EQ(ptr, false);
	IUTEST_ASSERT(ptr.empty());
	ptr.type();
	ptr.makeAddConst();
	ptr.clear();

	ptr = &a;
	IUTEST_ASSERT_EQ(ptr.get(), &a);
	IUTEST_ASSERT_EQ(ptr.tryCast<int>(), nullptr);
	IUTEST_ASSERT_EQ(ptr.tryCast<test::A>(), &a);
	IUTEST_ASSERT_EQ(ptr, true);
	IUTEST_ASSERT(!ptr.empty());
	ptr.type();
	IUTEST_ASSERT_EQ(ptr.makeAddConst().type().info().isConst(), true);
	//ptr.clear();

	ptr = &a;

	test::A* ptr_a = ptr; // ok
	test::B* ptr_b = ptr; // ng
	
	IUTEST_ASSERT_EQ(ptr_a, &a);
	IUTEST_ASSERT_EQ(ptr_b, nullptr);

	// 非constからconstへはOK
	const test::A* ptr_ca = ptr;
	IUTEST_ASSERT_EQ(ptr_ca, &a);

	// constから非constへはNG
	ptr_a = ptr.makeAddConst();
	IUTEST_ASSERT_EQ(ptr_a, nullptr);
	
	IUTEST_ASSERT_EQ(ptr, ptr.makeAddConst());
	
	// ポインタ比較
	bool result = false;
	tofu::AnyPtr ptr2 = &b;

	result = ptr == ptr2;
	result = ptr != ptr2;
	result = ptr > ptr2;
	result = ptr < ptr2;
	result = ptr >= ptr2;
	result = ptr <= ptr2;

	result = ptr == nullptr;
	result = nullptr == ptr;
	result = ptr != nullptr;
	result = nullptr != ptr;
}
