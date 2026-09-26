//------------------------------------------------------------------------------
/**
 * @file    tofu_Log.cpp
 * @brief   ログ出力マクロ関連
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2026 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

// VCでvsnprintfの警告抑制.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <tofu.h>

#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace tofu {
namespace base {

//-----------------------------------------------------------------------------

#if defined(TOFU_ENABLE_LOG)

namespace detail
{

	//------------------------------------------------------------------------------
	void FormatLog( std::string_view filename, int line, std::string_view msg )
	{
		std::format_to(std::ostream_iterator<char>(std::cout), 
                   "{}({}): {}\n", 
                   filename, line, msg);
	}

	//------------------------------------------------------------------------------
	void  Log(const char* filename, [[maybe_unused]] int line, const char* msg)
	{
		if (!filename) filename = "";
		if (!msg) msg = "";

		//Printf( "%s(%d): ASSERT: %s\n", filename, line, msg );
		
		const int kStrMax = 256;
		char str[kStrMax];

		const char* const format = "%s(%d): %s";
		snprintf(str, kStrMax, format, filename, line, msg);

		str[kStrMax - 1] = 0;

		std::cout << str << std::endl;
	}
	//------------------------------------------------------------------------------
	void  Logf(const char* filename, int line, const char* format, ...)
	{
		if (format)
		{
			const int kStrMax = 256;
			char str[kStrMax];
			va_list ap;

			va_start(ap, format);
			vsnprintf(str, kStrMax, format, ap);
			va_end(ap);

			str[kStrMax - 1] = 0;

			Log(filename, line, str);
		}
	}

} // detail

#endif // TOFU_ENABLE_ASSERT

} // base
} // tofu
