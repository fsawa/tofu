//------------------------------------------------------------------------------
/**
 * @file    Angle.cpp
 * @brief   角度を扱う機能
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <tofu/math/Angle.hpp>

namespace tofu {
namespace math {

//------------------------------------------------------------------------------
template <typename T>
BasicAngle<T>  BasicAngle<T>::distanceFrom( self_type from ) const noexcept
{
	self_type diff = *this - from;
	// 0.0～1.0に正規化
	diff = diff.normalize();
	// -0.5～0.5に変換
	if( diff >= Half() ){
		diff -= One();
	}
	return diff;
}

//------------------------------------------------------------------------------

// インスタンス化
template class BasicAngle<float>;
template class BasicAngle<double>;
template class BasicAngle<long double>;

} // math
} // tofu
