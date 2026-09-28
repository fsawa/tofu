//------------------------------------------------------------------------------
/**
 * @file    tofu_Log.h
 * @brief   ログ出力マクロ関連
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2026 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <array>
#include <string_view>
#include <format>

#include <tofu_StringData.h>

/// @defgroup log ログ出力マクロ

//------------------------------------------------------------------------------
// 動的ログ出力
//------------------------------------------------------------------------------
#if defined(TOFU_ENABLE_LOG)

	/// @ingroup log
	/// @brief 指定したメッセージを出力（format書式）
	#define TOFU_LOG( fmt, ... ) \
		::tofu::base::detail::FormatLog<fmt>( TOFU_FILE, TOFU_LINE, __VA_ARGS__ )

	/// @ingroup log
	/// @brief 指定したメッセージを出力（printf書式）
	#define TOFU_LOGF( ... ) \
		(void)(::tofu::base::detail::Logf( TOFU_FILE, TOFU_LINE, __VA_ARGS__ ), 0)


//------------------------------------------------------------------------------
#else

	#define TOFU_LOG(...)   ((void)0)
	#define TOFU_LOGF(...)   ((void)0)

#endif
//------------------------------------------------------------------------------


namespace tofu {
namespace base {

/// @cond

namespace detail
{
	void Log( std::string_view filename, int line, std::string_view msg );
	
	template <::tofu::StringData fmt, typename... Args>
	inline void FormatLog( std::string_view filename, int line, Args&&... args )
	{
		::tofu::StringData<256-1> buffer{};
		Log(filename, line, buffer.Format<fmt>(std::forward<Args>(args)...).view());
	}

	void Log_old( const char* filename, int line, const char* msg );
	void Logf( std::string_view filename, int line, const char* format, ... );

}// detail
//-----------------------------------------------------------------------------

/// @endcond

}// base
}// tofu
