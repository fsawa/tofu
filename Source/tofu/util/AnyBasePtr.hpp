//------------------------------------------------------------------------------
/**
 * @file    AnyBasePtr.hpp
 * @brief   特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>
#include <tofu/util/TypeInfo.hpp>
#include <tofu/util/SafePtr.hpp>

namespace tofu {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
/// @note 特定のクラス(T)から派生したクラスのポインタと型情報を保持し、参照時に指定した型でなければnullを返す
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class AnyBasePtr
{
	typedef AnyBasePtr  self_type;
	
	typedef detail_safe_ptr::const_cast_helper<T>  const_cast_helper;
	typedef typename const_cast_helper::type  const_cast_type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	using base_type = T; ///< 扱う基底クラス
	using pointer_type = T*; ///< 扱うポインタの型
	
	using reference_type = typename detail_safe_ptr::ptr_traits<T>::reference;
	
	typedef AnyBasePtr<typename std::add_const<T>::type    >      AddConstType; ///< TがconstのPtrクラスへキャスト
	typedef AnyBasePtr<typename std::remove_const<T>::type >   RemoveConstType; ///< TがconstじゃないPtrクラスへキャスト
	typedef AnyBasePtr<typename detail_safe_ptr::reverse_const<T>::type>  ReverseConstType; ///< Tのconst修飾を反転した型
	
	static const bool  kIsConst = std::is_const<T>::value; ///< Tがconstかどうか
	
	//------------------------------------------------------------------------------
	using value_type = base_type;
	using pointer    = pointer_type;
	using reference  = reference_type;
	
// FUNCTION
	
	/// constructor
	AnyBasePtr() noexcept : m_ptr(nullptr), m_typeId() {}
	
	/// copy constructor
	AnyBasePtr( const AnyBasePtr& rhs ) noexcept
		: m_ptr(rhs.m_ptr)
		, m_typeId(rhs.m_typeId)
	{}
	
	/// nullptr代入コンストラクタ
	/*explicit*/ AnyBasePtr( nullptr_t ) noexcept : m_ptr(nullptr) {}
	
	/// 基底ポインタ代入コンストラクタ
	/*explicit*/ AnyBasePtr( pointer p ) noexcept : m_ptr(nullptr) { *this = p; }
	
	/// 派生ポインタ代入コンストラクタ
	template <typename Derived>
	/*explicit*/ AnyBasePtr( Derived* p ) noexcept : m_ptr(nullptr) { *this = p; }
	
	/// 基底ポインタ＋型を直接指定
	AnyBasePtr( pointer p, TypeId typeId ) noexcept : m_ptr(nullptr) { assign( p, typeId ); }
	
	/// destructor
	~AnyBasePtr() = default;
	
	//==============================
	
	/// copy
	AnyBasePtr&  operator=( const AnyBasePtr& rhs ) noexcept
	{
		m_ptr = rhs.m_ptr;
		m_typeId = rhs.m_typeId;
		return *this;
	}
	
	/// 代入（nullptr）
	AnyBasePtr&  operator=( nullptr_t ) noexcept
	{
		m_ptr = nullptr;
		m_typeId.clear();
		return *this;
	}
	
	/// 代入（基底ポインタから）
	AnyBasePtr&  operator=( pointer p ) noexcept
	{
		m_ptr = p;
		m_typeId = MakeTypeId<T>();
		return *this;
	}
	
	/// 代入（派生ポインタから）
	template <typename Derived>
	AnyBasePtr&  operator=( Derived* p ) noexcept
	{
		m_ptr = p;
		// 基底がconst、かつ、Derivedが非constの場合、
		// TypeIdがconstになるようにする。
		if( kIsConst && !std::is_const<Derived>::value ){
			m_typeId = MakeTypeId<const Derived>();
		}
		else{
			m_typeId = MakeTypeId<Derived>();
		}
		return *this;
	}
	
	//==============================
	
	/// 設定
	void  assign( pointer p, TypeId typeId ) noexcept
	{
		m_ptr = p;
		m_typeId = typeId;
	}
	
	/// クリア
	void  clear() noexcept
	{
		m_ptr = nullptr;
		m_typeId.clear();
	}
	
	/// TypeId取得
	const TypeId&  type() const noexcept  { return m_typeId; }
	
	/// 基底の生ポインタ取得
	pointer  get() const noexcept  { return m_ptr; }
	
	/// 基底の生ポインタ取得（nullチェック済み取得）
	pointer  safe_get() const  { null_assert(); return m_ptr; }
	
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
		if( _castTest( (Derived*)nullptr ) ){
			return _doCast<Derived>();
		}
		return nullptr;
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename Derived>
	operator Derived*() const noexcept  { return tryCast<Derived>(); }
	
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
	AnyBasePtr<const T>  makeAddConst() const noexcept  { return AnyBasePtr<const T>( m_ptr, m_typeId.makeAddConst() ); }
	
	//------------------------------------------------------------------------------
	
	/// アロー演算子
	pointer operator->() const { null_assert(); return m_ptr; }
	
	/// 参照 *
	reference operator*() const { null_assert(); return *m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// delete
	void  del()
	{
		delete m_ptr;
		clear();
	}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
	template <typename Derived>
	inline Derived*  _doCast() const
	{
		// constをあわせる
		using tmp_type = typename detail_safe_ptr::set_const<T, std::is_const<Derived>::value>::type;
		tmp_type* ptr = const_cast<tmp_type*>(m_ptr);
		return static_cast<Derived*>(ptr);
	}
	
	//------------------------------------------------------------------------------
	
	// キャスト可能か判定
	inline bool  _castTest( typename std::add_const<T>::type* ) const noexcept
	{
		return true;
	}
	
	// キャスト可能か判定
	inline bool  _castTest( typename std::remove_const<T>::type* ) const noexcept
	{
		// constから非constへのキャストはダメ
		if( kIsConst ){
			return false;
		}
		return true;
	}
	
	// キャスト可能か判定
	template <typename Derived>
	inline bool  _castTest( Derived* ) const
	{
		// constへのキャスト時は自分のconstと、
		// 非constへのキャスト時はそのまま比較。
		TypeId  my_type_id = std::is_const<Derived>::value
			? m_typeId.makeAddConst()
			: m_typeId
		;
		// 同一型？
		return MakeTypeId<Derived>() == my_type_id;
	}

// VARIABLE
	
	pointer  m_ptr;
	TypeId  m_typeId;
};
// << AnyBasePtr

//------------------------------------------------------------------------------
// AnyBasePtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <typename T, typename U>
inline bool operator ==(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() == y.get(); }

/// 比較 !=
template <typename T, typename U>
inline bool operator !=(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() != y.get(); }

/// 比較 <
template <typename T, typename U>
inline bool operator <(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() < y.get(); }

/// 比較 <=
template <typename T, typename U>
inline bool operator <=(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() <= y.get(); }

/// 比較 >
template <typename T, typename U>
inline bool operator >(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() > y.get(); }

/// 比較 >=
template <typename T, typename U>
inline bool operator >=(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() >= y.get(); }

//------------------------------------------------------------------------------

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(const AnyBasePtr<T>& x, nullptr_t)
	{ return x.get() == 0; }

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(nullptr_t, const AnyBasePtr<T>& x)
	{ return 0 == x.get(); }

/// 比較 (nullptr) !=
template <typename T>
inline bool operator !=(const AnyBasePtr<T>& x, nullptr_t)
	{ return x.get() != 0; }

/// 比較 (nullptr) !=
template <typename T>
inline bool operator !=(nullptr_t, const AnyBasePtr<T>& x)
	{ return 0 != x.get(); }

//------------------------------------------------------------------------------

} // tofu
