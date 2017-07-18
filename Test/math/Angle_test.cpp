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
#include <cfloat>
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
	IUTEST_ASSERT_GT( 0.001f, Abs((a % 0.5f).value() - 0.1f) );
	a = Angle(-2.6f);
	IUTEST_ASSERT_GT( 0.001f, Abs((a % 0.5f).value() + 0.1f) );
	
	IUTEST_ASSERT_EQ( 1.0f, (Angle(0.5f) + Degree(180.f)).value() );
	
	// 正規化 [0.0～1.0)の範囲に変換 = [0度～360度)
	a = Angle(2.25f).normalize();
	IUTEST_ASSERT_EQ( 0.25f, a.value() );
	IUTEST_ASSERT_EQ( 0.0f, Angle(0.0f).normalize().value() );
	IUTEST_ASSERT_EQ( 0.0f, Angle(1.0f).normalize().value() );
	IUTEST_ASSERT_EQ( 0.0f, Angle(-1.0f).normalize() );
	IUTEST_ASSERT_EQ( 350.f, Degree(-10.f).normalize() );
	IUTEST_ASSERT_EQ( 10.f, Degree(10.f).normalize() );
	IUTEST_ASSERT_GE( FLT_EPSILON*100, Abs(Degree(370.f).normalize().value() - 10.0f) );
	
	a = Angle(-2.25f).normalize();
	IUTEST_ASSERT_EQ( 0.75f, a.value() );
	
	a = Angle(2.0f).normalize();
	IUTEST_ASSERT_EQ( 0.f, a.value() );

	a = Angle(-2.0f).normalize();
	IUTEST_ASSERT_EQ( 0.f, a.value() );
	
	// 正と負それぞれの方向で正規化 (-1.0～1.0) = (-360度～360度)
	a = Angle(2.25f).signedNormalize();
	IUTEST_ASSERT_EQ( 0.25f, a.value() );
	
	a = Angle(-2.25f).signedNormalize();
	IUTEST_ASSERT_EQ( -0.25f, a.value() );
	
	a = Angle(2.0f).signedNormalize();
	IUTEST_ASSERT_EQ( 0.f, a.value() );

	a = Angle(-2.0f).signedNormalize();
	IUTEST_ASSERT_EQ( 0.f, a.value() );
	
	// 角度差（最短距離）[-180度～180度)
	{
		IUTEST_ASSERT_EQ( -0.5f, Angle(0.5f).distanceFrom( Angle(1.0f) ).value() );
		IUTEST_ASSERT_EQ( -0.5f, Angle(0.5f).distanceFrom( Angle(-1.0f) ).value() );
		
		IUTEST_ASSERT_EQ( 0.25f, Angle(1.5f).distanceFrom( Angle(4.25f) ).value() );
		IUTEST_ASSERT_EQ( -0.25f, Angle(6.0f).distanceFrom( Angle(4.25f) ).value() );
		IUTEST_ASSERT_EQ( 0.0f, Angle(-3.75f).distanceFrom( Angle(4.25f) ).value() );
		IUTEST_ASSERT_EQ( -0.5f, Angle(-3.25f).distanceFrom( Angle(4.25f) ).value() );
	}
	
	// 0度からの角度差（最短距離）[-180度～180度)
	IUTEST_ASSERT_EQ( 0.25f, Angle(1.25f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( -0.25f, Angle(-1.25f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( 0.25f, Angle(-1.75f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( -0.5f, Angle(-1.5f).distanceFromZero().value() );
	IUTEST_ASSERT_EQ( -0.5f, Angle(1.5f).distanceFromZero().value() );

	// 反射
	{
		auto diff = MakeDegree(10.f).reflect( MakeDegree(20.f) ) - MakeDegree(30.f);
		// 誤差が出るのである程度許容
		IUTEST_ASSERT_GE( FLT_EPSILON, diff.value() );
	}
	{
		auto diff = MakeDegree(0.f).reflect( MakeDegree(180.f) ) - MakeDegree(-360.f);
		// 誤差が出るのである程度許容
		IUTEST_ASSERT_GE( FLT_EPSILON, diff.value() );
	}
	{
		auto diff = MakeDegree(0.f).reflect( MakeDegree(170.f) ) - MakeDegree(340.f);
		// 誤差が出るのである程度許容
		IUTEST_ASSERT_GE( FLT_EPSILON, diff.value() );
	}
	{
		auto diff = MakeDegree(350.f).reflect( MakeDegree(10.f) ) - MakeDegree(390.f);
		// 誤差が出るのである程度許容
		IUTEST_ASSERT_GE( FLT_EPSILON, diff.value() );
	}

	// 
	{
		MakeRadian(10.f);
		MakeDegree(10.f);
		MakeAngle(10.f);
		
		// radian to angle
		{
			Angle a2 = Radian::One();
			IUTEST_ASSERT_EQ( 1.f, a2.value() );
			Radian x = a2;
			IUTEST_ASSERT_EQ( Radian::One().value(), x.value() );
		}
		// degree to angle
		{
			Angle a2 = Degree::One();
			IUTEST_ASSERT_EQ( 1.f, a2.value() );
			Degree x = a2;
			IUTEST_ASSERT_EQ( Degree::One().value(), x.value() );
		}
		// degree to radian
		{
			Radian a2 = Degree::One();
			IUTEST_ASSERT_EQ( Radian::One().value(), a2.value() );
			Degree x = a2;
			IUTEST_ASSERT_EQ( Degree::One().value(), x.value() );
		}
	}

	// constexpr
	{
		constexpr Angle a2 = Angle(1.f) + Degree::One();
		static_assert( a2.toDegree().toRadian().toAngle().value() == 2.0f, "constexpr Angle");
		static_assert( a2 == Angle(2.0f), "constexpr Angle");
	}
	
	// 三角関数
	{
		Degree v( 0.f );
		IUTEST_ASSERT_EQ( 0.f, v.sin() );
		IUTEST_ASSERT_EQ( 1.f, v.cos() );
		IUTEST_ASSERT_EQ( 0.f, v.tan() );
				
		v.value(30.f);
		IUTEST_ASSERT_EQ( 0.5f, v.sin() );
		//IUTEST_ASSERT_EQ( 0.f, v.cos() );
		//IUTEST_ASSERT_EQ( 0.f, v.tan() );
		
		v.value(90.f);
		IUTEST_ASSERT_GT( 0.001f, v.sin() - 1.f );
		IUTEST_ASSERT_GT( 0.001f, v.cos() - 0.f );
		//IUTEST_ASSERT_EQ( 0.f, v.tan() );
		
		v.value(180.f);
		IUTEST_ASSERT_GT( 0.001f, v.sin() - 0.f );
		IUTEST_ASSERT_GT( 0.001f, v.cos() + 1.f );
		IUTEST_ASSERT_GT( 0.001f, v.tan() - 0.f );
		
		v.value(270.f);
		IUTEST_ASSERT_GT( 0.001f, v.sin() + 1.f );
		IUTEST_ASSERT_GT( 0.001f, v.cos() - 0.f );
		//IUTEST_ASSERT_EQ( 0.f ,v.tan() );
	}

	// 角度計算
	{
		IUTEST_ASSERT_EQ( 0.0f, CalcAngle(0.f, 0.f).value() );
		IUTEST_ASSERT_EQ( 0.5f, CalcAngle(0.f, -0.f).value() );
		IUTEST_ASSERT_GT( 0.001f, CalcAngle(0.f, 10.f).value() - 0.25f );
		IUTEST_ASSERT_GT( 0.001f, CalcAngle(0.f, -10.f).value() - 0.75f );
		IUTEST_ASSERT_GT( 0.001f, CalcAngle(1.f, 1.f).toDegree().value() - 45.0f );
		//IUTEST_ASSERT_GT( 0.001f, CalcAngle(0.f, 0.f) - 0.f );
	}
	
	// ユーザー定義リテラル
}
