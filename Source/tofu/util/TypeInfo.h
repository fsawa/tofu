//------------------------------------------------------------------------------
/**
 * @file   TypeInfo.h
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <type_traits>

namespace tofu {
	
// 前方宣言
template <typename T> class TypeInfoOf;

////////////////////////////////////////////////////////////////////////////////
/// @brief      型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeInfo
{
	using self_t = TypeInfo;
	
public:

	using UpcastFunc = void* (*)(void*, const TypeInfo&);

public:
	
	/// 型名取得
	const char* getName() const noexcept { return m_name; }
	
	/// 型名設定
	void        setNamePtr( const char* name ) noexcept { m_name = name; }
	
	//------------------------------------------------------------------------------
	
	/// const修飾ありのTypeInfoを取得
	virtual const TypeInfo&  getAddConst() const noexcept = 0;
	
	/// const修飾なしのTypeInfoを取得
	virtual const TypeInfo&  getRemoveConst() const noexcept = 0;
	
	/// volatile修飾ありのTypeInfoを取得
	//const TypeInfo&  getAddVolatile() const noexcept  { return *m_addVolatile; }
	
	/// volatile修飾なしのTypeInfoを取得
	//const TypeInfo&  getRemoveVolatile() const noexcept  { return *m_removeVolatile; }
	
	/// CV修飾なしのTypeInfoを取得
	virtual const TypeInfo& getRemoveCV() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// const修飾されているか
	virtual bool  isConst() const noexcept = 0;
	
	/// volatile修飾されているか
	virtual bool  isVolatile() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// 同一判定
	bool  operator==( const TypeInfo& rhs ) const noexcept  { return this == &rhs; }
	
	/// 非同一判定
	bool  operator!=( const TypeInfo& rhs ) const noexcept  { return this != &rhs; }
	
	//------------------------------------------------------------------------------

	// 指定したTypeInfoが基底クラスか
	bool  isBaseType(const TypeInfo& info) const noexcept
	{
		// CVがついてたらNG
		if(isConst() || isVolatile())
		{
			return getRemoveCV().isBaseType(info);
		}

		if(m_baseInfo)
		{
			// thisはCVなしなので、baseInfoにもCVがついていない想定
			if(m_baseInfo == &info.getRemoveCV())
			{
				return true;
			}
			// 再帰チェック
			return m_baseInfo->isBaseType(info);
		}
		return false;
	}

	// アップキャスト
	template <typename BaseType>
	BaseType*  upcast(const void* p) const noexcept
	{
		using base_type_no_cv = std::remove_cv<BaseType>::type;

		// cvなしのTypeInfoで処理する
		if(isConst() || isVolatile())
		{
			return getRemoveCV().upcast<BaseType>(p);
		}
		void* result = upcast(const_cast<void*>(p), TypeInfoOf<base_type_no_cv>::Instance());
		return static_cast<BaseType*>(result);
	}
	
	// 指定のTypeInfoの型へアップキャスト
	void*  upcast(void* p, const TypeInfo& target_type_info) const noexcept
	{
		if(m_upcastFunc){
			return m_upcastFunc(p, target_type_info);
		}
		return nullptr;
	}
	
protected:

	TypeInfo() noexcept;
	virtual ~TypeInfo() = default;
	
	static const char* ParseTypeName( char* str, const char* src, int len ) noexcept;
	
protected:
	
	const char*  m_name = "";
	
	const TypeInfo*  m_baseInfo = nullptr;
	UpcastFunc m_upcastFunc = nullptr;
};
// << TypeInfo

////////////////////////////////////////////////////////////////////////////////
/// @brief      型毎の型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TypeInfoOf : public TypeInfo
{
	using self_t = TypeInfoOf;
	
public:
	
	using type = T;
	
	//------------------------------------------------------------------------------
	
	/// インスタンス取得
	static self_t&  Instance() noexcept
	{
		static self_t  sInstance;
		return sInstance;
	}
	
	//------------------------------------------------------------------------------
	
	TypeInfoOf() noexcept
	{
		_initTypeName(TOFU_FUNCTION_NAME);
	}
	virtual ~TypeInfoOf() = default;
	
	/// const修飾ありのTypeInfoを取得
	const TypeInfo&  getAddConst() const noexcept override { return TypeInfoOf<std::add_const_t<T>>::Instance(); }
	
	/// const修飾なしのTypeInfoを取得
	const TypeInfo&  getRemoveConst() const noexcept override { return TypeInfoOf<std::remove_const_t<T>>::Instance(); }
	
	/// CV修飾なしのTypeInfoを取得
	const TypeInfo& getRemoveCV() const noexcept override { return TypeInfoOf<std::remove_cv_t<T>>::Instance(); }

	/// const修飾されているか
	bool  isConst() const noexcept override { return std::is_const_v<T>; }
	
	/// volatile修飾されているか
	bool  isVolatile() const noexcept override { return std::is_volatile_v<T>; }
	
	//------------------------------------------------------------------------------
	// 
	// 基底クラス設定
	template <typename BaseType>
	void  setBaseType()
	{
		// BaseTypeはTの基底クラスでなければならない
		static_assert(std::is_base_of<BaseType, T>::value == true);
		// TもBaseTypeもCV修飾がついていたらNG
		static_assert(std::is_const<T>::value == false);
		static_assert(std::is_volatile<T>::value == false);
		static_assert(std::is_const<BaseType>::value == false);
		static_assert(std::is_volatile<BaseType>::value == false);

		// 既に設定済みはNG
		TOFU_ASSERT(m_baseInfo == nullptr);
		m_baseInfo = &TypeInfoOf<BaseType>::Instance();

		// アップキャスト関数を設定
		m_upcastFunc = [](void* p, const TypeInfo& target_type_info)
		{
			// pはTのポインタである前提で、BaseTypeにアップキャストする
			BaseType* base = static_cast<BaseType*>( static_cast<T*>(p) );
			// targetがBaseTypeだったらポインタを返す
			if(target_type_info == TypeInfoOf<BaseType>::Instance())
			{
				return static_cast<void*>(base);
			}
			// 違ったら、BaseTypeの基底クラスへのアップキャストを試みる
			return TypeInfoOf<BaseType>::Instance().upcast(base, target_type_info);
		};
	}

private:
	
	template <int N>
	void  _initTypeName(const char (&src)[N] ) noexcept
	{
		static char sNameBuffer[N]{ 0 };
		m_name = ParseTypeName(sNameBuffer, src, N);
	}

private:
};
// << TypeInfoOf

//------------------------------------------------------------------------------

/// TypeInfo取得
template <typename T>
inline TypeInfoOf<T>&  GetTypeInfo() noexcept
{
	return TypeInfoOf<T>::Instance();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief      型ID
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeId
{
public:
	
	TypeId() noexcept {}
	explicit TypeId( const TypeInfo* info ) noexcept : m_pTypeInfo(info) {}
	explicit TypeId( const TypeInfo& info ) noexcept : m_pTypeInfo(&info) {}
	
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
	const TypeInfo&  info() const  { TOFU_ASSERT(m_pTypeInfo); return *m_pTypeInfo; }
	
	/// const修飾ありのTypeId取得
	TypeId  makeAddConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getAddConst() ) : TypeId(); }
	
	/// const修飾なしのTypeId取得
	TypeId  makeRemoveConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getRemoveConst() ) : TypeId(); }
	
	/// volatile修飾ありのTypeId取得
	//TypeId  makeAddVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getAddVolatile() ) : TypeId(); }
	
	/// volatile修飾なしのTypeId取得
	//TypeId  makeRemoveVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->getRemoveVolatile() ) : TypeId(); }
	
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
	
//------------------------------------------------------------------------------

template <typename Base, typename Derived>
class BaseTypeSetter
{
public:
	BaseTypeSetter()
	{
		GetTypeInfo<Derived>().setBaseType<Base>();
	}
};

} // tofu
