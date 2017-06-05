//------------------------------------------------------------------------------
/**
 * @file    Platform.hpp
 * @brief   プラットフォーム毎の定義
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
// プラットフォーム識別用マクロ

#if defined(_WIN32)
#define TOFU_PLATFORM_IS_WIN
#else
#error "No-Support Platform"
#endif

//------------------------------------------------------------------------------
/// @name  for Windows
//@{
#ifdef TOFU_PLATFORM_IS_WIN

/// @ingroup platform
/// @brief リトルエンディアンの場合に定義されるマクロ
#define TOFU_LITTLE_ENDIAN
//#define TOFU_BIG_ENDIAN

#if defined(_WIN64) || defined(WIN64)
/// @ingroup platform
/// @brief	64bit環境の場合に定義されるマクロ
#define TOFU_BIT64
#else
/// @ingroup platform
/// @brief	32bit環境の場合に定義されるマクロ
#define TOFU_BIT32
#endif

#endif // TOFU_PLATFORM_IS_WIN
//@}

//------------------------------------------------------------------------------
