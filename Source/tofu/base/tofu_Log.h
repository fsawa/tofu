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
	/// @brief 指定したメッセージを出力（printf書式）
	#define TOFU_LOGF( ... ) \
		(void)(::tofu::base::detail::Logf( TOFU_FILE, TOFU_LINE, __VA_ARGS__ ), 0)

	/// @ingroup log
	/// @brief 指定したメッセージを出力（format書式）
	#define TOFU_LOG( fmt, ... ) \
		::tofu::base::detail::FormatLog( TOFU_FILE, TOFU_LINE, ::tofu::base::detail::FormatLogBuffer().Make<fmt>(__VA_ARGS__) )

//------------------------------------------------------------------------------
#else

	#define TOFU_LOGF(...)   ((void)0)
	#define TOFU_LOG(...)   ((void)0)

#endif
//------------------------------------------------------------------------------


namespace tofu {
namespace base {

/// @cond

namespace detail
{
	// 固定長バッファにフォーマット文字列を構築するためのクラス
	template <size_t BufferSize = 256>
	class FormatLogBuffer
	{
	public:
		template <::tofu::StringData fmt, typename... Args>
		std::string_view Make(Args&&... args) noexcept
		{
			auto result = std::format_to_n(m_buffer.data(), m_buffer.size(), fmt.data, std::forward<Args>(args)...);
			return std::string_view(m_buffer.data(), std::min<size_t>(result.size, m_buffer.size()));
		}

	private:
		std::array<char, BufferSize> m_buffer;
	};
	
	void FormatLog( std::string_view filename, int line, std::string_view msg );

	void Log( const char* filename, int line, const char* msg );
	void Logf( const char* filename, int line, const char* format, ... );

}// detail
//-----------------------------------------------------------------------------

/// @endcond

}// base
}// tofu
