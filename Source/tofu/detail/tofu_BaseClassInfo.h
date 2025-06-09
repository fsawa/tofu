//------------------------------------------------------------------------------
/**
 * @file   tofu_BaseClassInfo.h
 * @brief  基底クラス情報
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <type_traits>

#include <tofu_TypeTraits.h>

namespace tofu {
	
// 前方宣言
class TypeInfo;
template <typename T> class TypeInfoOf;

namespace detail {

// 基底クラス情報
class BaseClassInfo
{
public:
	const TypeInfo& m_BaseTypeInfo;
	const BaseClassInfo* const m_Next = nullptr;

public:
	BaseClassInfo() = delete;
	BaseClassInfo(TypeInfo& derived, const TypeInfo& base);
		
	const TypeInfo& GetBaseTypeInfo() const noexcept
	{
		return m_BaseTypeInfo;
	}

	// アップキャスト
	virtual void* TryCastTo(void* p, const TypeInfo& target_type_info) const noexcept = 0;
		
	const void* TryCastTo(const void* p, const TypeInfo& target_type_info) const noexcept
	{
		return TryCastTo(const_cast<void*>(p), target_type_info);
	}

	// ダウンキャスト
	virtual void* TryCastFrom(void* p, const TypeInfo& target_type_info) const noexcept = 0;
		
	const void* TryCastFrom(const void* p, const TypeInfo& target_type_info) const noexcept
	{
		return TryCastFrom(const_cast<void*>(p), target_type_info);
	}
};

// 
template <class DerivedT, class BaseT>
requires (std::derived_from<DerivedT, BaseT> && std::negation_v<std::is_same<DerivedT, BaseT>>)
class BaseClassInfoOf final : private BaseClassInfo
{
	using self_type = BaseClassInfoOf;

public:
	// 静的初期化時に登録させるためのインスタンス
	static BaseClassInfoOf s_Instance;

	// インスタンスを実体化させるため参照
	static constexpr const BaseClassInfoOf& Fetch() noexcept{
		return s_Instance;
	}

public:
	BaseClassInfoOf() : BaseClassInfo(
		TypeInfoOf<DerivedT>::Instance(),
		TypeInfoOf<BaseT>::Instance())
	{}

	// アップキャスト
	void* TryCastTo(void* p, const TypeInfo& target_type_info) const noexcept override
	{
		// pはTのポインタである前提で、BaseTypeにアップキャストする
		BaseT* base = static_cast<BaseT*>( static_cast<DerivedT*>(p) );
		// targetがBaseTypeだったらポインタを返す
		if(target_type_info == TypeInfoOf<BaseT>::Instance())
		{
			return static_cast<void*>(base);
		}
		// 違ったら、BaseTypeの基底クラスへのアップキャストを試みる
		return GetBaseTypeInfo().TryUpcast(base, target_type_info);
	}

	// ダウンキャスト
	void* TryCastFrom(void* p, const TypeInfo& target_type_info) const noexcept override
	{
		return nullptr;
	}
};

// インスタンス
template <class DerivedT, class BaseT>
requires (std::derived_from<DerivedT, BaseT> && std::negation_v<std::is_same<DerivedT, BaseT>>)
BaseClassInfoOf<DerivedT, BaseT> BaseClassInfoOf<DerivedT, BaseT>::s_Instance{};
	
// 継承関係を定義させる
template <class DerivedT, class BaseT>
constexpr void DefineDerivedFrom() noexcept
{
	if constexpr (!std::is_same_v<DerivedT, BaseT>)
	{
		BaseClassInfoOf<DerivedT, BaseT>::Fetch();
	}
}

//------------------------------------------------------------------------------

} // detail
} // tofu
