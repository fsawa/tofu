//------------------------------------------------------------------------------
/**
 * @file    AnyBasePtr.h
 * @brief   特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
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
/// @brief      特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
/// @note 特定のクラス(T)から派生したクラスのポインタと型情報を保持し、参照時に指定した型でなければnullを返す
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T = void, template <class> typename Holder = SafePtr >
class AnyBasePtr
{
	using self_type = AnyBasePtr;
	
	using const_cast_helper = detail_safe_ptr::const_cast_helper<T>;
	using const_cast_type = typename const_cast_helper::type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
	using base_type = T; ///< 扱う基底クラス
	using pointer_type = T*; ///< 扱うポインタの型
	using holder_type = Holder<T>; ///< ポインタを保持する型
	
	using reference_type = typename detail_safe_ptr::ptr_traits<T>::reference;
	
	using ConstType     = AnyBasePtr<typename std::add_const<T>::type    >; ///< TがconstのPtrクラスへキャスト
	using NotConstType  = AnyBasePtr<typename std::remove_const<T>::type >; ///< TがconstじゃないPtrクラスへキャスト
	using ReverseConstType = AnyBasePtr<typename detail_safe_ptr::reverse_const<T>::type>; ///< Tのconst修飾を反転した型
	
	static constexpr bool IsVoid  = std::is_same_v<std::remove_cv_t<T>, void>; ///< Tがvoidかどうか
	static constexpr bool IsConst = std::is_const_v<T>;      ///< Tがconstかどうか
	static constexpr bool IsVolatile = std::is_volatile_v<T>;      ///< Tがvolatileかどうか
	
	//------------------------------------------------------------------------------
	using value_type = base_type;
	using pointer    = pointer_type;
	using reference  = reference_type;
	
public:
	
	AnyBasePtr() noexcept = default;
	~AnyBasePtr() = default;
	
	// -- move
	
	AnyBasePtr( AnyBasePtr&& rhs ) noexcept { *this = rhs; }
	AnyBasePtr& operator=( AnyBasePtr&& rhs ) noexcept
	{
		m_ptr = rhs.MoveHolder();
		m_typeId = rhs.type();
		rhs.reset();
		return *this;
	}

	// -- copy

	AnyBasePtr( const AnyBasePtr& rhs ) noexcept = default;
	AnyBasePtr&  operator=( const AnyBasePtr& rhs ) noexcept = default;
	
	// -- move <U> / void
	
	template <typename U>
		requires IsVoid
	AnyBasePtr( AnyBasePtr<U, Holder>&& rhs ) noexcept { *this = std::move(rhs); }

	template <typename U>
		requires IsVoid
	AnyBasePtr& operator=( AnyBasePtr<U, Holder>&& rhs ) noexcept
	{
		m_ptr = rhs.MoveHolder();
		iSetTypeId(rhs.type());
		rhs.reset();
		return *this;
	}
	
	// -- move <U> / derived

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr( AnyBasePtr<U, Holder>&& rhs ) noexcept { *this = std::move(rhs); }

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr& operator=( AnyBasePtr<U, Holder>&& rhs ) noexcept
	{
		m_ptr = rhs.MoveHolder();
		iSetTypeId(rhs.type());
		rhs.reset();
		return *this;
	}

	// -- copy <U> / void

	template <typename U>
		requires IsVoid
	AnyBasePtr( const AnyBasePtr<U, Holder>& rhs ) noexcept { *this = rhs; }
	
	template <typename U>
		requires IsVoid
	AnyBasePtr&  operator=( const AnyBasePtr<U, Holder>& rhs ) noexcept
	{
		m_ptr = rhs.GetHolder();
		iSetTypeId(rhs.type());
		return *this;
	}

	// -- copy <U> / derived

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr( const AnyBasePtr<U, Holder>& rhs ) noexcept { *this = rhs; }

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr&  operator=( const AnyBasePtr<U, Holder>& rhs ) noexcept
	{
		m_ptr = rhs.GetHolder();
		iSetTypeId(rhs.type());
		return *this;
	}
	
	// -- from raw-pointer <U> / void

	template <typename U>
		requires IsVoid
	AnyBasePtr( U* p ) noexcept
		// T=void の場合はconstのポインタも受け付ける
		: m_ptr(const_cast<std::remove_const_t<U>*>(p))
		, m_typeId(iMakeTypeId<U>())
	{
	}
	
	template <typename U>
		requires IsVoid
	AnyBasePtr&  operator=( U* p ) noexcept
	{
		// T=void の場合はconstのポインタも受け付ける
		m_ptr.reset(const_cast<std::remove_const_t<U>*>(p));
		iSetTypeId<U>();
		return *this;
	}
	
	// -- from raw-pointer <U> / derived

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr( U* p ) noexcept
		: m_ptr(p)
		, m_typeId(iMakeTypeId<U>())
	{
	}

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr&  operator=( U* p ) noexcept
	{
		m_ptr.reset(p);
		iSetTypeId<U>();
		return *this;
	}
	
	// -- holder<U> copy / void
	
	template <typename U>
		requires IsVoid
	AnyBasePtr( Holder<U> p ) noexcept { *this = p; }
	
	template <typename U>
		requires IsVoid
	AnyBasePtr&  operator=( Holder<U> p ) noexcept
	{
		//m_ptr = const_cast<std::remove_const_t<U>*>(p.get());
		m_ptr = p;
		iSetTypeId<U>();
		return *this;
	}
	
	// -- holder<U> copy / derived

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr( Holder<U> p ) noexcept { *this = p; }

	template <typename U>
		requires std::derived_from<U, T>
	AnyBasePtr&  operator=( Holder<U> p ) noexcept
	{
		m_ptr = p;
		iSetTypeId<U>();
		return *this;
	}
	
	//==============================

#if 0
	/// nullptr代入コンストラクタ
	/*explicit*/ AnyBasePtr( nullptr_t ) noexcept {}

	/// 代入（nullptr）
	AnyBasePtr&  operator=( nullptr_t ) noexcept
	{
		reset();
		return *this;
	}
#endif
	
	//==============================
	
	/// リセット
	void  reset() noexcept
	{
		m_ptr.reset();
		m_typeId.clear();
	}
	
	/// TypeId取得
	TypeId  type() const noexcept  { return m_typeId; }
	
	/// 基底の生ポインタ取得
	pointer  get() const noexcept  { return m_ptr.get(); }
	
	/// 基底の生ポインタ取得（nullチェック済み取得）
	pointer  safe_get() const  { null_assert(); return m_ptr.get(); }

	/// holder取得
	const holder_type& GetHolder() const { return m_ptr; }

	holder_type MoveHolder()
	{
		return std::move(m_ptr);
	}
	
	/// ポインタ変換（変換出来なかったらアサート）
	template <typename Derived>
	Derived*  cast() const
	{
		Derived* ret = tryCast<Derived>();
		TOFU_ASSERT_MSG( ret, "[AnyBasePtr::cast] failed cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename Derived>
	Derived*  tryCast() const noexcept
	{
		// constとvolatileは外せない
		if constexpr (IsConst && !std::is_const_v<Derived>){
			return nullptr;
		}
		else if constexpr (IsVolatile && !std::is_volatile_v<Derived>){
			return nullptr;
		}
		else
		{
			if(m_typeId.empty()) return nullptr;
			// constとvolatileは外せない
			if(!std::is_const_v<Derived> && m_typeId.info().isConst()) return nullptr;
			if(!std::is_volatile_v<Derived> && m_typeId.info().isVolatile()) return nullptr;

			return _tryCast<Derived>();
		}
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename U>
	operator U*() const noexcept  { return tryCast<U>(); }
	
	//------------------------------------------------------------------------------
	
	/// ポインタのnullアサートチェック
	void  null_assert() const  { TOFU_ASSERT(m_ptr); }
	
	/// 基底の生ポインタにキャスト
	operator pointer() const noexcept  { return m_ptr; }
	
	/// 基底の生ポインタにキャスト ( const付加 )
	operator const_cast_type() const noexcept  { return m_ptr; }

	/// boolキャスト
	operator bool() const noexcept  { return nullptr != m_ptr; }
	
	/// ポインタ未設定か
	bool  empty() const noexcept  { return nullptr == m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// インスタンスの型にconst修飾を付加したAnyBasePtrを取得
	AnyBasePtr<const T>  makeAddConst() const noexcept
	{
		return AnyBasePtr<const T>( *this );
	}
	
	//------------------------------------------------------------------------------
	
	/// アロー演算子
	pointer operator->() const { null_assert(); return m_ptr; }
	
	/// 参照 *
	reference operator*() const { null_assert(); return *m_ptr; }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION

	static inline TypeId iConvertType(TypeId id)
	{
		if constexpr ( IsConst ){
			return id.makeAddConst();
		}
		else{
			return id;
		}
	}
	
	void iSetTypeId(TypeId id)
	{
		m_typeId = iConvertType(id);
	}
	
	template <typename U>
		requires IsVoid
	TypeId iMakeTypeId()
	{
		return MakeTypeId<U>();
	}

	template <typename U>
		requires std::derived_from<U, T>
	TypeId iMakeTypeId()
	{
		// 基底クラス自動登録
		//SetBaseType<T, U>();

		// 基底がconst、かつ、Uが非constの場合、
		// TypeIdがconstになるようにする。
		if constexpr ( IsConst ){
			return MakeTypeId<const U>();
		}
		else{
			return MakeTypeId<U>();
		}
	}

	template <typename U>
	void iSetTypeId()
	{
		m_typeId = iMakeTypeId<U>();
	}
	
	template <typename U>
		requires IsVoid
	U* _tryCast() const noexcept
	{
		// cv修飾のチェックはtryCast側で済ましている
		// Uがm_typeIdの型かそのcv修飾の場合、キャストOK
		if( m_typeId.info().IsSameRemoveCV<U>() ){
			return static_cast<U*>(m_ptr.get());
		}
		// アップキャストを試みる
		return m_typeId.info().TryUpcast<U>(m_ptr.get());
	}

	template <typename U>
		requires std::derived_from<U, T>
	U* _tryCast() const noexcept
	{
		// cv修飾のチェックはtryCast側で済ましている
		// UもしくはUから派生したクラスを保持している
		if(m_typeId.info().IsDerivedFrom<U>())
		{
			// T*をU*にキャストする
			return static_cast<U*>(m_ptr.get());
		}
		return nullptr;
	}
	
	//------------------------------------------------------------------------------
	
	//const void*  _getConstVoidPtr() const noexcept { return m_ptr.get(); }
	//void*  _getVoidPtr() const noexcept { return const_cast<void*>(_getConstVoidPtr()); }

// VARIABLE
	
	holder_type m_ptr{};
	TypeId m_typeId{};
};
// << AnyBasePtr

//------------------------------------------------------------------------------
// AnyBasePtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <typename T, typename U, template <class> typename Holder>
inline bool operator ==(const AnyBasePtr<T, Holder>& x, const AnyBasePtr<U, Holder>& y)
	{ return x.get() == y.get(); }

/// 三方比較 <=>
template <typename T, typename U, template <class> typename Holder>
constexpr auto operator <=>(AnyBasePtr<T, Holder>a, AnyBasePtr<U, Holder>b)
	{ return a.get() <=> b.get(); }

//------------------------------------------------------------------------------

/// 比較 (nullptr) ==
template <typename T, template <class> typename Holder>
inline bool operator ==(const AnyBasePtr<T, Holder>& x, nullptr_t)
	{ return x.get() == nullptr; }

/// 比較 (nullptr) ==
template <typename T, template <class> typename Holder>
inline bool operator ==(nullptr_t, const AnyBasePtr<T, Holder>& x)
	{ return nullptr == x.get(); }

//------------------------------------------------------------------------------

} // tofu
