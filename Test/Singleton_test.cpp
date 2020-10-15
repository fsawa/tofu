//------------------------------------------------------------------------------
/**
 * @file    Singleton_test.cpp
 * @brief   シングルトンテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/Singleton.hpp>
#include <tofu/util/LooseSingleton.hpp>

#include <iostream>
#include <iutest.hpp>

// Looseシングルトン
namespace test_loose
{
	// 継承するだけでOK
	class A : public tofu::LooseSingleton<A>
	{
	};
}
class Foo {};
struct SubTag;

// LooseシングルトンのHolderテスト
using GlobalMainFoo = tofu::LooseSingletonHolder<Foo>;
using GlobalSubFoo = tofu::LooseSingletonHolder<Foo, SubTag>;

// Static
namespace test_static
{
	class A : public tofu::Singleton<A, tofu::singleton::StaticCreator>
	{
	public:
		void foo() {}
	};
	
	A::SingletonAccessor  g_A;
}
// Dynamic
namespace test_dynamic
{
	class A : public tofu::Singleton<A, tofu::singleton::DynamicCreator>
	{
	public:
		void foo() {}
	};
	
	A::SingletonAccessor  g_A;
}

IUTEST(util, Singleton)
{
	// Loose
	{
		using namespace test_loose;

		IUTEST_ASSERT( !A::ExistInstance() );
		{
			A a;

			// 関数テスト
			IUTEST_ASSERT_EQ( &a, &A::Instance() );
			IUTEST_ASSERT_EQ( &a, A::GetInstance() );
			A::CreateInstance(); // 意味なし
			IUTEST_ASSERT( A::ExistInstance() );
			A::DestroyInstance(); // 意味無し
		}
		IUTEST_ASSERT( !A::ExistInstance() );
	}
	
	// Loose Holder
	{
		using namespace test_loose;
		
		IUTEST_ASSERT_EQ( false, GlobalMainFoo::ExistInstance() );
		IUTEST_ASSERT_EQ( false, GlobalSubFoo::ExistInstance() );
		{
			GlobalMainFoo main;

			// 関数テスト
			IUTEST_ASSERT_EQ( &main.content, &GlobalMainFoo::Instance() );
			IUTEST_ASSERT_EQ( &main.content, GlobalMainFoo::GetInstance() );
			GlobalMainFoo::CreateInstance(); // 意味なし
			IUTEST_ASSERT_EQ( true, GlobalMainFoo::ExistInstance() );
			GlobalMainFoo::DestroyInstance(); // 意味無し
			
			GlobalSubFoo sub;

			// 関数テスト
			IUTEST_ASSERT_EQ( &sub.content, &GlobalSubFoo::Instance() );
			IUTEST_ASSERT_EQ( &sub.content, GlobalSubFoo::GetInstance() );
			GlobalSubFoo::CreateInstance(); // 意味なし
			IUTEST_ASSERT_EQ( true, GlobalSubFoo::ExistInstance() );
			GlobalSubFoo::DestroyInstance(); // 意味無し
		}
		IUTEST_ASSERT_EQ( false, GlobalSubFoo::ExistInstance() );
		IUTEST_ASSERT_EQ( false, GlobalMainFoo::ExistInstance() );
	}

	// StaticCreator
	{
		using namespace test_static;

		IUTEST_ASSERT_TRUE( A::ExistInstance() );

		A* a = A::CreateInstance();
		IUTEST_ASSERT_NE( nullptr, a );

		IUTEST_ASSERT_TRUE( A::ExistInstance() );
		
		IUTEST_ASSERT_EQ( a, A::GetInstance() );
		IUTEST_ASSERT_EQ( a, &A::Instance() );

		A::DestroyInstance();
		
		// StaticCreatorは、Destroy呼んでも開放されず、インスタンスは生きている
		IUTEST_ASSERT_TRUE( A::ExistInstance() );

		// アクセスクラス
		g_A->foo();
		a = g_A; // 暗黙のポインタキャスト
		IUTEST_ASSERT_NE( nullptr, a );
		IUTEST_ASSERT_EQ( a, g_A.get() );
		IUTEST_ASSERT_TRUE( g_A ); // boolキャスト
	}

	// DynamicCreator
	{
		using namespace test_dynamic;

		IUTEST_ASSERT_FALSE( A::ExistInstance() );

		A* a = A::CreateInstance();
		IUTEST_ASSERT_NE( nullptr, a );

		IUTEST_ASSERT_TRUE( A::ExistInstance() );
		
		IUTEST_ASSERT_EQ( a, A::GetInstance() );
		IUTEST_ASSERT_EQ( a, &A::Instance() );

		// アクセスクラス
		g_A->foo();
		a = g_A; // 暗黙のポインタキャスト
		IUTEST_ASSERT_NE( nullptr, a );
		IUTEST_ASSERT_EQ( a, g_A.get() );
		IUTEST_ASSERT_TRUE( g_A ); // boolキャスト

		A::DestroyInstance();
		IUTEST_ASSERT_FALSE( A::ExistInstance() );
	}
}
