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
#include <tofu/util/Finalizer.hpp>
#include <tofu/util/LooseSingleton.hpp>

#include <iostream>
#include <iutest.hpp>

namespace
{
	int g_add_count = 0;
	int g_fin_count = 0;
	bool g_bad_order = false;
	
	class A
	{
	public:
		A()  {}
		~A() { std::cout << "~A" << std::endl;}
	};

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
		std::cout << "func1" << std::endl;
	}
}

IUTEST(util, Finalizer)
{
	{
		tofu::Finalizer fin;
		
		fin.add( new SingleA )->func();
		fin.addFunctor(func1);
		fin.addArray( new A[3] );
		fin.add( new SingleB )->func();
		
		g_fin_count = g_add_count;
		fin.finalize();

		IUTEST_ASSERT((g_bad_order == false));
		
		IUTEST_ASSERT_EQ( SingleA::ExistInstance(), false);
		IUTEST_ASSERT_EQ( SingleB::ExistInstance(), false);
	}
}
