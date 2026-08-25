//------------------------------------------------------------------------------
/**
 * @file    tofu_Concepts.h
 * @brief   汎用コンセプト定義
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2026 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <type_traits>
#include <concepts>

namespace tofu {
	
// cv修飾なしか
template <class T>
concept not_cv =
	!std::is_const_v<T> && !std::is_volatile_v<T>;

// cv修飾がcast可能な関係か
template <class From, class To>
concept castable_cv_to =
	!(std::is_const_v<From> && !std::is_const_v<To>) &&
	!(std::is_volatile_v<From> && !std::is_volatile_v<To>);

/// @brief 安全にcast可能か
/// @note 
/// ・cv修飾がキャスト可能
/// ・アップキャスト
template <class From, class To>
concept safe_castable_to =
	castable_cv_to<From, To> && 
	std::derived_from<From, To>;

} // tofu