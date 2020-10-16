//------------------------------------------------------------------------------
/**
 * @file    SafePtr_test.cpp
 * @brief   SafePtrテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/SafePtr.hpp>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	class A
	{
	public:
		void func() {}
	};
	//class B {};
}

IUTEST(util, SafePtr)
{
	test::A a[2];

	{
		tofu::SafePtr<test::A> ptr(a);
		IUTEST_ASSERT_EQ(ptr.get(), &a[0]);
		
		tofu::SafePtr<test::A> ptr2(ptr);
		IUTEST_ASSERT_EQ(ptr2.get(), a);
	}
	
	tofu::SafePtr<test::A> ptr;
	tofu::SafePtr<const test::A> ptr2(a + 1);
	
	{ tofu::SafePtr<void> ptr; }
	{ tofu::SafePtr<const void> ptr; }
	{ tofu::SafePtr<int> ptr; }
	{ tofu::SafePtr<const int> ptr; }
	
	IUTEST_ASSERT_EQ(ptr.get(), nullptr);
	IUTEST_ASSERT((nullptr==ptr));
	IUTEST_ASSERT((false==ptr));
	IUTEST_ASSERT(ptr.empty());
	ptr.clear();

	ptr = a;
	IUTEST_ASSERT_EQ(ptr.get(), a);
	IUTEST_ASSERT((ptr==a));
	IUTEST_ASSERT(!ptr.empty());
	//ptr.clear();
	
	ptr->func();
	(*ptr).func();
	
	++ptr;
	ptr++;
	--ptr;
	ptr--;
	ptr+=3;
	ptr-=3;
	
	IUTEST_ASSERT_NE(ptr + 3, ptr - 3);
	
	IUTEST_ASSERT_EQ(ptr2 - ptr, 1);
	
	IUTEST_ASSERT(( ptr < ptr2 ));
	IUTEST_ASSERT(( ptr2 > ptr ));
	IUTEST_ASSERT(( ptr <= ptr2 ));
	IUTEST_ASSERT(( ptr2 >= ptr ));
	IUTEST_ASSERT(( ptr != ptr2 ));
	IUTEST_ASSERT(( ptr == ptr ));
	
	IUTEST_ASSERT(( ptr <= ptr ));
	IUTEST_ASSERT(( ptr >= ptr ));
	
	IUTEST_ASSERT(( ptr != nullptr ));
	IUTEST_ASSERT(( nullptr != ptr ));
	
	ptr.clear();
	IUTEST_ASSERT(( ptr == nullptr ));
	IUTEST_ASSERT(( nullptr == ptr ));

	ptr = a;

	// 非constからconstへはOK
	const test::A* ptr_ca = ptr;
	IUTEST_ASSERT_EQ(ptr_ca, a);
}
