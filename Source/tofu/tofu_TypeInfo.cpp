//------------------------------------------------------------------------------
/**
 * @file   TypeInfo.cpp
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <tofu_TypeInfo.h>
#include <iostream>

namespace tofu {

//------------------------------------------------------------------------------
// 対象型のvoid*を、指定のTypeInfoの型へアップキャストする
void* TypeInfo::TryUpcast(void* p, const TypeInfo& target_type_info) const noexcept
{
	// cvなしのTypeInfoで処理する
	if(isConst() || isVolatile())
	{
		return getRemoveCV().TryUpcast(p, target_type_info);
	}
	
	//std::cout << GetName() << " upcast : " <<  target_type_info.GetName() << std::endl;
	const auto* base_info = m_BaseClassInfo;
	while(base_info)
	{
		//std::cout << base_info->GetTypeInfo().GetName() << std::endl;

		if(void* ptr = base_info->TryCastTo(p, target_type_info)){
			return ptr;
		}
		base_info = base_info->GetNext();
	}
	return nullptr;
}
//------------------------------------------------------------------------------
const void* TypeInfo::TryUpcast(const void* p, const TypeInfo& target_type_info) const noexcept
{
	return TryUpcast(const_cast<void*>(p), target_type_info);
}

//------------------------------------------------------------------------------
bool TypeInfo::IsDerivedFrom(const TypeInfo& base_type_info_) const noexcept
{
	if(isConst() || isVolatile())
	{
		return getRemoveCV().IsDerivedFrom(base_type_info_);
	}
	// cv外す
	const auto& target = base_type_info_.getRemoveCV();

	// このクラス
	if(target == *this) {
		return true;
	}
	
	//std::cout << GetName() << " search : " <<  base_type_info_.GetName() << std::endl;
	const auto* base_info = m_BaseClassInfo;
	while(base_info)
	{
		//std::cout << base_info->GetTypeInfo().GetName() << std::endl;
		if(target == base_info->GetBaseTypeInfo()){
			return true;
		}
		// 登録済みの基底クラスを辿って調べる
		if(base_info->GetBaseTypeInfo().IsDerivedFrom(target)){
			return true;
		}
		base_info = base_info->GetNext();
	}

	return false;
}

//------------------------------------------------------------------------------
const detail::BaseClassInfo* TypeInfo::AddBaseInfo(const BaseClassInfo& info)
{
	auto next = m_BaseClassInfo;
	m_BaseClassInfo = &info;
	return next;
}

//------------------------------------------------------------------------------
} //tofu
