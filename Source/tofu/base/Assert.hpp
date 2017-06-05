//------------------------------------------------------------------------------
/**
 * @file    Assert.hpp
 * @brief   アサートマクロ関連
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

/// @defgroup assert アサートマクロ

//------------------------------------------------------------------------------
// 静的アサート
//------------------------------------------------------------------------------

/// @ingroup assert
/// @brief 配列サイズが意図したものかチェックする静的アサート
#define TOFU_ARRAY_SIZE_ASSERT( array, len ) \
	static_assert( tofu::countof(array) == len, "bad array size" )

//------------------------------------------------------------------------------
// 動的アサート
//------------------------------------------------------------------------------
#if defined(TOFU_ENABLE_ASSERT)

	/// @ingroup assert
	/// @brief アサートマクロ
	#define TOFU_ASSERT( exp ) \
		(void)( (exp) || (::tofu::base::detail::Assert( TOFU_FILE, TOFU_LINE, #exp ), 0) )
	
	/// @ingroup assert
	/// @brief 指定したメッセージを出力して停止するアサート
	#define TOFU_ASSERT_MSG( exp, ... ) \
		(void)( (exp) || (::tofu::base::detail::Assertf( TOFU_FILE, TOFU_LINE, __VA_ARGS__ ), 0) )
	
	/// @ingroup assert
	/// @brief 停止前に指定の処理を実行するアサート
	#define TOFU_ASSERT_CALL( exp, call_ ) \
		(void)( (exp) || ((call_), ::tofu::base::detail::Assert( TOFU_FILE, TOFU_LINE, #exp ), 0) )
	
	/// @ingroup assert
	/// @brief 到達すればエラー停止させるマクロ
	#define TOFU_ERROR()   ::tofu::base::detail::Assert( TOFU_FILE, TOFU_LINE )
	
	/// @ingroup assert
	/// @brief 指定したメッセージを出力して停止するエラー
	#define TOFU_ERROR_MSG(...)   ::tofu::base::detail::Assertf( TOFU_FILE, TOFU_LINE, __VA_ARGS__ )
	
	/// @ingroup assert
	/// @brief TOFU_ASSERTと同じだが、ASSERTが無効でも引数(exp)のコードを残す
	#define TOFU_VERIFY( exp )   TOFU_ASSERT( exp )
	
	/// @ingroup assert
	/// @brief アサート無効時に、アサートのためのコードを消すためのマクロ
	#define TOFU_FOR_ASSERT(...)   __VA_ARGS__

//------------------------------------------------------------------------------
#else

	#define TOFU_ASSERT(...)       ((void)0)
	#define TOFU_ASSERT_MSG(...)   ((void)0)
	#define TOFU_ASSERT_CALL(...)  ((void)0)
	#define TOFU_ERROR(...)        ((void)0)
	#define TOFU_ERROR_MSG(...)    ((void)0)
	
	#define TOFU_VERIFY( exp )  ::tofu::base::VerifyDummyFunc( static_cast<bool>(exp) )
	#define TOFU_FOR_ASSERT(...)

#endif
//------------------------------------------------------------------------------


namespace tofu {
namespace base {

/// プログラムを停止させます。
void  Halt();

/// @cond

namespace detail
{
	void  Assert( const char* filename, int line, const char* msg );
	void  Assert( const char* filename, int line );
	void  Assertf( const char* filename, int line, const char* format, ... );

	inline void  VerifyDummyFunc( bool ) {}

}// detail
//-----------------------------------------------------------------------------

/// @endcond

}// base
}// tofu
