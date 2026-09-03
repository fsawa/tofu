//------------------------------------------------------------------------------
/**
 * @file    SubPtr.h
 * @brief   代入したサブクラスのポインタと型情報を保持するポインタクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>
#include <tofu_TypeInfo.h>
#include <tofu_SafePtr.h>

namespace tofu {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      代入したサブクラスのポインタと型情報を保持するポインタクラス
/// @note RTTIを使わずに代入した型へ高速にダウンキャストを行える
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, template <class> typename HolderOf = SafePtr>
class SubPtr final
{
	template <typename, template <class> typename>
    friend class SubPtr;

//------------------------------------------------------------------------------
public:
	
	using value_type = T;
	using pointer    = T*;
	using reference  = typename detail_safe_ptr::ptr_traits<T>::reference;
	
	using Holder = HolderOf<T>; ///< ポインタを保持する型

	static constexpr bool IsConst    = std::is_const_v<T>;      ///< Tがconstかどうか
	static constexpr bool IsVolatile = std::is_volatile_v<T>;   ///< Tがvolatileかどうか
	
//------------------------------------------------------------------------------
public:
	
	SubPtr() noexcept = default;
	~SubPtr() = default;
	
	// -- move
	SubPtr( SubPtr&& rhs ) noexcept
		: m_holder(std::move(rhs.m_holder))
		, m_type_id(rhs.m_type_id)
	{
		rhs.reset();
	}

	SubPtr& operator=( SubPtr&& rhs ) noexcept
	{
		if (this != &rhs) {
			m_holder = std::move(rhs.m_holder);
			m_type_id = rhs.m_type_id;
			rhs.reset();
		}
		return *this;
	}

	// -- copy
	SubPtr( const SubPtr& rhs ) noexcept = default;
	SubPtr& operator=( const SubPtr& rhs ) noexcept = default;
	
	// -- derived type (move)
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( SubPtr<U, HolderOf>&& rhs ) noexcept
		: m_holder(std::move(rhs.m_holder))
		, m_type_id(ConvertType(rhs.m_type_id))
	{
		rhs.reset();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( SubPtr<U, HolderOf>&& rhs ) noexcept
	{
		m_holder = std::move(rhs.m_holder);
		m_type_id = ConvertType(rhs.m_type_id);
		rhs.reset();
		return *this;
	}

	// -- derived type (copy)
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( const SubPtr<U, HolderOf>& rhs ) noexcept
		: m_holder(rhs.m_holder)
		, m_type_id(ConvertType(rhs.m_type_id))
	{
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( const SubPtr<U, HolderOf>& rhs ) noexcept
	{
		m_holder = rhs.m_holder;
		m_type_id = ConvertType(rhs.m_type_id);
		return *this;
	}
	
	// -- from raw-pointer
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( U* p ) noexcept
		: m_holder(p)
	{
		iSetTypeIdFrom<U>();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( U* p ) noexcept
	{
		m_holder.reset(p);
		iSetTypeIdFrom<U>();
		return *this;
	}
	
	// -- from holder
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( HolderOf<U> p ) noexcept
		: m_holder(p)
	{
		iSetTypeIdFrom<U>();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( HolderOf<U> p ) noexcept
	{
		m_holder = p;
		iSetTypeIdFrom<U>();
		return *this;
	}
	
	// -- nullptr
	SubPtr( nullptr_t ) noexcept {}

	SubPtr& operator=( nullptr_t ) noexcept
	{
		reset();
		return *this;
	}
	
	//------------------------------------------------------------------------------
	
	/// リセット
	void reset() noexcept
	{
		m_holder.reset();
		m_type_id.Clear();
	}
	
	/// TypeId取得
	TypeId type() const noexcept { return m_type_id; }
	
	/// 基底の生ポインタ取得
	pointer get() const noexcept { return m_holder.get(); }
	
	/// ポインタのnullアサートチェック
	void null_assert() const { TOFU_ASSERT(get()); }
	
	/// 基底の生ポインタにキャスト
	operator pointer() const noexcept { return m_holder.get(); }

	/// boolキャスト
	explicit operator bool() const noexcept { return nullptr != get(); }
	
	/// ポインタ未設定か
	bool empty() const noexcept { return nullptr == get(); }
	
	//------------------------------------------------------------------------------
	
	/// アロー演算子
	pointer operator->() const { null_assert(); return m_holder; }
	
	/// 参照 *
	reference operator*() const { null_assert(); return *m_holder; }
	
	//------------------------------------------------------------------------------
	
	/// holder取得
	const Holder& GetHolder() const { return m_holder; }

	/// ポインタ変換（変換出来なかったらアサート）
	template <typename Derived>
	requires std::derived_from<Derived, T>
	Derived* Cast() const
	{
		Derived* ret = TryCast<Derived>();
		TOFU_ASSERT_MSG( ret, "[SubPtr::Cast] failed Cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename Derived>
	requires std::derived_from<Derived, T>
	Derived* TryCast() const noexcept
	{
		// constは外せない
		if constexpr (IsConst && !std::is_const_v<Derived>){
			return nullptr;
		}
		// volatileは外せない
		else if constexpr (IsVolatile && !std::is_volatile_v<Derived>){
			return nullptr;
		}
		else {
			return iTryCastImpl<Derived>();
		}
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename Derived>
	requires std::derived_from<Derived, T>
	operator Derived*() const noexcept { return TryCast<Derived>(); }
	
	//------------------------------------------------------------------------------
	
	/// インスタンスの型にconst修飾を付加したSubPtrを取得
	SubPtr<const T, HolderOf> ToConst() const noexcept
	{
		return SubPtr<const T, HolderOf>( *this );
	}
	
//------------------------------------------------------------------------------
private:
	
	static constexpr void iDummy() noexcept
	{
		DefineDerivedFromAuto<T>();
	}

	/// TypeIdのCV修飾を、このクラスのTに合わせる
	static constexpr TypeId ConvertType(TypeId id) noexcept
	{
		if constexpr ( IsConst ){
			return id.GetAddConst();
		}
		else{
			return id;
		}
	}

	/// 型UからのTypeId設定
	template <typename U>
	requires safe_castable_to<U, T>
	void iSetTypeIdFrom() noexcept
	{
		DefineDerivedFrom<U, T>();
		DefineDerivedFromAuto<U>();
		m_type_id = MakeTypeId<copy_cv_to_t<T, U>>();
	}
	
	// upcast or same type
	template <typename U>
	requires safe_castable_to<T, U>
	U* iTryCastImpl() const noexcept
	{
		return m_holder.get();
	}

	// downcast
	template <typename U>
	requires (std::derived_from<U, T> && !safe_castable_to<T, U>)
	U* iTryCastImpl() const noexcept
	{
		DefineDerivedFrom<U, T>();
		if(!m_type_id.IsEmpty() && m_type_id.info().IsDerivedFrom<U>())
		{
			return static_cast<U*>(m_holder.get());
		}
		return nullptr;
	}
	
//------------------------------------------------------------------------------
private:
	
	Holder m_holder{};
	TypeId m_type_id{};
};
// << SubPtr

//------------------------------------------------------------------------------
// SubPtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <typename T, typename U, template <class> typename HolderOf>
constexpr bool operator ==(const SubPtr<T, HolderOf>& x, const SubPtr<U, HolderOf>& y) noexcept
	{ return x.get() == y.get(); }

/// 三方比較 <=>
template <typename T, typename U, template <class> typename HolderOf>
constexpr auto operator <=>(const SubPtr<T, HolderOf>& x, const SubPtr<U, HolderOf>& y) noexcept
	{ return x.get() <=> y.get(); }

/// 比較 (nullptr) ==
template <typename T, template <class> typename HolderOf>
constexpr bool operator ==(const SubPtr<T, HolderOf>& x, std::nullptr_t) noexcept
	{ return x.get() == nullptr; }

/// 三方比較 (nullptr) <=>
template <typename T, template <class> typename HolderOf>
constexpr auto operator <=>(const SubPtr<T, HolderOf>& x, std::nullptr_t) noexcept
	{ return x.get() <=> nullptr; }

//------------------------------------------------------------------------------

} // tofu