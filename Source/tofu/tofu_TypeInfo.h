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

#include <tofu_TypeTraits.h>
#include <tofu_TypeName.h>
#include <detail/tofu_BaseClassInfo.h>

namespace tofu {
	
// 前方宣言
class TypeInfo;
template <typename T> class TypeInfoOf;

////////////////////////////////////////////////////////////////////////////////
/// @brief      型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeInfo
{
	using self_type = TypeInfo;

	friend class rtti_detail::BaseClassInfo;
	using BaseClassInfo = rtti_detail::BaseClassInfo;

public:
	
	/// 型名取得
	const char* GetNamePtr() const noexcept { return m_Name.data(); }
	
	/// 型名取得
	std::string_view GetName() const noexcept { return m_Name; }
	
	//------------------------------------------------------------------------------
	
	/// const修飾ありのTypeInfoを取得
	virtual const TypeInfo&  GetAddConst() const noexcept = 0;
	
	/// const修飾なしのTypeInfoを取得
	virtual const TypeInfo&  GetRemoveConst() const noexcept = 0;
	
	/// volatile修飾ありのTypeInfoを取得
	virtual const TypeInfo&  GetAddVolatile() const noexcept = 0;
	
	/// volatile修飾なしのTypeInfoを取得
	virtual const TypeInfo&  GetRemoveVolatile() const noexcept = 0;
	
	/// CV修飾ありのTypeInfoを取得
	virtual const TypeInfo& GetAddCV() const noexcept = 0;

	/// CV修飾なしのTypeInfoを取得
	virtual const TypeInfo& GetRemoveCV() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// const修飾されているか
	virtual bool  IsConst() const noexcept = 0;
	
	/// volatile修飾されているか
	virtual bool  IsVolatile() const noexcept = 0;
	
	//------------------------------------------------------------------------------
	
	/// 比較 ==
	bool operator==( const TypeInfo& rhs ) const noexcept
		{ return this == &rhs; }
	
	/// 三方比較 <=>
	constexpr auto operator <=>(const TypeInfo& rhs) const noexcept
		{ return this <=> &rhs; }
	
	//------------------------------------------------------------------------------

	// 対象型のvoid*を、BaseType*へアップキャストを試みる
	template <typename BaseT>
	BaseT* TryUpcast(copy_cv_to_t<BaseT, void>* p) const noexcept
	{
		using base_type_no_cv = std::remove_cv<BaseT>::type;
		auto result = TryUpcast(p, TypeInfoOf<base_type_no_cv>::Instance());
		return static_cast<BaseT*>(result);
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
		if(target_info == &GetRemoveCV()){
			return true;
		}
		return false;
	}
	
	// base_infoから派生しているかどうか
	bool IsDerivedFrom(const TypeInfo& base_info) const noexcept;

	// BaseTから派生しているかどうか
	template <typename BaseT>
	bool IsDerivedFrom() const noexcept
	{
		using base_t = std::remove_cv_t<BaseT>;
		const auto& base_info = TypeInfoOf<base_t>::Instance();
		return IsDerivedFrom(base_info);
	}
	
protected:

	TypeInfo() = delete;
	explicit TypeInfo(std::string_view name) noexcept : m_Name{name} {}
	virtual ~TypeInfo() = default;

private:
	const BaseClassInfo* AddBaseInfo(const BaseClassInfo& info);
	
private:
	const std::string_view m_Name;
	
	const BaseClassInfo* m_BaseClassInfo = nullptr; // 基底クラス情報
};
// << TypeInfo

////////////////////////////////////////////////////////////////////////////////
/// @brief      型毎の型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TypeInfoOf : public TypeInfo
{
	using self_type = TypeInfoOf;
	
public:
	
	using type = T;
	
public:

	static constexpr auto Name = TypeName<T>::Value;
	
	/// インスタンス取得
	static self_type&  Instance() noexcept
	{
		static self_type sInstance;
		return sInstance;
	}
	
	//------------------------------------------------------------------------------
	
	TypeInfoOf() noexcept
		: TypeInfo(Name.view())
	{
	}
	virtual ~TypeInfoOf() = default;
	
	/// const修飾ありのTypeInfoを取得
	const TypeInfo& GetAddConst() const noexcept override { return TypeInfoOf<std::add_const_t<T>>::Instance(); }
	
	/// const修飾なしのTypeInfoを取得
	const TypeInfo& GetRemoveConst() const noexcept override { return TypeInfoOf<std::remove_const_t<T>>::Instance(); }
	
	/// volatile修飾ありのTypeInfoを取得
	const TypeInfo& GetAddVolatile() const noexcept { return TypeInfoOf<std::add_volatile_t<T>>::Instance(); }
	
	/// volatile修飾なしのTypeInfoを取得
	const TypeInfo& GetRemoveVolatile() const noexcept { return TypeInfoOf<std::remove_volatile_t<T>>::Instance(); }
	
	/// CV修飾ありのTypeInfoを取得
	const TypeInfo& GetAddCV() const noexcept override { return TypeInfoOf<std::add_cv_t<T>>::Instance(); }

	/// CV修飾なしのTypeInfoを取得
	const TypeInfo& GetRemoveCV() const noexcept override { return TypeInfoOf<std::remove_cv_t<T>>::Instance(); }

	/// const修飾されているか
	bool IsConst() const noexcept override { return std::is_const_v<T>; }
	
	/// volatile修飾されているか
	bool IsVolatile() const noexcept override { return std::is_volatile_v<T>; }
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
	
	constexpr TypeId() noexcept {}
	explicit constexpr TypeId( const TypeInfo* info ) noexcept : m_pTypeInfo(info) {}
	explicit constexpr TypeId( const TypeInfo& info ) noexcept : m_pTypeInfo(&info) {}
	
	/// 指定した型のIDを設定
	template <typename T>
	constexpr void  assign() noexcept  { m_pTypeInfo = &GetTypeInfo<T>(); }
	
	/// 空かどうか
	constexpr bool  empty() const noexcept  { return nullptr == m_pTypeInfo; }
	
	/// クリア
	constexpr void  clear() noexcept
	{
		m_pTypeInfo = nullptr;
	}
	
	/// TypeInfo取得
	constexpr const TypeInfo& info() const  { TOFU_ASSERT(m_pTypeInfo); return *m_pTypeInfo; }
	
	/// TypeInfo取得
	constexpr const TypeInfo* GetInfoPtr() const noexcept  { return m_pTypeInfo; }

	/// const修飾ありのTypeId取得
	constexpr TypeId  GetAddConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->GetAddConst() ) : TypeId(); }
	
	/// const修飾なしのTypeId取得
	constexpr TypeId  GetRemoveConst() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->GetRemoveConst() ) : TypeId(); }
	
	/// volatile修飾ありのTypeId取得
	//TypeId  makeAddVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->GetAddVolatile() ) : TypeId(); }
	
	/// volatile修飾なしのTypeId取得
	//TypeId  makeRemoveVolatile() const noexcept  { return m_pTypeInfo ? TypeId( m_pTypeInfo->GetRemoveVolatile() ) : TypeId(); }
	
	/// 比較 ==
	constexpr bool operator ==(const TypeId& rhs) const noexcept
		{ return GetInfoPtr() == rhs.GetInfoPtr(); }

	/// 三方比較 <=>
	constexpr auto operator <=>(const TypeId& rhs) const noexcept
		{ return GetInfoPtr() <=> rhs.GetInfoPtr(); }

private:
	const TypeInfo* m_pTypeInfo = nullptr;
};

//------------------------------------------------------------------------------

/// TypeId作成
template <typename T>
constexpr TypeId MakeTypeId() noexcept
{
	return TypeId( &GetTypeInfo<T>() );
};

/// 変数からTypeId作成
template <typename T>
constexpr TypeId MakeTypeId( T& ) noexcept
{
	return TypeId( &GetTypeInfo<T>() );
};
	
//------------------------------------------------------------------------------

// 継承関係を定義させる
template <class DerivedT, class BaseT>
requires std::derived_from<DerivedT, BaseT>
constexpr void DefineDerivedFrom() noexcept
{
	rtti_detail::DefineDerivedFrom<std::remove_cv_t<DerivedT>, std::remove_cv_t<BaseT>>();
}

/// 独自RTTIの継承関係を定義するマクロ
#define TOFU_RTTI_DERIVED_FROM(Derived, Base)  TOFU_STATIC_CALL(::tofu::DefineDerivedFrom<Derived, Base>)

// 継承関係を定義させる
template <class DerivedT>
constexpr void DefineDerivedFromAuto() noexcept
{
	rtti_detail::DefineDerivedFromAuto<DerivedT>();
}

} // tofu
