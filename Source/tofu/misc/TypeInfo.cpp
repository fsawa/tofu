//------------------------------------------------------------------------------
/**
 * @file   TypeInfo.cpp
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <tofu.hpp>
#include <tofu/misc/TypeInfo.hpp>

// 文字列のプリントテスト
//#define TEST_PRINT

#ifdef TEST_PRINT
#include <iostream>
#endif

namespace tofu {

const char* TypeInfo::ParseTypeName(char* dst, const char* src, int len) noexcept
{
#ifdef TEST_PRINT
	std::cout << src << std::endl;
	std::cout << len << std::endl;
#endif

// clang
#if defined(TOFU_COMPILER_IS_CLANG)
// tofu::TTypeInfo<int>::TTypeInfo() [T = int]
// という形式の文字列が来る
	int begin = 0;
	for (; src[begin]; ++begin)
	{
		if (src[begin] == '=')
		{
			begin += 2;
			break;
		}
	}
	int end = begin;
	int count = 0;
	for (; end < len && src[end]; ++end, ++count)
	{
		dst[count] = src[end];
		if (src[end] == ']')
		{
			dst[count] = 0; // null終端化
			break;
		}
	}
#ifdef TEST_PRINT
	std::cout << dst << std::endl;
#endif
	return dst;
// Microsoft Visual C++
#elif defined(_MSC_VER)
// tofu::TTypeInfo<struct XXX>::TTypeInfo
// という形式の文字列が来る
	int begin = 0;
	for (; src[begin]; ++begin)
	{
		if (src[begin] == ' ')
		{
			++begin;
			break;
		}
	}
	int end = begin + 1;
	int count = 0;
	for (; src[end]; ++end, ++count)
	{
		dst[count] = src[end];
		if (src[end] == '>')
		{
			dst[count] = 0; // null終端化
			break;
		}
	}
	return dst;
#else
/// @todo 
	return dst;
#endif
}

} // tofu
