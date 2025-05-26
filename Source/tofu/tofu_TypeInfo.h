//------------------------------------------------------------------------------
/**
 * @file   tofu_TypeInfo.h
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
#include <string>
#include <string_view>
#include <map>

#include <tofu_TypeName.h>
#include <tofu_mpl_String.h>

namespace tofu {
	
// 前方宣言
template <typename T> class TypeInfoOf;

// Fromのconst修飾をToに反映する
template <class From, class To>
struct copy_const_to
{
	using type = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, std::remove_const_t<To>>;
};

template <class From, class To>
using copy_const_to_t = typename copy_const_to<From, To>::type;

// Fromのvolatile修飾をToに反映する
template <class From, class To>
struct copy_volatile_to
{
	using type = std::conditional_t<std::is_volatile_v<From>, std::add_volatile_t<To>, std::remove_volatile_t<To>>;
};

template <class From, class To>
using copy_volatile_to_t = typename copy_volatile_to<From, To>::type;

// Fromのcv修飾をToに反映する
template <class From, class To>
struct copy_cv_to
{
	using type = copy_volatile_to_t<From, copy_const_to_t<From, To>>;
};

template <class From, class To>
using copy_cv_to_t = typename copy_cv_to<From, To>::type;

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
	const char* GetNamePtr() const noexcept { return m_Name.data(); }
	
	/// 型名取得
	std::string_view GetName() const noexcept { return m_Name; }
	
	//------------------------------------------------------------------------------
	
	/// const修飾ありのTypeInfoを取得
	virtual const TypeInfo&  getAddConst() const noexcept = 0;
	
	/// const修飾なしのTypeInfoを取得
	virtual const TypeInfo&  getRemoveConst() const noexcept = 0;
	
	/// volatile修飾ありのTypeInfoを取得
	virtual const TypeInfo&  getAddVolatile() const noexcept = 0;
	
	/// volatile修飾なしのTypeInfoを取得
	virtual const TypeInfo&  getRemoveVolatile() const noexcept = 0;
	
	/// CV修飾ありのTypeInfoを取得
	virtual const TypeInfo& getAddCV() const noexcept = 0;

	/// CV修飾なしのTypeInfoを取得
	virtual const TypeInfo& getRemoveCV() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// const修飾されているか
	virtual bool  isConst() const noexcept = 0;
	
	/// volatile修飾されているか
	virtual bool  isVolatile() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// 同一判定
	bool operator==( const TypeInfo& rhs ) const noexcept  { return this == &rhs; }
	
	/// 非同一判定
	bool operator!=( const TypeInfo& rhs ) const noexcept  { return this != &rhs; }
	
	//------------------------------------------------------------------------------

	// 対象型のvoid*を、BaseType*へアップキャストを試みる
	template <typename BaseType>
	BaseType* TryUpcast(copy_cv_to_t<BaseType, void>* p) const noexcept
	{
		using base_type_no_cv = std::remove_cv<BaseType>::type;
		auto result = TryUpcast(p, TypeInfoOf<base_type_no_cv>::Instance());
		return static_cast<BaseType*>(result);
	}
	
	// 対象型のvoid*を、指定のTypeInfoの型へアップキャストする
	void* TryUpcast(void* p, const TypeInfo& target_type_info) const noexcept;
	const void* TryUpcast(const void* p, const TypeInfo& target_type_info) const noexcept;

	// 型がTと同じか
	template <typename T>
	bool IsSame() const noexcept
	{
		const TypeInfo* const target_info = &TypeInfoOf<T>::Instance();
		if(target_info == this){
			return true;
		}
		return false;
	}

	// cv修飾を除いた型がTと同じか
	template <typename T>
	bool IsSameRemoveCV() const noexcept
	{
		// cv外した型がTと同じならOK
		using target_t = std::remove_cv_t<T>;
		const TypeInfo* const target_info = &TypeInfoOf<target_t>::Instance();
		if(target_info == &getRemoveCV()){
			return true;
		}
		return false;
	}
	
	// BaseTypeから派生しているかどうか
	bool IsDerivedFrom(const TypeInfo& base_info) const noexcept;

	// BaseTypeから派生しているかどうか
	template <typename BaseType>
	bool IsDerivedFrom() const noexcept
	{
		using base_t = std::remove_cv_t<BaseType>;
		const auto& base_info = TypeInfoOf<base_t>::Instance();
		return IsDerivedFrom(base_info);
	}
	
protected:

	TypeInfo() = delete;
	explicit TypeInfo(std::string_view name) noexcept : m_Name{name} {}
	virtual ~TypeInfo() = default;
	
	void SetBaseTypeImpl(const TypeInfo& base_info, UpcastFunc func);

private:
	
	const std::string_view m_Name;
	
	// 基底クラスのTypeInfoとそのクラスへのUpcastを行う関数ポインタ
	const TypeInfo* m_BaseInfo = nullptr;
	UpcastFunc m_UpcastFunc = nullptr;

	// 基底クラスのTypeInfoとそのクラスへのUpcastを行う関数ポインタのmap
	std::map<const TypeInfo*, UpcastFunc> m_BaseInfoMap;
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
	
public:

	static constexpr auto Name = TypeName<T>::Value;
	
	/// インスタンス取得
	static self_t&  Instance() noexcept
	{
		static self_t  sInstance;
		return sInstance;
	}
	
	//------------------------------------------------------------------------------
	
	TypeInfoOf() noexcept
		: TypeInfo(Name.view())
	{
	}
	virtual ~TypeInfoOf() = default;
	
	/// const修飾ありのTypeInfoを取得
	const TypeInfo&  getAddConst() const noexcept override { return TypeInfoOf<std::add_const_t<T>>::Instance(); }
	
	/// const修飾なしのTypeInfoを取得
	const TypeInfo&  getRemoveConst() const noexcept override { return TypeInfoOf<std::remove_const_t<T>>::Instance(); }
	
	/// volatile修飾ありのTypeInfoを取得
	const TypeInfo& getAddVolatile() const noexcept { return TypeInfoOf<std::add_volatile_t<T>>::Instance(); }
	
	/// volatile修飾なしのTypeInfoを取得
	const TypeInfo& getRemoveVolatile() const noexcept { return TypeInfoOf<std::remove_volatile_t<T>>::Instance(); }
	
	/// CV修飾ありのTypeInfoを取得
	const TypeInfo& getAddCV() const noexcept override { return TypeInfoOf<std::add_cv_t<T>>::Instance(); }

	/// CV修飾なしのTypeInfoを取得
	const TypeInfo& getRemoveCV() const noexcept override { return TypeInfoOf<std::remove_cv_t<T>>::Instance(); }

	/// const修飾されているか
	bool  isConst() const noexcept override { return std::is_const_v<T>; }
	
	/// volatile修飾されているか
	bool  isVolatile() const noexcept override { return std::is_volatile_v<T>; }
	
	//------------------------------------------------------------------------------
	
	// 基底クラス設定
	template <typename BaseType>
		requires std::derived_from<T, BaseType>
	void  SetBaseType()
	{
		// TもBaseTypeもCV修飾がついていたらNG
		static_assert(std::is_const_v<T> == false);
		static_assert(std::is_volatile_v<T> == false);
		static_assert(std::is_const_v<BaseType> == false);
		static_assert(std::is_volatile_v<BaseType> == false);

		const TypeInfo& info = TypeInfoOf<BaseType>::Instance();

		// アップキャスト関数
		auto func = [](void* p, const TypeInfo& target_type_info)
		{
			// pはTのポインタである前提で、BaseTypeにアップキャストする
			BaseType* base = static_cast<BaseType*>( static_cast<T*>(p) );
			// targetがBaseTypeだったらポインタを返す
			if(target_type_info == TypeInfoOf<BaseType>::Instance())
			{
				return static_cast<void*>(base);
			}
			// 違ったら、BaseTypeの基底クラスへのアップキャストを試みる
			return TypeInfoOf<BaseType>::Instance().TryUpcast(base, target_type_info);
		};

		SetBaseTypeImpl(info, func);
	}
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

/// 基底クラス情報の設定マクロ
#define TOFU_SET_BASE_TYPE(Base, Derived)  TOFU_STATIC_CALL(::tofu::SetBaseType<Base,Derived>)

/// 基底クラス情報の設定
template <typename Base, typename Derived>
inline void SetBaseType()
{
	GetTypeInfo<std::remove_cv_t<Derived>>().template SetBaseType<std::remove_cv_t<Base>>();
}

} // tofu
