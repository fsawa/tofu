//------------------------------------------------------------------------------
/**
 * @file   tofu_BaseClassInfo.cpp
 * @brief  基底クラス情報
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <detail/tofu_BaseClassInfo.h>
#include <tofu_TypeInfo.h>
#include <iostream>

namespace tofu {
namespace rtti_detail {

//------------------------------------------------------------------------------
BaseClassInfo::BaseClassInfo(TypeInfo& derived, const TypeInfo& base)
	: m_BaseTypeInfo(base)
	, m_Next{ derived.AddBaseInfo(*this) }
{
	std::cout << "BaseClassInfo: " << derived.GetName() << " from " << base.GetName() << std::endl;
}

//------------------------------------------------------------------------------
} //rtti_detail
} //tofu
