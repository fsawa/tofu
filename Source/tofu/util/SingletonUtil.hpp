//------------------------------------------------------------------------------
/**
 * @file    SingletonUtil.hpp
 * @brief   シングルトンの補助クラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>

namespace tofu {
namespace singleton {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      シングルトンへの安全なアクセスクラス
/// @details
/// よくあるグローバルポインタ変数の代替え用クラス。<br>
/// ポインタアクセスする度にnullチェックを行う。
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename InstanceType, typename SingletonType>
class SingletonAccessor
{
	typedef SingletonAccessor  self_type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
	using instance_type = InstanceType;
	using singleton_type = SingletonType;
	
// FUNCTION
	
	SingletonAccessor() {}
	//virtual ~SingletonAccessor();
	
	/// 参照
	instance_type*  operator->() const  { return _safeGetPtr(); }
	
	/// 参照
	instance_type&  operator*() const  { return *_safeGetPtr(); }
	
	/// 生のポインタへキャスト
	operator instance_type*() const  { return _safeGetPtr(); }
	
	/// 生のポインタ取得（存在しないときはアサート）
	instance_type*  get() const { return _safeGetPtr(); }
	
	/// インスタンス存在チェック
	bool  exist() const  { return nullptr != _getPtr(); }
	
	/// インスタンス存在チェック
	operator bool() const  { return exist(); }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// FUNCTION

	instance_type* _getPtr() const
	{
		if( singleton_type::ExistInstance() ){
			m_lastPtr = &singleton_type::Instance();
		}
		else{
			m_lastPtr = nullptr;
		}
		return m_lastPtr;
	}
	
	instance_type* _safeGetPtr() const
	{
		_getPtr();
		TOFU_ASSERT(m_lastPtr);
		return m_lastPtr;
	}
	
// VARIABLE

	mutable instance_type*  m_lastPtr = nullptr;
};
// << SingletonAccessor

} // singleton

// 上の名前空間で使えるようにする
using singleton::SingletonAccessor;

} // tofu
