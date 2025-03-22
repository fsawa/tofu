//------------------------------------------------------------------------------
/**
 * @file   mpl_Common.h
 * @brief  Meta Programming Library 共通定義
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
namespace mpl {
	
namespace detail
{
	// データ取り出し
	struct ToData {};
}

// データ取り出し
inline constexpr auto ToData = detail::ToData();

//------------------------------------------------------------------------------

} // mpl
} // tofu
