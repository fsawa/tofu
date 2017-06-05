//------------------------------------------------------------------------------
/**
 * @file   TypeInfo.hpp
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2016 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>

#include <type_traits>

namespace tofu {

////////////////////////////////////////////////////////////////////////////////
/// @brief      型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeInfo
{
	typedef TypeInfo  self_type;
	
public:
	
	/// 型名取得
	const char* getName() const noexcept { return m_name; }
	
	/// 型名設定
	void        setNamePtr( const char* name ) noexcept { m_name = name; }
	
	//------------------------------------------------------------------------------
	
	/// const修飾ありのTypeInfoを取得
	const TypeInfo&  getAddConst() const noexcept  { return *m_addConst; }
	
	/// const修飾なしのTypeInfoを取得
	const TypeInfo&  getRemoveConst() const noexcept  { return *m_removeConst; }
	
	/// volatile修飾ありのTypeInfoを取得
	const TypeInfo&  getAddVolatile() const noexcept  { return *m_addVolatile; }
	
	/// volatile修飾なしのTypeInfoを取得
	const TypeInfo&  getRemoveVolatile() const noexcept  { return *m_removeVolatile; }
	
	//------------------------------------------------------------------------------
	
	/// constかどうか
	bool  isConst() const noexcept  { return m_isConst; }
	
	/// volatileかどうか
	bool  isVolatile() const noexcept  { return m_isVolatile; }
	
protected:

	TypeInfo() {}
	//virtual ~TypeInfo();
	
	static const char* ParseTypeName( char* str ) noexcept;
	
protected:
	
	const char*  m_name = "";
	
	TypeInfo*  m_addConst = nullptr;
	TypeInfo*  m_removeConst = nullptr;
	TypeInfo*  m_addVolatile = nullptr;
	TypeInfo*  m_removeVolatile = nullptr;
	
	bool m_isConst = false;
	bool m_isVolatile = false;
};
// << TypeInfo


////////////////////////////////////////////////////////////////////////////////
/// @brief      型毎の型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TTypeInfo : public TypeInfo
{
	typedef TTypeInfo  self_type;
	
public:
	
	typedef T  type;
	
	//------------------------------------------------------------------------------
	
	/// インスタンス取得
	static self_type&  Instance() noexcept
	{
		static self_type  sInstance;
		return sInstance;
	}
	
	//------------------------------------------------------------------------------
	
	TTypeInfo()
	{
		static char sNameBuffer[] = TOFU_FUNCTION_NAME;
		m_name = ParseTypeName( sNameBuffer );
		
		m_addConst = this;
		if( !std::is_const<T>::value ){
			m_addConst = &TTypeInfo< typename std::add_const<T>::type >::Instance();
		}
		
		m_removeConst = this;
		if(std::is_const<T>::value ){
			m_removeConst = &TTypeInfo< typename std::remove_const<T>::type >::Instance();
		}
		
		m_addVolatile = this;
		if( !std::is_volatile<T>::value ){
			m_addVolatile = &TTypeInfo< typename std::add_volatile<T>::type >::Instance();
		}
		
		m_removeVolatile = this;
		if(std::is_volatile<T>::value ){
			m_removeVolatile = &TTypeInfo< typename std::remove_volatile<T>::type >::Instance();
		}
		
		m_isConst = std::is_const<T>::value;
		m_isVolatile = std::is_volatile<T>::value;
	}
};
// << TTypeInfo

//------------------------------------------------------------------------------

/// TypeInfo取得
template <typename T>
inline TypeInfo&  GetTypeInfo() noexcept
{
	return TTypeInfo<T>::Instance();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief      型ID
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeId
{
public:
	
	TypeId() {}
	explicit TypeId( const TypeInfo* info ) : m_pTypeInfo(info) {}
	explicit TypeId( const TypeInfo& info ) : m_pTypeInfo(&info) {}
	
	/// 指定した型のIDを設定
	template <typename T>
	void  assign() noexcept  { m_pTypeInfo = &GetTypeInfo<T>(); }
	
	/// 空かどうか
	bool  empty() const noexcept  { return nullptr == m_pTypeInfo; }
	
	/// クリア
	void  clear() noexcept
	{
		m_pTypeInfo = nullptr;
	}
	
	/// TypeInfo取得
	const TypeInfo&  info() const noexcept  { TOFU_ASSERT(m_pTypeInfo); return *m_pTypeInfo; }
	
	/// const修飾ありのTypeId取得
	TypeId  makeAddConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getAddConst() ) : TypeId(); }
	
	/// const修飾なしのTypeId取得
	TypeId  makeRemoveConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getRemoveConst() ) : TypeId(); }
	
	/// volatile修飾ありのTypeId取得
	TypeId  makeAddVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getAddVolatile() ) : TypeId(); }
	
	/// volatile修飾なしのTypeId取得
	TypeId  makeRemoveVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getRemoveVolatile() ) : TypeId(); }
	
	/// 同一判定
	bool  operator==( const TypeId& rhs ) const noexcept  { return m_pTypeInfo == rhs.m_pTypeInfo; }
	
	/// 非同一判定
	bool  operator!=( const TypeId& rhs ) const noexcept  { return m_pTypeInfo != rhs.m_pTypeInfo; }
	
private:
	const TypeInfo* m_pTypeInfo = nullptr;
};

//------------------------------------------------------------------------------

/// TypeId作成
template <typename T>
inline TypeId  MakeTypeId() noexcept
{
	return TypeId( &GetTypeInfo<T>() );
};

/// 変数からTypeId作成
template <typename T>
inline TypeId  MakeTypeId( T& ) noexcept
{
	return TypeId( &GetTypeInfo<T>() );
};

} // tofu
