//------------------------------------------------------------------------------
/**
 * @file   tofu_TypeTraits.h
 * @brief  自前の型特性クラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

namespace tofu {
	
// Fromのconst修飾をToに反映する
template <class From, class To>
struct copy_const_to
{
	using type = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, std::remove_const_t<To>>;
};

template <class From, class To>
using copy_const_to_t = typename copy_const_to<From, To>::type;

// Fromのvolatile修飾をToに反映する
template <class From, class To>
struct copy_volatile_to
{
	using type = std::conditional_t<std::is_volatile_v<From>, std::add_volatile_t<To>, std::remove_volatile_t<To>>;
};

template <class From, class To>
using copy_volatile_to_t = typename copy_volatile_to<From, To>::type;

// Fromのcv修飾をToに反映する
template <class From, class To>
struct copy_cv_to
{
	using type = copy_volatile_to_t<From, copy_const_to_t<From, To>>;
};

template <class From, class To>
using copy_cv_to_t = typename copy_cv_to<From, To>::type;

} // tofu
