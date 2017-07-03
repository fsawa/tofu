//------------------------------------------------------------------------------
/**
 * @file    MathCommon.hpp
 * @brief   数学共通
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>

#include <cmath>

namespace tofu {
namespace math {

//------------------------------------------------------------------------------
// 定数

/// 定数群
template <typename T>
struct Konst
{
	/// 円周率
	static constexpr T pi = static_cast<T>(3.1415926535897932384626433832795);
	
	/// 円周率 x2
	static constexpr T pi2 = static_cast<T>(3.1415926535897932384626433832795 * 2);
	
	/// 円周率 / 2
	static constexpr T pi_div2 = static_cast<T>(3.1415926535897932384626433832795 / 2);
};

/// 円周率
template <typename T>
constexpr T Pi() { return Konst<T>::pi; }

//------------------------------------------------------------------------------

/// 絶対値
template <typename T>
constexpr T Abs( T x ) noexcept
{
	return x < 0 ? -x : x;
}

/// 剰余
inline float Mod( float x, float y )
{
	return ::fmod(x,y);
}

/// 剰余
inline double Mod( double x, double y )
{
	return ::fmod(x,y);
}

/// 剰余
inline long double Mod( long double x, long double y )
{
	return ::fmod(x,y);
}

/// 剰余
inline int Mod( int x, int y )
{
	return x % y;
}

} // math
} // tofu
