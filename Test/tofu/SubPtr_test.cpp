//------------------------------------------------------------------------------
/**
 * @file    SubPtr_test.cpp
 * @brief   SubPtrテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu_SubPtr.h>

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
		using Base = A;

	public:
		int ab;
		AB() : ab(0){}
	};
	
	// クラス継承関係登録
	TOFU_RTTI_DERIVED_FROM(AB, A);
	TOFU_RTTI_DERIVED_FROM(AB, B);

	struct S1{};
	struct S2 : public S1 { using Base = S1; };
	struct S3 : public S2 { using Base = S2; };
	
	struct T2 : public S1 { using Base = S1; };


	//TOFU_RTTI_DERIVED_FROM(S2, S1);
	//TOFU_RTTI_DERIVED_FROM(S3, S2);
	
	//TOFU_RTTI_DERIVED_FROM(T2, S1);
	
}

IUTEST(util, SubPtr)
{
	A a;
	AB ab;
	
	{
		auto func = [](tofu::SubPtr<A>){};

		// コンストラクタ
		tofu::SubPtr<A>  p1;
		tofu::SubPtr<A>  p2( p1 );
		tofu::SubPtr<A>  p3{};
		tofu::SubPtr<A>  p4( &a );
		tofu::SubPtr<A>  p5( &ab );
		tofu::SubPtr<A>  p6( p5 );
		tofu::SubPtr<A>  p7( nullptr );

		func({});
		func( p1 );
		func( &a );
		func( &ab );
		func( nullptr );
		
		// 非const to const
		[[maybe_unused]] tofu::SubPtr<const A>  cp1;
		[[maybe_unused]] tofu::SubPtr<const A>  cp2( p1 );
		[[maybe_unused]] tofu::SubPtr<const A>  cp3{};
		[[maybe_unused]] tofu::SubPtr<const A>  cp4( &a );
		[[maybe_unused]] tofu::SubPtr<const A>  cp5( &ab );
		[[maybe_unused]] tofu::SubPtr<const A>  cp6( p5 );
		[[maybe_unused]] tofu::SubPtr<const A>  cp7( cp6 );

		// constから非constへはNG
		{
			[[maybe_unused]] const A* p = &a;
			//p1 = cp1;
			//p1 = p;
		}

		p1.ToConst();

		p1 = nullptr;
		
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
		
		// ToConst
		tofu::SubPtr<const A>  p10 = p5.ToConst();
		p10 = p5.ToConst();
		
		// to const
		tofu::SubPtr<const A>  p11( p5 );
		//p11 = p5; // 上手くいってない
		
		// 演算子
		p5->a = 1;
		IUTEST_ASSERT_EQ( p5->a, (*p10).a );
		
		//------------------------------------------------------------------------------
		
		// 比較 SubPtr<T>同士
		{
			AB ab_array[2];
			tofu::SubPtr<A>  p100( ab_array + 0 );
			tofu::SubPtr<A>  p200( ab_array + 1 );
			tofu::SubPtr<A>  p300( p100 );
			tofu::SubPtr<A>  p400;
			
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
	
	tofu::SubPtr<A>  p1;
	p1 = &a;
	p1 = &ab;
	
	IUTEST_ASSERT_TRUE( tofu::MakeTypeId<AB>() == p1.type() );
	
	// cast 基底
	p1 = &a;
	IUTEST_ASSERT_TRUE( &a == p1.Cast<A>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &a == p1.Cast<const A>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.Cast<B>() ); // 別の型はNG, 内部のassertに引っかかる
	
	IUTEST_ASSERT_TRUE( &a == p1.TryCast<A>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &a == p1.TryCast<const A>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.TryCast<B>() ); // 別の型はNG
	
	// cast 派生
	p1 = &ab;
	IUTEST_ASSERT_TRUE( &ab == p1.Cast<A>() ); // 基底クラスへのキャスト
	IUTEST_ASSERT_TRUE( &ab == p1.Cast<const A>() ); // const基底クラスへのキャスト
	IUTEST_ASSERT_TRUE( &ab == p1.Cast<AB>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &ab == p1.Cast<const AB>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.Cast<B>() ); // 別の型はNG, コンパイルエラー
	
	IUTEST_ASSERT_TRUE( &ab == p1.TryCast<A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.TryCast<const A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.TryCast<AB>() ); // 同一の型はOK
	IUTEST_ASSERT_TRUE( &ab == p1.TryCast<const AB>() ); // const昇格はOK
//	IUTEST_ASSERT_TRUE( nullptr == p1.TryCast<B>() ); // 別の型はNG, コンパイルエラー
	
	IUTEST_ASSERT_TRUE( &ab == (A*)(p1) ); // 基底クラスへのキャスト演算はOK
	IUTEST_ASSERT_TRUE( &ab == (const A*)(p1) ); // 基底クラスへのキャスト演算はOK
	A* pa = p1;
	IUTEST_ASSERT_TRUE( &ab == pa ); // 基底クラスへのキャスト演算はOK

	// const関係
	{
		tofu::SubPtr<const A>  p2;
		p2 = &c_a; // const
		p2 = &a; // const昇格

		IUTEST_ASSERT_TRUE( nullptr == p2.TryCast<A>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &a == p2.TryCast<const A>() ); //
		
		p2 = &ab;
		IUTEST_ASSERT_TRUE( nullptr == p2.TryCast<A>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &ab == p2.TryCast<const A>() ); //
		IUTEST_ASSERT_TRUE( nullptr == p2.TryCast<AB>() ); // 非constへはキャスト不可
		IUTEST_ASSERT_TRUE( &ab == p2.TryCast<const AB>() ); //
	}
	{
		[[maybe_unused]] tofu::SubPtr<A>  p2;
		//p2 = &c_a; // const入れられない　コンパイル時エラー
	}

	// 別テンプレート同士の比較
	{
		tofu::SubPtr<A>  p100;
		tofu::SubPtr<const A>  p200;
		
		IUTEST_ASSERT_TRUE( p100 == p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 != p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 < p200 ); // nullptr同士
		IUTEST_ASSERT_FALSE( p100 > p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 <= p200 ); // nullptr同士
		IUTEST_ASSERT_TRUE( p100 >= p200 ); // nullptr同士
	}
	{
		tofu::SubPtr<A>  p100;
		tofu::SubPtr<const AB>  p200;
		
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
		tofu::SubPtr<S1> ap = &s3;
		
		S1* ap1 = ap;
		S2* ap2 = ap; // ここがうまくいくかどうか
		S3* ap3 = ap;
		
		S1* p_1 = &s3;
		S2* p_2 = &s3;
		S3* p_3 = &s3;

		IUTEST_ASSERT_EQ(ap1, p_1);
		IUTEST_ASSERT_EQ(ap2, p_2);
		IUTEST_ASSERT_EQ(ap3, p_3);
		
		const S1* cs1 = ap;
		const S2* cs2 = ap;
		const S3* cs3 = ap;
		IUTEST_ASSERT_EQ(cs1, p_1);
		IUTEST_ASSERT_EQ(cs2, p_2);
		IUTEST_ASSERT_EQ(cs3, p_3);
	}
	{
		S2 s2;
		tofu::SubPtr<S1> ap = &s2;
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
		AB ab2;
		tofu::SubPtr<AB> p = &ab2;
		const A* a_any = p;
		const A* a_ptr = &ab2;
		IUTEST_ASSERT_EQ(a_any, a_ptr);
	}
	// 多重継承でそれぞれにキャスト可能
	{
		AB ab2;
		tofu::SubPtr<AB> p = &ab2;
		A* a_any = p;
		B* b_any = p;
		
		A* a_ptr = &ab2;
		B* b_ptr = &ab2;
		
		IUTEST_ASSERT_EQ(a_any, a_ptr);
		IUTEST_ASSERT_EQ(b_any, b_ptr);
	}

	// shared_ptr
	std::cout << "-- shared_ptr" << std::endl;
	{
		{
			tofu::SubPtr<A, std::shared_ptr> p_1 = new A;
			tofu::SubPtr<A, std::shared_ptr> p_2 = std::make_shared<A>();

			// move
			p_2 = std::move(p_1);
			IUTEST_ASSERT_TRUE(p_1.get() == nullptr);

			// copy
			p_1 = p_2;
			IUTEST_ASSERT_EQ(p_1.get(), p_2.get());
			IUTEST_ASSERT_EQ(p_1, p_2);

			// 生ポインタの代入
			p_1 = new A;
			p_1 = new AB;
			
			// holder代入
			p_1 = std::make_shared<A>();
			p_1 = std::make_shared<AB>();

			// 派生
			tofu::SubPtr<AB, std::shared_ptr> p_3 = new AB;
			p_1 = p_3;
			p_1 = std::move(p_3);

			auto p_4 = p_1.ToConst();
		}
		// 派生クラスのインスタンス代入
		{
			tofu::SubPtr<A, std::shared_ptr> p_1 = new AB;
			tofu::SubPtr<A, std::shared_ptr> p_2 = std::make_shared<AB>();
		}
	}
	std::cout << "-- done" << std::endl;
}
