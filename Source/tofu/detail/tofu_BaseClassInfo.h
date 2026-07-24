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

namespace rtti_detail {

//------------------------------------------------------------------------------
// 基底クラス情報クラス
class BaseClassInfo
{
public:
	BaseClassInfo() = delete;
	BaseClassInfo(TypeInfo& derived, const TypeInfo& base);
		
	const TypeInfo& GetBaseTypeInfo() const noexcept
	{
		return m_BaseTypeInfo;
	}
	
	const BaseClassInfo* GetNext() const noexcept { return m_Next; }

	// アップキャスト
	virtual void* TryCastTo(void* p, const TypeInfo& target_type_info) const noexcept = 0;
		
	const void* TryCastTo(const void* p, const TypeInfo& target_type_info) const noexcept
	{
		return TryCastTo(const_cast<void*>(p), target_type_info);
	}

private:
	const TypeInfo& m_BaseTypeInfo;
	const BaseClassInfo* const m_Next = nullptr;

};

//------------------------------------------------------------------------------
// 基底クラス情報クラス
template <class DerivedT, class BaseT>
requires (std::derived_from<DerivedT, BaseT> && std::negation_v<std::is_same<DerivedT, BaseT>>)
class BaseClassInfoOf final : private BaseClassInfo
{
	// 静的初期化時に登録させるためのインスタンス
	static const BaseClassInfoOf s_Instance;
	
	static constexpr const BaseClassInfoOf& FetchImpl() noexcept {
		return s_Instance;
	}
	
public:
	// インスタンスを実体化させるため参照
	static constexpr void Fetch() noexcept {
		(void)FetchImpl();
	}

public:
	BaseClassInfoOf() : BaseClassInfo(
		TypeInfoOf<DerivedT>::Instance(),
		TypeInfoOf<BaseT>::Instance())
	{}

	// アップキャスト
	void* TryCastTo(void* p, const TypeInfo& target_type_info) const noexcept override
	{
		// pはDerivedTのポインタである前提で、BaseTypeにアップキャストする
		BaseT* base = static_cast<BaseT*>( static_cast<DerivedT*>(p) );
		// targetがBaseTだったらポインタを返す
		if(target_type_info == TypeInfoOf<BaseT>::Instance())
		{
			return static_cast<void*>(base);
		}
		// 違ったら、BaseTの基底クラスへのアップキャストを試みる
		return GetBaseTypeInfo().TryUpcast(base, target_type_info);
	}
};

//------------------------------------------------------------------------------

// インスタンス
template <class DerivedT, class BaseT>
requires (std::derived_from<DerivedT, BaseT> && std::negation_v<std::is_same<DerivedT, BaseT>>)
const BaseClassInfoOf<DerivedT, BaseT> BaseClassInfoOf<DerivedT, BaseT>::s_Instance{};

//------------------------------------------------------------------------------
// 継承関係を定義させる
template <class DerivedT, class BaseT>
requires std::derived_from<DerivedT, BaseT>
constexpr void DefineDerivedFrom() noexcept
{
	if constexpr (!std::is_same_v<DerivedT, BaseT>)
	{
		BaseClassInfoOf<DerivedT, BaseT>::Fetch();
	}
}

//------------------------------------------------------------------------------
namespace detail
{
	// 基底クラス検出
	template<class, template<class> class, class = std::void_t<>>
	struct BaseDetect
	{
		static constexpr bool value = false;
		using type = void;
	};

	// 基底クラスかどうかの条件付き
	template<class T, template<class> class Check>
	requires std::derived_from<T, Check<T>> && (!std::same_as<T, Check<T>>)
	struct BaseDetect<T, Check, std::void_t<Check<T>>>
	{
		static constexpr bool value = true;
		using type = Check<T>;
	};

	// 基底クラスの型を定義しているかどうかの検出
	template<class T> using check_base = typename T::base;
	template<class T> using check_Base = typename T::Base;
	template<class T> using check_BaseType = typename T::BaseType;
	template<class T> using check_base_type = typename T::base_type;

	template<class T>
	struct BaseTypeDetect
	{
	private:
		template<template<class> class Check, class Prev = std::false_type>
		struct MyDetect
		{
			using detect_t = BaseDetect<T, Check>;

			// 最終検出 Prevで見つかっていたらPrevを採用する
			static constexpr bool value = Prev::value ? true : detect_t::value;
			using type = std::conditional_t<Prev::value, typename Prev::type, typename detect_t::type>;

			template<template<class> class Check2>
			using Next = MyDetect<Check2, MyDetect<Check, Prev>>;
		};

		// いくつかの定義名候補を調べる
		using FinalDetect = 
			MyDetect<check_base,
			MyDetect<check_Base,
			MyDetect<check_BaseType,
			MyDetect<check_base_type
			>>>>;
	
	public:
		// 基底クラスが検出できたらtrue
		static constexpr bool value = FinalDetect::value;

		// 検出できた基底クラス
		using type = typename FinalDetect::type;
	};
} // detail


// 基底クラスの定義が見つかれば、自動で継承関係を定義させる
template <class DerivedT>
constexpr void DefineDerivedFromAuto() noexcept
{
	using derived_type = std::remove_cv_t<DerivedT>;
	if constexpr (detail::BaseTypeDetect<derived_type>::value)
	{
		using base_type = typename detail::BaseTypeDetect<derived_type>::type;

		DefineDerivedFrom<derived_type, base_type>();
		
		//std::cout << GetTypeName<DerivedT>() << std::endl;
		//std::cout << "base=" << GetTypeName<base_type>() << std::endl;

		// 再起チェック
		DefineDerivedFromAuto<base_type>();
	}
}

//------------------------------------------------------------------------------

} // rtti_detail
} // tofu
