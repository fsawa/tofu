//------------------------------------------------------------------------------
/**
 * @file    AnyBasePtr_test.cpp
 * @brief   AnyBasePtrテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/AnyBasePtr.hpp>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	class A
	{
	public:
		int a;
		A() : a(0) {}
	};

	class B
	{
	public:
		int b;
		B() : b(0) {}
	};

	class AB : public A, public B
	{
	public:
		int ab;
		AB() : ab(0) {}
	};
}

IUTEST(util, AnyBasePtr)
{
	using namespace test;

	A a;
	AB ab;
	
	{
		// コンストラクタ
		tofu::AnyBasePtr<A>  p1;
		tofu::AnyBasePtr<A>  p2( p1 );
		tofu::AnyBasePtr<A>  p3( nullptr );
		tofu::AnyBasePtr<A>  p4( &a );
		tofu::AnyBasePtr<A>  p5( &ab );
		tofu::AnyBasePtr<A>  p6( p5.get(), p5.type() );
		
		// コピー
		p2 = p1;
		p3 = nullptr;
		p4 = &a;
		p5 = &ab;
		
		//
		p6.assign( p5.get(), p5.type() );
		
		IUTEST_ASSERT_EQ( p5.get(), p6.get() );
		IUTEST_ASSERT_TRUE( p5.type() == p6.type() );
		
		IUTEST_ASSERT_EQ( p6.get(), &ab );
		IUTEST_ASSERT_TRUE( p6.type() == tofu::MakeTypeId<AB>() );
		
		// clear
		p6.clear();
		IUTEST_ASSERT_TRUE( p6.get() == nullptr );
		IUTEST_ASSERT_TRUE( p6.type() == tofu::TypeId() );
		
		//------------------------------------------------------------------------------
		
		// null_assert
		//p1.null_assert(); // ng
		p4.null_assert(); // ok
		
		// キャスト
		{
			A* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			AB* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			const A* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			const AB* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		
		// boolキャスト
		IUTEST_ASSERT_FALSE( p1 );
		IUTEST_ASSERT_TRUE( p5 );
		
		// empty
		IUTEST_ASSERT_TRUE( p1.empty() );
		IUTEST_ASSERT_FALSE( p5.empty() );
		
		// safe
		//p1.safe_get(); // ng
		p4.safe_get(); // ok
		
		// makeAddConst
		tofu::AnyBasePtr<const A>  p10 = p5.makeAddConst();
		p10 = p5.makeAddConst();
		
		// to const
		tofu::AnyBasePtr<const A>  p11( p5 );
		//p11 = p5; // 上手くいってない
		
		// 演算子
		p5->a = 1;
		IUTEST_ASSERT_EQ( p5->a, (*p10).a );
		
		// del
		tofu::AnyBasePtr<A>  p20( new AB() );
		p20.del();
		
		//------------------------------------------------------------------------------
		
		// 比較 AnyBasePtr<T>同士
		{
			AB ab_array[2];
			tofu::AnyBasePtr<A>  p100( ab_array + 0 );
			tofu::AnyBasePtr<A>  p200( ab_array + 1 );
			tofu::AnyBasePtr<A>  p300( p100 );
			tofu::AnyBasePtr<A>  p400;
			
			// ==
			IUTEST_ASSERT_FALSE( p100 == p200 );
			IUTEST_ASSERT_TRUE ( p100 == p300 );
			IUTEST_ASSERT_FALSE( p100 == p400 );
		
			// !=
			IUTEST_ASSERT_TRUE ( p100 != p200 );
			IUTEST_ASSERT_FALSE( p100 != p300 );
			IUTEST_ASSERT_TRUE ( p100 != p400 );
			
			// <
			IUTEST_ASSERT_TRUE ( p100 < p200 );
			IUTEST_ASSERT_FALSE( p100 < p300 );
			IUTEST_ASSERT_FALSE( p100 < p400 );
			
			// <=
			IUTEST_ASSERT_TRUE ( p100 <= p200 );
			IUTEST_ASSERT_TRUE ( p100 <= p300 );
			IUTEST_ASSERT_FALSE( p100 <= p400 );
			
			// >
			IUTEST_ASSERT_FALSE( p100 > p200 );
			IUTEST_ASSERT_FALSE( p100 > p300 );
			IUTEST_ASSERT_TRUE ( p100 > p400 );

			// >=
			IUTEST_ASSERT_FALSE( p100 >= p200 );
			IUTEST_ASSERT_TRUE ( p100 >= p300 );
			IUTEST_ASSERT_TRUE ( p100 >= p400 );
		}
		// 比較 nullptr と
		{
			// ==
			IUTEST_ASSERT_FALSE( p5 == nullptr ); // 0 == null
			IUTEST_ASSERT_FALSE( nullptr == p5 ); // 0 == null
		
			// !=
			IUTEST_ASSERT_TRUE( p5 != nullptr ); // 0 != null
			IUTEST_ASSERT_TRUE( nullptr != p5 ); // 0 != null
		}
	}
	//------------------------------------------------------------------------------

	const A c_a;
	
	tofu::AnyBasePtr<A>  p1;
	p1 = &a;
	p1 = &ab;
	
	IUTEST_ASSERT_TRUE( tofu::MakeTypeId<AB>() == p1.type() );
	
	// cast 基底
	p1 = &a;
	IUTEST_ASSERT_TRUE( &a == p1.cast<A>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &a == p1.cast<const A>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.cast<B>() ); // 別の型はNG, 内部のassertに引っかかる
	
	IUTEST_ASSERT_TRUE( &a == p1.tryCast<A>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &a == p1.tryCast<const A>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.tryCast<B>() ); // 別の型はNG
	
	// cast 派生
	p1 = &ab;
	IUTEST_ASSERT_TRUE( &ab == p1.cast<A>() ); // 基底クラスへのキャスト
	IUTEST_ASSERT_TRUE( &ab == p1.cast<const A>() ); // const基底クラスへのキャスト
	IUTEST_ASSERT_TRUE( &ab == p1.cast<AB>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &ab == p1.cast<const AB>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.cast<B>() ); // 別の型はNG, コンパイルエラー
	
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<const A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<AB>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<const AB>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.tryCast<B>() ); // 別の型はNG, コンパイルエラー
	
	IUTEST_ASSERT_TRUE( &ab == (A*)(p1) ); // 基底クラスへのキャスト演算はOK
	IUTEST_ASSERT_TRUE( &ab == (const A*)(p1) ); // 基底クラスへのキャスト演算はOK
	A* pa = p1;
	IUTEST_ASSERT_TRUE( &ab == pa ); // 基底クラスへのキャスト演算はOK

	// const関係
	{
		tofu::AnyBasePtr<const A>  p2;
		p2 = &c_a; // const
		p2 = &a; // const昇格

		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<A>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &a == p2.tryCast<const A>() ); //
		
		p2 = &ab;
		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<A>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &ab == p2.tryCast<const A>() ); //
		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<AB>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &ab == p2.tryCast<const AB>() ); //
	}

	// 別テンプレート同士の比較
	{
		tofu::AnyBasePtr<A>  p100;
		tofu::AnyBasePtr<const A>  p200;
		
		IUTEST_ASSERT_TRUE( p100 == p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 != p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 < p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 > p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 <= p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 >= p200 ); // nullptr同士
	}
	{
		tofu::AnyBasePtr<A>  p100;
		tofu::AnyBasePtr<const AB>  p200;
		
		IUTEST_ASSERT_TRUE( p100 == p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 != p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 < p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 > p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 <= p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 >= p200 ); // nullptr同士
	}
}
