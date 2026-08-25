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
template <typename T, template <class> typename Holder = SafePtr >
class SubPtr final
{
	template <typename BaseU, template <class> typename HolderU>
    friend class SubPtr;

//**************************************************************
//              : public
//**************************************************************
public:
	
	using holder_type = Holder<T>; ///< ポインタを保持する型
	using value_type = T;
	using pointer    = T*;
	using reference  = typename detail_safe_ptr::ptr_traits<T>::reference;
	
	static constexpr bool IsConst = std::is_const_v<T>;      ///< Tがconstかどうか
	static constexpr bool IsVolatile = std::is_volatile_v<T>;      ///< Tがvolatileかどうか
	
public:
	
	SubPtr() noexcept = default;
	~SubPtr() = default;
	
	// -- move
	SubPtr( SubPtr&& rhs ) noexcept
		: m_ptr(std::move(rhs.m_ptr))
		, m_typeId(rhs.m_typeId)
	{
		rhs.reset();
	}

	SubPtr& operator=( SubPtr&& rhs ) noexcept
	{
		if (this != &rhs) {
			m_ptr = std::move(rhs.m_ptr);
			m_typeId = rhs.m_typeId;
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
	SubPtr( SubPtr<U, Holder>&& rhs ) noexcept
		: m_ptr(std::move(rhs.m_ptr))
		, m_typeId(ConvertType(rhs.m_typeId))
	{
		rhs.reset();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( SubPtr<U, Holder>&& rhs ) noexcept
	{
		m_ptr = std::move(rhs.m_ptr);
		m_typeId = ConvertType(rhs.m_typeId);
		rhs.reset();
		return *this;
	}

	// -- derived type (copy)
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( const SubPtr<U, Holder>& rhs ) noexcept
		: m_ptr(rhs.m_ptr)
		, m_typeId(ConvertType(rhs.m_typeId))
	{
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( const SubPtr<U, Holder>& rhs ) noexcept
	{
		m_ptr = rhs.m_ptr;
		m_typeId = ConvertType(rhs.m_typeId);
		return *this;
	}
	
	// -- from raw-pointer
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( U* p ) noexcept
		: m_ptr(p)
	{
		iSetTypeIdFrom<U>();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( U* p ) noexcept
	{
		m_ptr.reset(p);
		iSetTypeIdFrom<U>();
		return *this;
	}
	
	// -- from holder
	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( Holder<U> p ) noexcept
		: m_ptr(p)
	{
		iSetTypeIdFrom<U>();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( Holder<U> p ) noexcept
	{
		m_ptr = p;
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
		m_ptr.reset();
		m_typeId.Clear();
	}
	
	/// TypeId取得
	TypeId type() const noexcept { return m_typeId; }
	
	/// 基底の生ポインタ取得
	pointer get() const noexcept { return m_ptr.get(); }
	
	/// ポインタのnullアサートチェック
	void null_assert() const { TOFU_ASSERT(get()); }
	
	/// 基底の生ポインタにキャスト
	operator pointer() const noexcept { return m_ptr.get(); }

	/// boolキャスト
	explicit operator bool() const noexcept { return nullptr != get(); }
	
	/// ポインタ未設定か
	bool empty() const noexcept { return nullptr == get(); }
	
	//------------------------------------------------------------------------------
	
	/// アロー演算子
	pointer operator->() const { null_assert(); return m_ptr; }
	
	/// 参照 *
	reference operator*() const { null_assert(); return *m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// holder取得
	const holder_type& GetHolder() const { return m_ptr; }

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
	SubPtr<const T> ToConst() const noexcept
	{
		return SubPtr<const T>( *this );
	}

//**************************************************************
//              : private
//**************************************************************
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
		m_typeId = MakeTypeId<copy_cv_to_t<T, U>>();
	}
	
	// upcast or same type
	template <typename U>
	requires safe_castable_to<T, U>
	U* iTryCastImpl() const noexcept
	{
		return m_ptr.get();
	}

	// downcast
	template <typename U>
	requires (std::derived_from<U, T> && !safe_castable_to<T, U>)
	U* iTryCastImpl() const noexcept
	{
		if(!m_typeId.IsEmpty() && m_typeId.info().IsDerivedFrom<U>())
		{
			return static_cast<U*>(m_ptr.get());
		}
		return nullptr;
	}

private:
	
	holder_type m_ptr{};
	TypeId m_typeId{};
};
// << SubPtr

//------------------------------------------------------------------------------
// SubPtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <typename T, typename U, template <class> typename Holder>
constexpr bool operator ==(const SubPtr<T, Holder>& x, const SubPtr<U, Holder>& y) noexcept
	{ return x.get() == y.get(); }

/// 三方比較 <=>
template <typename T, typename U, template <class> typename Holder>
constexpr auto operator <=>(const SubPtr<T, Holder>& x, const SubPtr<U, Holder>& y) noexcept
	{ return x.get() <=> y.get(); }

/// 比較 (nullptr) ==
template <typename T, template <class> typename Holder>
constexpr bool operator ==(const SubPtr<T, Holder>& x, std::nullptr_t) noexcept
	{ return x.get() == nullptr; }

/// 三方比較 (nullptr) <=>
template <typename T, template <class> typename Holder>
constexpr auto operator <=>(const SubPtr<T, Holder>& x, std::nullptr_t) noexcept
	{ return x.get() <=> nullptr; }

//------------------------------------------------------------------------------

} // tofu