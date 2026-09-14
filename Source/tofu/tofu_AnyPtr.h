//------------------------------------------------------------------------------
/**
 * @file    AnyPtr.h
 * @brief   任意のポインタを型情報と共に保持し、参照時に型チェックをして安全に取り出すクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>
#include <tofu_TypeInfo.h>
#include <tofu_BaseClassInfo.h>
#include <tofu_SafePtr.h>

namespace tofu {
	
////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief     任意のポインタを型情報と共に保持し、参照時に型チェックをして安全に取り出すクラス
/// @note 格納されている型からキャストできない型として取り出そうとしたらnullを返す
////////////////////////////////////////////////////////////////////////////////////////////////
template <template <class> typename HolderOf = SafePtr>
class AnyPtr final
{
//------------------------------------------------------------------------------
public:
	
	using value_type = void;
	using pointer    = void*;

	using Holder = HolderOf<void>; ///< ポインタを保持する型
	
//------------------------------------------------------------------------------
public:
	
	AnyPtr() noexcept = default;
	~AnyPtr() = default;
	
	// -- move
	AnyPtr( AnyPtr&& rhs ) noexcept
		: m_holder(std::move(rhs.m_holder))
		, m_type_id(rhs.m_type_id)
	{
		rhs.reset();
	}

	AnyPtr& operator=( AnyPtr&& rhs ) noexcept
	{
		if (this != &rhs) {
			m_holder = std::move(rhs.m_holder);
			m_type_id = rhs.type();
			rhs.reset();
		}
		return *this;
	}

	// -- copy
	AnyPtr( const AnyPtr& rhs ) noexcept = default;
	AnyPtr& operator=( const AnyPtr& rhs ) noexcept = default;
	
	// -- from raw-pointer
	template <typename U>
	AnyPtr( U* p ) noexcept
		: m_holder(const_cast<std::remove_const_t<U>*>(p))
	{
		iSetTypeIdFrom<U>();
	}
	
	template <typename U>
	AnyPtr& operator=( U* p ) noexcept
	{
		m_holder.reset(const_cast<std::remove_const_t<U>*>(p));
		iSetTypeIdFrom<U>();
		return *this;
	}
	
	// -- from holder
	template <typename U>
	AnyPtr( HolderOf<U> p ) noexcept
		: m_holder(std::move(p))
	{
		iSetTypeIdFrom<U>();
	}

	template <typename U>
	AnyPtr& operator=( HolderOf<U> p ) noexcept
	{
		m_holder = std::move(p);
		iSetTypeIdFrom<U>();
		return *this;
	}

	// -- nullptr
	AnyPtr( nullptr_t ) noexcept {}

	AnyPtr& operator=( nullptr_t ) noexcept
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
	
	/// 生ポインタ取得
	pointer get() const noexcept { return m_holder.get(); }
	
	/// ポインタのnullアサートチェック
	void null_assert() const { TOFU_ASSERT(m_holder); }
	
	/// void生ポインタにキャスト
	operator pointer() const noexcept { return m_holder.get(); }

	/// boolキャスト
	explicit operator bool() const noexcept { return nullptr != get(); }
	
	/// ポインタ未設定か
	bool empty() const noexcept { return nullptr == get(); }
	
	//------------------------------------------------------------------------------
	
	/// 比較 ==
	friend constexpr bool operator ==(const AnyPtr<HolderOf>& x, const AnyPtr<HolderOf>& y) noexcept
		{ return x.m_holder == y.m_holder; }

	/// 三方比較 <=>
	friend constexpr auto operator <=>(const AnyPtr<HolderOf>& x, const AnyPtr<HolderOf>& y) noexcept
		{ return x.m_holder <=> y.m_holder; }

	/// 比較 (nullptr) ==
	friend constexpr bool operator ==(const AnyPtr<HolderOf>& x, std::nullptr_t) noexcept
		{ return x.m_holder == nullptr; }

	/// 三方比較 (nullptr) <=>
	friend constexpr bool operator <=>(const AnyPtr<HolderOf>& x, std::nullptr_t) noexcept
		{ return x.m_holder <=> nullptr; }

	//------------------------------------------------------------------------------

	/// holder取得
	const Holder& GetHolder() const { return m_holder; }

	/// ポインタ変換（変換出来なかったらアサート）
	template <typename Derived>
	Derived* Cast() const
	{
		Derived* ret = TryCast<Derived>();
		TOFU_ASSERT_MSG( ret, "[AnyPtr::Cast] failed Cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename Derived>
	Derived* TryCast() const noexcept
	{
		// 型が未設定の場合
		if(m_type_id.IsEmpty()) return nullptr;
		// constは外せない
		if constexpr (!std::is_const_v<Derived>)
		{
			if(m_type_id.IsConst()) return nullptr;
		}
		return iTryCastImpl<Derived>();
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename U>
	operator U*() const noexcept { return TryCast<U>(); }
	
	/// インスタンスの型にconst修飾を付加したAnyPtrを取得
	AnyPtr ToConst() const noexcept
	{
		AnyPtr a = *this;
		a.m_type_id = m_type_id.ToConst();
		return a;
	}

//------------------------------------------------------------------------------
private:
	
	/// 型UからのTypeId設定
	template <typename U>
	void iSetTypeIdFrom() noexcept
	{
		DefineDerivedFromAuto<U>();
		m_type_id = MakeTypeId<U>();
	}
	
	/// ポインタ変換実装
	template <typename U>
	U* iTryCastImpl() const noexcept
	{
		// cv修飾のチェックはTryCast側で済ましている
		// Uがm_type_idの型かそのcv修飾の場合、キャストOK
		if(m_type_id.RawTypeIs<U>()){
			return static_cast<U*>(m_holder.get());
		}
		// アップキャストを試みる
		return m_type_id.info().TryUpcast<U>(m_holder.get());
	}
	
//------------------------------------------------------------------------------
private:
	
	Holder m_holder{};
	TypeId m_type_id{};
};
// << AnyPtr

} // tofu
