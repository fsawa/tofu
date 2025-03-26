//------------------------------------------------------------------------------
/**
 * @file    Reflection_test.cpp
 * @brief   リフレクションテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu_Reflection.h>
#include <tofu_TypeName.h>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	class Base
	{
	public:
		Base() = default;
		virtual ~Base()
		{
		}
	};

	class A : public Base
	{
	public:
		A() = default;
		virtual ~A()
		{
		}
	};

	class B : public Base
	{
	public:
		B() = default;
		virtual ~B()
		{
		}
	};

	// クラス継承関係登録
	TOFU_SET_BASE_TYPE(Base, A);
	TOFU_SET_BASE_TYPE(Base, B);

	enum E1{};
	enum class E2{};

	template<typename T, size_t N>
	class F;
	
	// 無名名前空間
	namespace
	{
		class D;
	}
}

// 無名名前空間
namespace
{
	class C;

	template <typename T>
	void PrintDebugTypeName()
	{
		std::cout << tofu::GetTypeName<T>() << std::endl;
		std::cout << "        " << tofu::TypeName<T>::TestValue.view() << std::endl;
	}
}

TOFU_REFLECTION_CLASS(test::A);
TOFU_REFLECTION_CLASS(test::B);

IUTEST(tofu, Reflection)
{
	std::cout << "-- Print --" << std::endl;
	PrintDebugTypeName<int>();
	PrintDebugTypeName<int&>();
	PrintDebugTypeName<int*>();
	PrintDebugTypeName<int**>();
	PrintDebugTypeName<int[]>();
	PrintDebugTypeName<int[3]>();
	PrintDebugTypeName<const int[3][4]>();
	PrintDebugTypeName<const volatile int&>();
	PrintDebugTypeName<test::A>();
	PrintDebugTypeName<const volatile test::A* const>();
	PrintDebugTypeName<C>();
	PrintDebugTypeName<test::D>();
	PrintDebugTypeName<test::E1>();
	PrintDebugTypeName<test::E2>();
	PrintDebugTypeName<test::F<const int, 1>>();
	PrintDebugTypeName<void(int)>();
	std::cout << "----" << std::endl;

	// 派生クラスを文字列から
	{
		auto ptr = tofu::reflection::Create("test::A");
		IUTEST_ASSERT_NE(ptr.get(), nullptr);
		std::cout << ptr.type().info().GetName() << std::endl;
		//delete ptr.get();
		//ptr.Clear();
	}
}
