//------------------------------------------------------------------------------
/**
 * @file   TypeInfo.hpp
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

// clang
#if defined(__clang__)

/// @ingroup compiler
/// @brief	コンパイラを識別するマクロ
#define TOFU_COMPILER_IS_CLANG

/// @ingroup compiler
/// @brief	ファイル名マクロ
#define TOFU_FILE  __FILE__

/// @ingroup compiler
/// @brief	行番号を表すマクロ
#define TOFU_LINE  __LINE__

/// @ingroup compiler
/// @brief	関数のフルネームを表すマクロ
#define TOFU_FUNCTION_NAME  __PRETTY_FUNCTION__

// Miscrosoft Visual C++
#elif defined( _MSC_VER ) && defined(_WIN32)

	/// @ingroup compiler
	/// @brief	コンパイラを識別するマクロ
	#define TOFU_COMPILER_IS_MSVC

	/// @ingroup compiler
	/// @brief	ファイル名マクロ
	#define TOFU_FILE  __FILE__

	/// @ingroup compiler
	/// @brief	行番号を表すマクロ
	#define TOFU_LINE  __LINE__
	
	/// @ingroup compiler
	/// @brief	関数のフルネームを表すマクロ
	#define TOFU_FUNCTION_NAME  __FUNCTION__

	//------------------------------------------------------------------------------
	
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	//#include <windows.h>
	
	#ifdef max
	#undef max
	#endif
	
	#ifdef min
	#undef min
	#endif
	
	#ifdef far
	#undef far
	#endif
	
	#ifdef near
	#undef near
	#endif

//------------------------------------------------------------------------------
// No-Support
#else

	#error "No-Support Compiler."
	
#endif
