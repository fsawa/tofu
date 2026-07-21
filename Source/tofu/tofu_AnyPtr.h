//------------------------------------------------------------------------------
/**
 * @file    AnyPtr.h
 * @brief   何かのポインタとその型情報を保持し、意図した型のポインタとして安全に取り出すクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>
#include <tofu_TypeInfo.h>
#include <tofu_AnyBasePtr.h>

namespace tofu {
	

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
/// @note 特定のクラス(T)から派生したクラスのポインタと型情報を保持し、参照時に指定した型でなければnullを返す
////////////////////////////////////////////////////////////////////////////////////////////////
template <template <class> typename Holder = SafePtr>
class AnyPtr final
{
	using self_type = AnyPtr;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
	using value_type = void;
	using pointer    = void*;

	using holder_type = Holder<void>; ///< ポインタを保持する型
	
public:
	
	AnyPtr() noexcept = default;
	~AnyPtr() = default;
	
	// -- move
	
	AnyPtr( AnyPtr&& rhs ) noexcept { *this = rhs; }
	AnyPtr& operator=( AnyPtr&& rhs ) noexcept
	{
		m_ptr = std::move(rhs.m_ptr);
		m_typeId = rhs.type();
		rhs.reset();
		return *this;
	}

	// -- copy

	AnyPtr( const AnyPtr& rhs ) noexcept = default;
	AnyPtr& operator=( const AnyPtr& rhs ) noexcept = default;
	
	// -- from raw-pointer <U>

	template <typename U>
	AnyPtr( U* p ) noexcept
		// constのポインタも受け付ける
		: m_ptr(const_cast<std::remove_const_t<U>*>(p))
		, m_typeId(iMakeTypeId<U>())
	{
	}
	
	template <typename U>
	AnyPtr& operator=( U* p ) noexcept
	{
		// constのポインタも受け付ける
		m_ptr.reset(const_cast<std::remove_const_t<U>*>(p));
		iSetTypeId<U>();
		return *this;
	}
	
	// -- holder<U> copy
	
	template <typename U>
	AnyPtr( Holder<U> p ) noexcept { *this = std::move(p); }
	
	template <typename U>
	AnyPtr& operator=( Holder<U> p ) noexcept
	{
		m_ptr = std::move(p);
		iSetTypeId<U>();
		return *this;
	}

	// -- nullptr

	/// nullptr代入コンストラクタ
	/*explicit*/ AnyPtr( nullptr_t ) noexcept {}

	/// 代入（nullptr）
	AnyPtr& operator=( nullptr_t ) noexcept
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
	
	/// 生ポインタ取得
	pointer get() const noexcept { return m_ptr.get(); }
	
	/// ポインタのnullアサートチェック
	void null_assert() const { TOFU_ASSERT(m_ptr); }
	
	/// void生ポインタにキャスト
	operator pointer() const noexcept { return m_ptr; }
	
	/// const void生ポインタにキャスト
	operator const pointer() const noexcept { return m_ptr; }

	/// boolキャスト
	explicit operator bool() const noexcept { return nullptr != get(); }
	
	/// ポインタ未設定か
	bool empty() const noexcept { return nullptr == get(); }
	
	//------------------------------------------------------------------------------

	/// holder取得
	const holder_type& GetHolder() const { return m_ptr; }

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
		if(m_typeId.IsEmpty()) return nullptr;
		// constとvolatileは外せない
		if(!std::is_const_v<Derived> && m_typeId.info().IsConst()) return nullptr;
		if(!std::is_volatile_v<Derived> && m_typeId.info().IsVolatile()) return nullptr;

		return iTryCast<Derived>();
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename U>
	operator U*() const noexcept { return TryCast<U>(); }
	
	/// インスタンスの型にconst修飾を付加したAnyPtrを取得
	self_type ToConst() const noexcept
	{
		self_type a = *this;
		a.iSetTypeId(m_typeId.GetAddConst());
		return a;
	}

//**************************************************************
//              : private
//**************************************************************
private:
	
	template <typename U>
	constexpr TypeId iMakeTypeId()
	{
		// 継承関係を自動定義
		DefineDerivedFromAuto<U>();

		return MakeTypeId<U>();
	}
	
	void iSetTypeId(TypeId id)
	{
		m_typeId = id;
	}

	template <typename U>
	void iSetTypeId()
	{
		m_typeId = iMakeTypeId<U>();
	}
	
	template <typename U>
	U* iTryCast() const noexcept
	{
		// cv修飾のチェックはtryCast側で済ましている
		// Uがm_typeIdの型かそのcv修飾の場合、キャストOK
		if( m_typeId.info().IsSameRemoveCV<U>() ){
			return static_cast<U*>(m_ptr.get());
		}
		// アップキャストを試みる
		return m_typeId.info().TryUpcast<U>(m_ptr.get());
	}
	
private:
	
	holder_type m_ptr{};
	TypeId m_typeId{};
};
// << AnyPtr

//------------------------------------------------------------------------------
// AnyPtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <template <class> typename Holder>
constexpr bool operator ==(const AnyPtr<Holder>& x, const AnyPtr<Holder>& y) noexcept
	{ return x.get() == y.get(); }

/// 三方比較 <=>
template <template <class> typename Holder>
constexpr auto operator <=>(const AnyPtr<Holder>& x, const AnyPtr<Holder>& y) noexcept
	{ return x.get() <=> y.get(); }

/// 比較 (nullptr) ==
template <template <class> typename Holder>
constexpr bool operator ==(const AnyPtr<Holder>& x, std::nullptr_t)
	{ return x.get() == nullptr; }

/// 三方比較 (nullptr) <=>
template <template <class> typename Holder>
constexpr bool operator <=>(const AnyPtr<Holder>&x, std::nullptr_t)
	{ return x.get() <=> nullptr; }

//using AnyPtr = AnyBasePtr<void>;

} // tofu
