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
#include <tofu_AnyPtr.h>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	struct A {};
	class B {};

	class B1 {};
	class B2 : public B1 {};
	class B3 : public B2
	{
	public:
		using Base = B2; // Baseを定義すれば自動で基底クラス検出される
	};

	//template class BaseTypeInfo<B3, B2>;
	
	TOFU_RTTI_DERIVED_FROM(B2, B1);
	//TOFU_RTTI_DERIVED_FROM(B3, B2);
}

IUTEST(util, AnyPtr)
{
		//tofu::DefineDerivedFrom<test::B3, test::B2>();
		//tofu::DefineDerivedFromAuto<test::B3>();
	test::A a;
	test::B b;

	tofu::AnyPtr ptr;
	
	IUTEST_ASSERT_EQ(ptr.get(), nullptr);
	IUTEST_ASSERT_EQ(ptr.tryCast<int>(), nullptr);
	IUTEST_ASSERT_EQ(static_cast<bool>(ptr), false);
	IUTEST_ASSERT(ptr.empty());
	ptr.type();
	ptr.GetAddConst();
	ptr.reset();

	ptr = &a;
	IUTEST_ASSERT_EQ(ptr.get(), &a);
	IUTEST_ASSERT_EQ(ptr.tryCast<int>(), nullptr);
	IUTEST_ASSERT_EQ(ptr.tryCast<test::A>(), &a);
	IUTEST_ASSERT_EQ(static_cast<bool>(ptr), true);
	IUTEST_ASSERT(!ptr.empty());
	ptr.type();
	IUTEST_ASSERT_EQ(ptr.GetAddConst().type().info().IsConst(), true);
	//ptr.Clear();

	ptr = {};
	ptr = &a;

	test::A* ptr_a = ptr; // ok
	test::B* ptr_b = ptr; // ng
	
	IUTEST_ASSERT_EQ(ptr_a, &a);
	IUTEST_ASSERT_EQ(ptr_b, nullptr);

	// 非constからconstへはOK
	const test::A* ptr_ca = ptr;
	IUTEST_ASSERT_EQ(ptr_ca, &a);

	// constから非constへはNG
	ptr_a = ptr.GetAddConst();
	IUTEST_ASSERT_EQ(ptr_a, nullptr);
	
	IUTEST_ASSERT_EQ(ptr, ptr.GetAddConst());
	
	// ポインタ比較
	[[maybe_unused]] bool result = false;
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

	// const代入
	{
		const test::A ca;
		tofu::AnyPtr ptr3 = &ca;
		IUTEST_ASSERT_NE(ptr3.get(), nullptr);
		
		const test::A* ptr1 = ptr3;
		IUTEST_ASSERT_EQ(ptr1, &ca);

		// const外せない
		test::A* ptr2 = ptr3;
		IUTEST_ASSERT_EQ(ptr2, nullptr);
	}

	// アップキャスト
	{
		using namespace test;
		B3 b3;

		//tofu::GetTypeInfo<B3>().SetBaseType<B2>();
		//tofu::GetTypeInfo<B2>().SetBaseType<B1>();

		ptr = &b3;
		IUTEST_ASSERT_EQ( static_cast<B3*>(&b3), ptr.tryCast<B3>() );
		IUTEST_ASSERT_EQ( static_cast<B2*>(&b3), ptr.tryCast<B2>() );
		IUTEST_ASSERT_EQ( static_cast<B1*>(&b3), ptr.tryCast<B1>() );
		IUTEST_ASSERT_EQ( static_cast<B1*>(&b3), ptr.tryCast<const B1>() );
		IUTEST_ASSERT_EQ( nullptr, ptr.tryCast<A>() );

		const B3 const_b3;
		ptr = &const_b3;
		IUTEST_ASSERT_EQ( static_cast<const B3*>(&const_b3), ptr.tryCast<const B3>() );
		IUTEST_ASSERT_EQ( static_cast<const B2*>(&const_b3), ptr.tryCast<const B2>() );
		IUTEST_ASSERT_EQ( static_cast<const B1*>(&const_b3), ptr.tryCast<const B1>() );
		// constは外せない
		IUTEST_ASSERT_EQ( nullptr, ptr.tryCast<B3>() );
		IUTEST_ASSERT_EQ( nullptr, ptr.tryCast<B2>() );
		IUTEST_ASSERT_EQ( nullptr, ptr.tryCast<B1>() );
		
		[[maybe_unused]] tofu::AnyPtr ptr2 = &const_b3;
	}
}
