//------------------------------------------------------------------------------
/**
 * @file    Nearly_test.cpp
 * @brief   ほぼ等しい判定のテスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/math/Nearly.hpp>

//#include <cfloat>
#include <iostream>
#include <iomanip>
#include <iutest.hpp>

IUTEST(math, Nearly)
{
	float a = 0.f;
	float b = 0.f;
	
	for(int i=0; i<10; ++i){
		a += 0.1f;
	}
	for(int i=0; i<1000; ++i){
		b += 0.1f;
	}
	
	std::cout << std::fixed;
	
	std::cout << std::setprecision(std::numeric_limits<float >::max_digits10)
		<< FLT_EPSILON << " // epsilon" << std::endl;
	
	std::cout << std::setprecision(std::numeric_limits<float >::max_digits10)
		<< FLT_EPSILON*100 << " // epsilon*100" << std::endl;
	
	std::cout << std::setprecision(std::numeric_limits<float >::max_digits10)
		<< (a - 1.f) << " ( " << a << std::endl;
	
	std::cout << std::setprecision(std::numeric_limits<float >::max_digits10)
		<< (b - 100.f) << " ( " << b << std::endl;

	// 誤差が出ることを確認
	IUTEST_ASSERT_NE( a, 1.f );
	IUTEST_ASSERT_NE( b, 100.f );

	// Nearlyの出番
	
	IUTEST_ASSERT_NE( 1.f, a ); // 1.fと比較NG
	IUTEST_ASSERT_NE( tofu::math::Nearly(1.f, 0.f), a ); // 許容誤差が小さすぎてもNG
	IUTEST_ASSERT_EQ( tofu::math::Nearly(1.f), a );
	//IUTEST_ASSERT_EQ( tofu::math::Nearly(1.f).tolerance(0.001f), a );
	//IUTEST_ASSERT_EQ( tofu::math::Nearly(1.f).epsilonRate(100.f), a );

	// オブジェクト化して利用
	{
		auto nearly = tofu::math::NearlyChecker<float>(0.0001f);
		
		IUTEST_ASSERT_EQ( nearly.set(1.f), a );
		IUTEST_ASSERT_EQ( nearly.make(1.f), a );
	}

}
