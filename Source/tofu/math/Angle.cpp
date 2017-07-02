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

// インスタンス化
template class BasicAngle<float>;
template class BasicAngle<double>;
template class BasicAngle<long double>;

} // math
} // tofu
