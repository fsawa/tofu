//------------------------------------------------------------------------------
/**
 * @file    countof.hpp
 * @brief   配列の要素数取得
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <cstddef>

namespace tofu {

/// 配列の要素数を取得する
template <class T, std::size_t N>
constexpr std::size_t  countof( const T (&a)[N] ) noexcept
{
	return N;
}

} // tofu
