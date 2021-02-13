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
#include <tofu/util/TypeInfo.hpp>

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
	void  clear() noexcept
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
	T*  cast() const
	{
		T* ret = tryCast<T>();
		TOFU_ASSERT_MSG( ret, "[AnyPtr::cast] failed cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename T>
	T*  tryCast() const noexcept
	{
		if(m_typeId.empty()) return nullptr;
		// constとvolatileは外せない
		if(m_typeId.info().isConst() && !std::is_const<T>::value ) return nullptr;
		if(m_typeId.info().isVolatile() && !std::is_volatile<T>::value ) return nullptr;

		if( _castTest< std::is_const<T>::value >( MakeTypeId<T>() ) ){
			return reinterpret_cast<T*>( _getVoidPtr() );
		}
		// アップキャストを試みる
		return m_typeId.info().upcast<T>(_getVoidPtr());
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename T>
	operator T*() const noexcept  { return tryCast<T>(); }
	
	//------------------------------------------------------------------------------
	
	/// ポインタのnullアサートチェック
	void  null_assert() const  { TOFU_ASSERT(m_ptr); }
	
	/// boolキャスト
	operator bool() const noexcept  { return nullptr != m_ptr; }
	
	/// ポインタ未設定か
	bool  empty() const noexcept  { return nullptr == m_ptr; }
	
	//------------------------------------------------------------------------------
	
	/// インスタンスの型にconst修飾を付加したAnyPtrを取得
	AnyPtr  makeAddConst() const noexcept  { return AnyPtr( m_ptr, m_typeId.makeAddConst() ); }
	
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
	return target == m_typeId.makeAddConst();
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

} // tofu
