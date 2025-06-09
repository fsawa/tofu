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
#include <tofu_AnyBasePtr.h>

#include <iostream>
#include <iutest.hpp>

namespace
{
	class A
	{
	public:
		int a;
		A() : a(0){}
	};

	class B
	{
	public:
		int b;
		B() : b(0) {}
	};

	class C{};

	class AB : public A, public B, public C
	{
	public:
		//using base_types = std::tuple<A, B, C>;
		using base_types = std::tuple<A, B>;

	public:
		int ab;
		AB() : ab(0){}
	};
	
	// クラス継承関係登録
	TOFU_RTTI_DERIVED_FROM(AB, A);
	TOFU_RTTI_DERIVED_FROM(AB, B);

	struct S1{};
	struct S2 : public S1 { using base_type = S1; };
	struct S3 : public S2 { using base_type = S2; };
	
	struct T2 : public S1 { using base_type = S1; };


	TOFU_RTTI_DERIVED_FROM(S2, S1);
	TOFU_RTTI_DERIVED_FROM(S3, S2);
	
	TOFU_RTTI_DERIVED_FROM(T2, S1);
	
}

IUTEST(util, AnyBasePtr)
{
	A a;
	AB ab;
	
	{
		// コンストラクタ
		tofu::AnyBasePtr<A>  p1;
		tofu::AnyBasePtr<A>  p2( p1 );
		tofu::AnyBasePtr<A>  p3{};
		tofu::AnyBasePtr<A>  p4( &a );
		tofu::AnyBasePtr<A>  p5( &ab );
		tofu::AnyBasePtr<A>  p6( p5 );
		
		// 非const to const
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp1;
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp2( p1 );
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp3{};
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp4( &a );
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp5( &ab );
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp6( p5 );
		[[maybe_unused]] tofu::AnyBasePtr<const A>  cp7( cp6 );
		
		// コピー
		p2 = p1;
		p3 = {};
		p4 = &a;
		p5 = &ab;
		
		//
		//p6.assign( p5.get(), p5.type() );
		
		IUTEST_ASSERT_EQ( p5.get(), p6.get() );
		IUTEST_ASSERT_TRUE( p5.type() == p6.type() );
		
		IUTEST_ASSERT_EQ( p6.get(), &ab );
		IUTEST_ASSERT_TRUE( p6.type() == tofu::MakeTypeId<AB>() );
		
		// Clear
		p6.reset();
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

	// upcast
	{

		S3 s3;
		tofu::AnyBasePtr<S1> ap = &s3;
		
		S1* ap1 = ap;
		S2* ap2 = ap; // ここがうまくいくかどうか
		S3* ap3 = ap;
		
		S1* p1 = &s3;
		S2* p2 = &s3;
		S3* p3 = &s3;

		IUTEST_ASSERT_EQ(ap1, p1);
		IUTEST_ASSERT_EQ(ap2, p2);
		IUTEST_ASSERT_EQ(ap3, p3);
		
		const S1* cs1 = ap;
		const S2* cs2 = ap;
		const S3* cs3 = ap;
		IUTEST_ASSERT_EQ(cs1, p1);
		IUTEST_ASSERT_EQ(cs2, p2);
		IUTEST_ASSERT_EQ(cs3, p3);
	}
	{
		S2 s2;
		tofu::AnyBasePtr<S1> ap = &s2;
		{
			S3* ap3 = ap; // S3へのキャストは失敗
			IUTEST_ASSERT_EQ(ap3, nullptr);
		}
		{
			const S2* p = ap;
			IUTEST_ASSERT_NE(p, nullptr);
		}
		{
			T2* t2 = ap;
			IUTEST_ASSERT_EQ(t2, nullptr);
		}
	}
	// Tから更にupcast
	{
		AB ab;
		tofu::AnyBasePtr<AB> p = &ab;
		const A* a_any = p;
		const A* a_ptr = &ab;
		IUTEST_ASSERT_EQ(a_any, a_ptr);
	}
	// 多重継承でそれぞれにキャスト可能
	{
		AB ab;
		tofu::AnyBasePtr<void> p = &ab;
		A* a_any = p;
		B* b_any = p;
		
		A* a_ptr = &ab;
		B* b_ptr = &ab;
		
		IUTEST_ASSERT_EQ(a_any, a_ptr);
		IUTEST_ASSERT_EQ(b_any, b_ptr);
	}

	// shared_ptr
	std::cout << "-- shared_ptr" << std::endl;
	{
		{
			tofu::AnyBasePtr<A, std::shared_ptr> p1 = new A;
			tofu::AnyBasePtr<A, std::shared_ptr> p2 = std::make_shared<A>();

			// move
			p2 = std::move(p1);
			IUTEST_ASSERT_TRUE(p1.get() == nullptr);

			// copy
			p1 = p2;
			IUTEST_ASSERT_EQ(p1.get(), p2.get());
			IUTEST_ASSERT_EQ(p1, p2);

			// 生ポインタの代入
			p1 = new A;
			p1 = new AB;
			
			// holder代入
			p1 = std::make_shared<A>();
			p1 = std::make_shared<AB>();

			// 派生
			tofu::AnyBasePtr<AB, std::shared_ptr> p3 = new AB;
			p1 = p3;
			p1 = std::move(p3);
		}
		// 派生クラスのインスタンス代入
		{
			tofu::AnyBasePtr<A, std::shared_ptr> p1 = new AB;
			tofu::AnyBasePtr<A, std::shared_ptr> p2 = std::make_shared<AB>();
		}
		// void
		{
			tofu::AnyBasePtr<void, std::shared_ptr> p1 = new A;
			IUTEST_ASSERT_EQ(p1.type(), tofu::MakeTypeId<A>());
			A* a = p1; // A*として取り出せる
			IUTEST_ASSERT_FALSE(a == nullptr);
		}
		{
			AB* ab = new AB;
			tofu::AnyBasePtr<void, std::shared_ptr> p1 = ab;
			IUTEST_ASSERT_EQ(p1.type(), tofu::MakeTypeId<AB>());
			A* a = p1; // A*として取り出せる
			IUTEST_ASSERT_EQ(static_cast<A*>(ab), a);
			IUTEST_ASSERT_EQ(static_cast<A*>(ab), p1.tryCast<A>());
			B* b = p1; // B*
			IUTEST_ASSERT_EQ(static_cast<B*>(ab), b);
			IUTEST_ASSERT_EQ(static_cast<B*>(ab), p1.tryCast<B>());
			C* c = p1; // Cは基底クラス登録していないので取り出せない
			IUTEST_ASSERT_NE(static_cast<C*>(ab), c);
			IUTEST_ASSERT_NE(static_cast<C*>(ab), p1.tryCast<C>());
			
			p1 = new AB;
		}
	}
	std::cout << "-- done" << std::endl;
}
