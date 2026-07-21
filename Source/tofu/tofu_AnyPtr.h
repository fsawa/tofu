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
	using pointer    = value_type*;

	using holder_type = Holder<value_type>; ///< ポインタを保持する型
	
public:
	
	AnyPtr() noexcept = default;
	~AnyPtr() = default;
	
	// -- move
	
	AnyPtr( AnyPtr&& rhs ) noexcept { *this = rhs; }
	AnyPtr& operator=( AnyPtr&& rhs ) noexcept
	{
		m_ptr = rhs.MoveHolder();
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
	AnyPtr( Holder<U> p ) noexcept { *this = p; }
	
	template <typename U>
	AnyPtr& operator=( Holder<U> p ) noexcept
	{
		//m_ptr = const_cast<std::remove_const_t<U>*>(p.get());
		m_ptr = p;
		iSetTypeId<U>();
		return *this;
	}
	
	//==============================

#if 0
	/// nullptr代入コンストラクタ
	/*explicit*/ AnyPtr( nullptr_t ) noexcept {}

	/// 代入（nullptr）
	AnyPtr& operator=( nullptr_t ) noexcept
	{
		reset();
		return *this;
	}
#endif
	
	//==============================
	
	/// リセット
	void reset() noexcept
	{
		m_ptr.reset();
		m_typeId.clear();
	}
	
	/// TypeId取得
	TypeId type() const noexcept { return m_typeId; }
	
	/// 基底の生ポインタ取得
	pointer get() const noexcept { return m_ptr.get(); }
	
	/// 基底の生ポインタ取得（nullチェック済み取得）
	pointer safe_get() const { null_assert(); return m_ptr.get(); }

	/// holder取得
	const holder_type& GetHolder() const { return m_ptr; }

	holder_type MoveHolder()
	{
		return std::move(m_ptr);
	}
	
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
		if(m_typeId.empty()) return nullptr;
		// constとvolatileは外せない
		if(!std::is_const_v<Derived> && m_typeId.info().IsConst()) return nullptr;
		if(!std::is_volatile_v<Derived> && m_typeId.info().IsVolatile()) return nullptr;

		return iTryCast<Derived>();
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename U>
	operator U*() const noexcept { return TryCast<U>(); }
	
	//------------------------------------------------------------------------------
	
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
	
	/// インスタンスの型にconst修飾を付加したAnyBasePtrを取得
	self_type GetAddConst() const noexcept
	{
		self_type a = *this;
		a.iSetTypeId(m_typeId.GetAddConst());
		return a;
	}

//**************************************************************
//              : private
//**************************************************************
private:
	
	void iSetTypeId(TypeId id)
	{
		m_typeId = id;
	}
	
	template <typename U>
	constexpr TypeId iMakeTypeId()
	{
		// 継承関係を自動定義
		DefineDerivedFromAuto<U>();

		return MakeTypeId<U>();
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


#if 0
////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      何かのポインタとその型情報を保持し、意図した型のポインタとして安全に取り出すクラス
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
class AnyPtr
{
	typedef AnyPtr  self_type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
// FUNCTION
	
	/// constructor
	AnyPtr() noexcept : m_ptr(nullptr), m_typeId() {}
	
	/// copy constructor
	AnyPtr( const AnyPtr& rhs ) noexcept
		: m_ptr(rhs.m_ptr)
		, m_typeId(rhs.m_typeId)
	{}
	
	/// ポインタ代入コンストラクタ
	template <typename T>
	/*explicit*/ AnyPtr( T* p ) noexcept : m_ptr(nullptr) { *this = p; }
	
	/// nullptr代入コンストラクタ
	/*explicit*/ AnyPtr( nullptr_t ) noexcept : m_ptr(nullptr) {}
	
	/// 型を直接指定
	AnyPtr( const void* p, TypeId typeId ) noexcept : m_ptr(nullptr) { assign( p, typeId ); }
	
	/// destructor
	//~AnyPtr();
	
	//==============================
	
	/// copy
	AnyPtr&  operator=( const AnyPtr& rhs ) noexcept
	{
		m_ptr = rhs.m_ptr;
		m_typeId = rhs.m_typeId;
		return *this;
	}
	
	/// 代入（生のポインタから）
	template <typename T>
	AnyPtr&  operator=( T* p ) noexcept
	{
		m_ptr = p;
		m_typeId = MakeTypeId<T>();
		return *this;
	}
	
	/// 代入（nullptr）
	AnyPtr&  operator=( nullptr_t ) noexcept
	{
		m_ptr = nullptr;
		m_typeId.clear();
		return *this;
	}
	
	//==============================
	
	/// 設定
	void  assign( const void* p, TypeId typeId ) noexcept
	{
		m_ptr = p;
		m_typeId = typeId;
	}
	
	/// クリア
	void  Clear() noexcept
	{
		m_ptr = nullptr;
		m_typeId.clear();
	}
	
	/// TypeId取得
	const TypeId&  type() const noexcept  { return m_typeId; }
	
	/// 生のポインタ取得
	const void*  get() const noexcept  { return m_ptr; }
	
	/// ポインタ変換（変換出来なかったらアサート）
	template <typename T>
	T*  Cast() const
	{
		T* ret = TryCast<T>();
		TOFU_ASSERT_MSG( ret, "[AnyPtr::Cast] failed Cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename T>
	T*  TryCast() const noexcept
	{
		if(m_typeId.empty()) return nullptr;
		// constとvolatileは外せない
		if(m_typeId.info().IsConst() && !std::is_const_v<T> ) return nullptr;
		if(m_typeId.info().IsVolatile() && !std::is_volatile_v<T> ) return nullptr;

		if( _castTest< std::is_const<T>::value >( MakeTypeId<T>() ) ){
			return reinterpret_cast<T*>( _getVoidPtr() );
		}
		// アップキャストを試みる
		return m_typeId.info().upcast<T>(_getVoidPtr());
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename T>
	operator T*() const noexcept  { return TryCast<T>(); }
	
	//------------------------------------------------------------------------------
	
	/// ポインタのnullアサートチェック
	void  null_assert() const  { TOFU_ASSERT(m_ptr); }
	
	/// boolキャスト
	operator bool() const noexcept  { return nullptr != m_ptr; }
	
	/// ポインタ未設定か
	bool  empty() const noexcept  { return nullptr == m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// インスタンスの型にconst修飾を付加したAnyPtrを取得
	AnyPtr  GetAddConst() const noexcept  { return AnyPtr( m_ptr, m_typeId.GetAddConst() ); }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
	void*  _getVoidPtr() const noexcept { return const_cast<void*>(m_ptr); }
	const void*  _getConstVoidPtr() const noexcept { return m_ptr; }
	
	template <bool IsConst>
	inline bool  _castTest( const TypeId& target ) const noexcept  { return target == m_typeId; }
	
// VARIABLE
	
	const void*  m_ptr;
	TypeId  m_typeId;
};
// << AnyPtr

// cast対象がconstの場合
template <>
inline bool  AnyPtr::_castTest<true>( const TypeId& target ) const noexcept
{
	return target == m_typeId.GetAddConst();
}

//------------------------------------------------------------------------------
// AnyPtrの２項演算子
//------------------------------------------------------------------------------

/// AnyPtr比較 ==
inline bool operator ==(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() == b.get(); }

/// AnyPtr比較 !=
inline bool operator !=(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() != b.get(); }

/// AnyPtr比較 <
inline bool operator <(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() < b.get(); }

/// AnyPtr比較 <=
inline bool operator <=(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() <= b.get(); }

/// AnyPtr比較 >
inline bool operator >(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() > b.get(); }

/// AnyPtr比較 >=
inline bool operator >=(const AnyPtr& a, const AnyPtr& b) noexcept
	{ return a.get() >= b.get(); }

//------------------------------------------------------------------------------

/// AnyPtr比較 (nullptr) ==
template <typename T>
inline bool operator ==(const AnyPtr& a, nullptr_t) noexcept
	{ return a.get() == nullptr; }

/// AnyPtr比較 (nullptr) ==
template <typename T>
inline bool operator ==(nullptr_t, const AnyPtr& a) noexcept
	{ return nullptr == a.get(); }

/// AnyPtr比較 (nullptr) !=
template <typename T>
inline bool operator !=(const AnyPtr& a, nullptr_t) noexcept
	{ return a.get() != nullptr; }

/// AnyPtr比較 (nullptr) !=
template <typename T>
inline bool operator !=(nullptr_t, const AnyPtr& a) noexcept
	{ return nullptr != a.get(); }

//------------------------------------------------------------------------------
#endif

//using AnyPtr = AnyBasePtr<void>;

} // tofu
