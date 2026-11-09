//------------------------------------------------------------------------------
/**
 * @file    Finalizer_test.cpp
 * @brief   Finalizerテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu_Finalizer.h>
#include <tofu_LooseSingleton.h>

#include <iostream>
#include <iutest.hpp>

namespace
{
	int g_add_count = 0;
	int g_fin_count = 0;
	bool g_bad_order = false;
	
	class SingleA : public tofu::LooseSingleton<SingleA>
	{
		int m_count;
	public:
		SingleA() : m_count(g_add_count++) {}
		~SingleA() { std::cout << "~SingleA" << std::endl; if(m_count != --g_fin_count){g_bad_order = true;} }
		void func() { std::cout << "SingleA::func" << std::endl; }
	};
	class SingleB : public tofu::LooseSingleton<SingleB>
	{
		int m_count;
	public:
		SingleB() : m_count(g_add_count++) {}
		~SingleB() { std::cout << "~SingleB" << std::endl; if(m_count != --g_fin_count){g_bad_order = true;} }
		void func() { std::cout << "SingleB::func" << std::endl; }
	};

	static void func1()
	{
		TOFU_LOG("func1\n");
	}
}

IUTEST(util, Finalizer)
{
	{
		tofu::Finalizer fin;
		
		auto single_a = std::make_shared<SingleA>();
		single_a->func();
		//fin.Push( std::move(single_a) );
		fin.Push( single_a );
		single_a.reset();

		fin.PushFunctor(func1);

		fin += std::make_unique<SingleB>();
		SingleB::Instance().func();
		
		IUTEST_ASSERT_EQ( SingleA::ExistInstance(), true);
		IUTEST_ASSERT_EQ( SingleB::ExistInstance(), true);

		g_fin_count = g_add_count;
		fin.Finalize();

		IUTEST_ASSERT((g_bad_order == false));
		
		IUTEST_ASSERT_EQ( SingleA::ExistInstance(), false);
		IUTEST_ASSERT_EQ( SingleB::ExistInstance(), false);
	}
}
