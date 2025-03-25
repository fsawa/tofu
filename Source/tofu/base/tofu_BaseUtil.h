//------------------------------------------------------------------------------
/**
 * @file    tofu_BaseUtil.h
 * @brief   基本ユーティリティ
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

namespace tofu {
	
/// プリプロセッサ：連結
#define TOFU_JOIN(a, b) TOFU_JOIN2(a, b)

/// Static Initializer で指定した関数を呼び出すマクロ
#define TOFU_STATIC_CALL(...)  TOFU_STATIC_CALL_1(__LINE__, (__VA_ARGS__))

//------------------------------------------------------------------------------
	
#define TOFU_JOIN2(a, b) a ## b

namespace detail
{
	class StaticCall
	{
	public:
		template <typename Functor>
		StaticCall(Functor&& func) { func(); }
	};
}

#define TOFU_STATIC_CALL_1(line, func)  TOFU_STATIC_CALL_2(line, func)
#define TOFU_STATIC_CALL_2(line, func)  static const ::tofu::detail::StaticCall si_ ## line ( func )

}// tofu
