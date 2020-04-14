//------------------------------------------------------------------------------
/**
 * @file    SingletonPolicy.hpp
 * @brief   
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

namespace tofu {
namespace singleton {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      静的インスタンス生成用ポリシークラス
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class StaticCreator
{
//**************************************************************
//              : public
//**************************************************************
public:
	
// FUNCTION
	
	StaticCreator() : m_instance() {}
	/*virtual*/ ~StaticCreator() {}
	
	/// インスタンス取得
	T&    Instance()      { return m_instance; }
	
	/// インスタンス生成
	T*    CreateInstance()   { return &m_instance; }
	
	/// インスタンス存在チェック
	bool  ExistInstance()    { return true; }
	
	/// インスタンス破壊
	void  DestroyInstance()  {}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
	T  m_instance;
};
// << StaticCreator

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      動的インスタンス生成用ポリシークラス
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class DynamicCreator
{
//**************************************************************
//              : public
//**************************************************************
public:
	
// FUNCTION
	
	DynamicCreator() {}
	/*virtual*/ ~DynamicCreator() {}
	
	/// インスタンス取得（自動でインスタンス生成はしません）
	T&    Instance()      { TOFU_ASSERT(m_instance); return *m_instance; }
	
	/// インスタンス生成
	T*    CreateInstance()
	{
		if( !m_instance ){
			m_instance = new T;
		}
		return m_instance;
	}
	
	/// インスタンス存在チェック
	bool  ExistInstance()    { return nullptr != m_instance; }
	
	/// インスタンス破壊
	void  DestroyInstance()
	{
		delete m_instance;
		m_instance = NULL;
	}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
	T*  m_instance = nullptr;
};
// << DynamicCreator

} // singleton
} // tofu
