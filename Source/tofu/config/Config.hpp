//------------------------------------------------------------------------------
/**
 * @file    Config.hpp
 * @brief   
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

// test
#define TOFU_BUILD_IS_DEBUG
//#define TOFU_BUILD_IS_RELEASE
//#define TOFU_BUILD_IS_FINAL

//------------------------------------------------------------------------------
// ビルドターゲット
//------------------------------------------------------------------------------

// TOFU_BUILD_IS_DEBUG
//   フルデバッグ版。
// TOFU_BUILD_IS_RELEASE
//   開発版。最適化オプションが有効。
// TOFU_BUILD_IS_FINAL
//   製品版。すべてのデバッグ機能がオフ。

// ターゲット定義チェック
#if defined(TOFU_BUILD_IS_DEBUG) || defined(TOFU_BUILD_IS_RELEASE) || defined(TOFU_BUILD_IS_FINAL)
#else
#error "Need to define BUILD-TARGET."
//#define TOFU_BUILD_IS_DEBUG
#endif

//------------------------------------------------------------------------------
// DEBUG
//------------------------------------------------------------------------------
#if defined(TOFU_BUILD_IS_DEBUG)

/// デバッグ機能の有効
#define TOFU_DEBUG

#define TOFU_ENABLE_PRINT
#define TOFU_ENABLE_ASSERT
#define TOFU_ENABLE_WARNING

#endif

//------------------------------------------------------------------------------
// DEVELOP
//------------------------------------------------------------------------------
#if defined(TOFU_BUILD_IS_RELEASE)

/// デバッグ機能の有効
#define TOFU_DEBUG

#define TOFU_ENABLE_PRINT
#define TOFU_ENABLE_ASSERT
#define TOFU_ENABLE_WARNING

#endif

//------------------------------------------------------------------------------
// FINAL
//------------------------------------------------------------------------------
#if defined(TOFU_BUILD_IS_FINAL)

/// デバッグ機能の有効
//#define TOFU_DEBUG

//#define TOFU_ENABLE_PRINT
//#define TOFU_ENABLE_ASSERT
//#define TOFU_ENABLE_WARNING

#endif
