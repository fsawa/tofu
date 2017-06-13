//------------------------------------------------------------------------------
/**
 * @file    Assert.cpp
 * @brief   アサートマクロ関連
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

// VCでvsnprintfの警告抑制.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <tofu.hpp>

#include <cstdarg>
#include <cstdio>

namespace tofu {
namespace base {

//------------------------------------------------------------------------------
void  Halt()
{
#if defined(TOFU_PLATFORM_IS_WIN)
	__debugbreak();
#endif
}

//-----------------------------------------------------------------------------

#if defined(TOFU_ENABLE_ASSERT)

namespace detail
{

	//------------------------------------------------------------------------------
	void  Assert(const char* filename, int line, const char* msg)
	{
		if (!filename) filename = "";
		if (!msg) msg = "";

		/// @todo
		//Printf( "%s(%d): ASSERT: %s\n", filename, line, msg );

		Halt();
	}
	//------------------------------------------------------------------------------
	void  Assert(const char* filename, int line)
	{
		Assert(filename, line, nullptr);
	}
	//------------------------------------------------------------------------------
	void  Assertf(const char* filename, int line, const char* format, ...)
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

			Assert(filename, line, str);
		}
	}

} // detail

#endif // TOFU_ENABLE_ASSERT

} // base
} // tofu
