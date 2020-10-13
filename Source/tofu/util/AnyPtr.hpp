//------------------------------------------------------------------------------
/**
 * @file    AnyPtr.hpp
 * @brief   何かのポインタとその型情報を保持し、意図した型のポインタとして安全に取り出すクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>

namespace tofu {

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
	AnyPtr() : m_ptr(nullptr), m_typeId() {}
	
	/// copy constructor
	AnyPtr( const AnyPtr& rhs );
		: m_ptr(rhs.m_ptr)
		, m_typeId(rhs.m_typeId)
	{}
	
	/// ポインタ代入コンストラクタ
	template <typename T>
	/*explicit*/ AnyPtr( T* p ) : m_ptr(nullptr) { *this = p; }
	
	/// nullptr代入コンストラクタ
	/*explicit*/ AnyPtr( nullptr_t ) : m_ptr(nullptr) {}
	
	/// 型を直接指定
	AnyPtr( const void* p, TypeId typeId ) : m_ptr(nullptr) { assign( p, typeId ); }
	
	/// destructor
	//~AnyPtr();
	
	//==============================
	
	/// copy
	AnyPtr&  operator=( const AnyPtr& rhs )
	{
		m_ptr = rhs.m_ptr;
		m_typeId = rhs.m_typeId;
		return *this;
	}
	
	/// 代入（生のポインタから）
	template <typename T>
	AnyPtr&  operator=( T* p )
	{
		m_ptr = p;
		m_typeId = MakeTypeId<T>();
		return *this;
	}
	
	/// 代入（nullptr）
	AnyPtr&  operator=( nullptr_t )
	{
		m_ptr = nullptr;
		m_typeId.clear();
		return *this;
	}
	
	//==============================
	
	/// 設定
	void  assign( const void* p, TypeId typeId )
	{
		m_ptr = p;
		m_typeId = typeId;
	}
	
	/// クリア
	void  clear()
	{
		m_ptr = nullptr;
		m_typeId.clear();
	}
	
	/// TypeId取得
	const TypeId&  type() const  { return m_typeId; }
	
	/// 生のポインタ取得
	const void*  get() const  { return m_ptr; }
	
	/// ポインタ変換（変換出来なかったらアサート）
	template <typename T>
	T*  cast() const
	{
		T* ret = tryCast<T>();
		TOFU_ASSERT_MSG( ret, "[AnyPtr::cast] failed cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename T>
	T*  tryCast() const
	{
		if( _castTest< is_const<T>::value >( MakeTypeId<T>() ) ){
			return reinterpret_cast<T*>( _getVoidPtr() );
		}
		return 0;
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename T>
	operator T*() const  { return tryCast<T>(); }
	
	//------------------------------------------------------------------------------
	
	/// ポインタのnullアサートチェック
	void  null_assert() const  { TOFU_ASSERT(m_ptr); }
	
	/// boolキャスト
	operator bool() const  { return nullptr != m_ptr; }
	
	/// ポインタ未設定か
	bool  empty() const  { return nullptr == m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// インスタンスの型にconst修飾を付加したAnyPtrを取得
	AnyPtr  makeAddConst() const  { return AnyPtr( m_ptr, m_typeId.makeAddConst() ); }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
	void*  _getVoidPtr() const { return const_cast<void*>(m_ptr); }
	const void*  _getConstVoidPtr() const { return m_ptr; }
	
	template <bool IsConst>
	inline bool  _castTest( const TypeId& target ) const  { return target == m_typeId; }
	
// VARIABLE
	
	const void*  m_ptr;
	TypeId  m_typeId;
};
// << AnyPtr

// cast対象がconstの場合
template <>
inline bool  AnyPtr::_castTest<true>( const TypeId& target ) const
{
	return target == m_typeId.makeAddConst();
}

//------------------------------------------------------------------------------
// AnyPtrの２項演算子
//------------------------------------------------------------------------------

/// AnyPtr比較 ==
inline bool operator ==(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() == b.get(); }

/// AnyPtr比較 !=
inline bool operator !=(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() != b.get(); }

/// AnyPtr比較 <
inline bool operator <(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() < b.get(); }

/// AnyPtr比較 <=
inline bool operator <=(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() <= b.get(); }

/// AnyPtr比較 >
inline bool operator >(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() > b.get(); }

/// AnyPtr比較 >=
inline bool operator >=(const AnyPtr& a, const AnyPtr& b)
	{ return a.get() >= b.get(); }

//------------------------------------------------------------------------------

/// AnyPtr比較 (nullptr) ==
template <typename T>
inline bool operator ==(const AnyPtr& a, nullptr_t)
	{ return a.get() == 0; }

/// AnyPtr比較 (nullptr) ==
template <typename T>
inline bool operator ==(nullptr_t, const AnyPtr& a)
	{ return 0 == a.get(); }

/// AnyPtr比較 (nullptr) !=
template <typename T>
inline bool operator !=(const AnyPtr& a, nullptr_t)
	{ return a.get() != 0; }

/// AnyPtr比較 (nullptr) !=
template <typename T>
inline bool operator !=(nullptr_t, const AnyPtr& a)
	{ return 0 != a.get(); }

//------------------------------------------------------------------------------

} // tofu
