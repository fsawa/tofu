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
