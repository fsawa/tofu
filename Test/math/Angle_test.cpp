//------------------------------------------------------------------------------
/**
 * @file    Angle_test.cpp
 * @brief   角度処理テスト
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/math/Angle.hpp>

#include <iostream>
#include <iutest.hpp>

tofu::math::Angle TestFunc_Angle( tofu::math::Angle a ) noexcept
{
	return a;
}

#if 0
tofu::math::Radian TestFunc_Radian( tofu::math::Radian a ) noexcept
{
	return a;
}

tofu::math::Degree TestFunc_Degree( tofu::math::Degree a ) noexcept
{
	return a;
}
#endif

IUTEST(math, Angle)
{
	using namespace tofu::math;
	Angle  a;
	
	// ctor
	{
		Angle(0.5f);
		Angle( a );
		
		Angle a2( 0.5f );
	}
	
	// 値を直接代入　あまり推奨しない
	a.value(0.5f);
	IUTEST_ASSERT_EQ( 0.5f, a.value() );
	
	// こちらの代入を推奨　aの型が変更になってもそのまま使える
	a = Angle(0.5f);
	IUTEST_ASSERT_EQ( 0.5f, a.value() );
	
	// 絶対値
	a = Angle(-0.25f).abs();
	IUTEST_ASSERT_EQ( 0.25f, a.value() );
	
	// 四則演算
	a = Angle(2.f);
	IUTEST_ASSERT_EQ( 2.5f, (a + Angle(0.5f)).value() );
	IUTEST_ASSERT_EQ( 1.5f, (a - Angle(0.5f)).value() );
	IUTEST_ASSERT_EQ( 1.0f, (a * 0.5f).value() );
	IUTEST_ASSERT_EQ( 1.0f, (a / 2.0f).value() );
	a = Angle(2.6f);
	IUTEST_ASSERT_EQ( 0.1f, (a % 0.5f).value() );
	a = Angle(-2.6f);
	IUTEST_ASSERT_EQ( 0.1f, (a % 0.5f).value() );
	
	#if 0
	// 正規化 [-360度～360度)の範囲に変換
	a = Angle(2.25f).normalize();
	IUTEST_ASSERT_EQ( 0.25f, a.value() );
	
	a = Angle(-2.25f).normalize();
	IUTEST_ASSERT_EQ( -0.25f, a.value() );
	
	// 正の正規化 [0度～360度)の範囲に変換
	a = Angle(2.25f).positiveNormalize();
	IUTEST_ASSERT_EQ( 0.25f, a.value() );
	
	a = Angle(-2.25f).positiveNormalize();
	IUTEST_ASSERT_EQ( 0.75f, a.value() );
	
	// 角度差（最短距離）[-180度～180度)
	{
		IUTEST_ASSERT_EQ( 0.5f, Angle(0.5f).distanceFrom( Angle(1.0f) ).value() );
		IUTEST_ASSERT_EQ( 0.5f, Angle(0.5f).distanceFrom( Angle(-1.0f) ).value() );
	}
	
	// 0度からの角度差（最短距離）[-180度～180度)
	IUTEST_ASSERT_EQ( 0.25f, Angle(1.25f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( 0.25f, Angle(-1.25f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( 0.25f, Angle(-1.75f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( -0.5f, Angle(-1.5f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( -0.5f, Angle(1.5f).distanceFromZero().value() );
	
	// 三角関数
	
	// 定数
	Angle::kHalf;
	Angle::k180;
	Angle::k90;
	Angle::k60;
	Angle::k45;
	Angle::k30;
	
	#endif
	
	// ユーザー定義リテラル
}
