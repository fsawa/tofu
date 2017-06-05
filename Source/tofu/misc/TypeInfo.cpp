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

#include <tofu/misc/TypeInfo.hpp>

namespace tofu {

const char* TypeInfo::ParseTypeName( char* str ) noexcept
{
// Microsoft Visual C++
#if defined(_MSC_VER)
	// tofu::TTypeInfo<struct XXX>::TTypeInfo
	// という形式のstrが来る。
	int begin = 0;
	for( ; str[begin]; ++begin )
	{
		if( str[begin] == ' ' )
		{
			++begin;
			break;
		}
	}
	int end = begin+1;
	for( ; str[end]; ++end )
	{
		if( str[end] == '>' )
		{
			str[end] = 0; // null終端化
			break;
		}
	}
	return str + begin;
#else
	/// @todo 
	return str;
#endif
}

} // tofu
