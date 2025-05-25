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

namespace tofu {

//------------------------------------------------------------------------------
// 対象型のvoid*を、指定のTypeInfoの型へアップキャストする
void* TypeInfo::TryUpcast(void* p, const TypeInfo& target_type_info) const noexcept
{
	if(m_UpcastFunc){
		if(void* ptr = m_UpcastFunc(p, target_type_info)){
			return ptr;
		}
	}
	for(auto& it : m_BaseInfoMap)
	{
		if(void* ptr = it.second(p, target_type_info)){
			return ptr;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
bool TypeInfo::IsDerivedFrom(const TypeInfo& base_info) const noexcept
{
	if(isConst() || isVolatile())
	{
		return getRemoveCV().IsDerivedFrom(base_info);
	}
	// cv外す
	const auto* ptr = &base_info.getRemoveCV();

	// このクラス
	if(ptr == this) {
		return true;
	}
	// 登録済みの基底クラスに一致
	else if(ptr == m_BaseInfo) {
		return true;
	}
	else if(m_BaseInfoMap.contains(ptr)){
		return true;
	}
		
	// 登録済みの基底クラスを辿って調べる
	if(m_BaseInfo && m_BaseInfo->IsDerivedFrom(base_info)) {
		return true;
	}
	for(auto& it : m_BaseInfoMap)
	{
		if(it.first->IsDerivedFrom(base_info)){
			return true;
		}
	}

	return false;
}

} //tofu
