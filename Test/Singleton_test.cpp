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
#include <tofu/util/EasySingleton.hpp>

#include <iostream>
#include <iutest.hpp>

// Easyシングルトン
namespace test_easy
{
	// 継承するだけでOK
	class A : public tofu::EasySingleton<A>
	{
	};
}
class B {};
struct SubTag;

// EasyシングルトンのHolderテスト
using GlobalMainB = tofu::EasySingletonHolder<B>;
using GlobalSubB = tofu::EasySingletonHolder<B, SubTag>;

IUTEST(util, Singleton)
{
	// Easy
	{
		using namespace test_easy;

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
	
	// Easy Holder
	{
		using namespace test_easy;
		
		IUTEST_ASSERT_EQ( false, GlobalMainB::ExistInstance() );
		IUTEST_ASSERT_EQ( false, GlobalSubB::ExistInstance() );
		{
			GlobalMainB main;

			// 関数テスト
			IUTEST_ASSERT_EQ( &main.content, &GlobalMainB::Instance() );
			IUTEST_ASSERT_EQ( &main.content, GlobalMainB::GetInstance() );
			GlobalMainB::CreateInstance(); // 意味なし
			IUTEST_ASSERT_EQ( true, GlobalMainB::ExistInstance() );
			GlobalMainB::DestroyInstance(); // 意味無し
			
			GlobalSubB sub;

			// 関数テスト
			IUTEST_ASSERT_EQ( &sub.content, &GlobalSubB::Instance() );
			IUTEST_ASSERT_EQ( &sub.content, GlobalSubB::GetInstance() );
			GlobalSubB::CreateInstance(); // 意味なし
			IUTEST_ASSERT_EQ( true, GlobalSubB::ExistInstance() );
			GlobalSubB::DestroyInstance(); // 意味無し
		}
		IUTEST_ASSERT_EQ( false, GlobalSubB::ExistInstance() );
		IUTEST_ASSERT_EQ( false, GlobalMainB::ExistInstance() );
	}
}
