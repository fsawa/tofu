//------------------------------------------------------------------------------
/**
 * @file    SafePtr.hpp
 * @brief   安全対策の為のポインタラッパークラス
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

namespace detail_safe_ptr
{
	template<typename T> struct ptr_traits
	{
		using reference = T&;
	};

	template<> struct ptr_traits<void>
	{
		using reference = void;
	};

	template<> struct ptr_traits<const void>
	{
		using reference = const void;
	};
	
	//------------------------------------------------------------------------------
	
	// constを付与
	template<typename T> struct const_cast_helper
	{
		using type = const T*; // const追加
		
		// T* を const T* に
		static inline type cast( T* m_p ) noexcept { return m_p; }
	};
	
	// Tがconstの場合はポインタに変換しない
	template<typename T> struct const_cast_helper<const T>
	{
		class dummy_t;
		
		using type = const dummy_t;
		
		// const dummy_t を返す。ポインタにしない。
		static inline type cast( const T* ) noexcept { return type(); }
	};
	
	//------------------------------------------------------------------------------
	
	// const設定
	template <typename TargetType, bool IsConst>
	struct set_const
	{
		using type = typename std::remove_const<TargetType>::type;
	};
	
	template <typename TargetType>
	struct set_const<TargetType, true>
	{
		using type = typename std::add_const<TargetType>::type;
	};
	
	//------------------------------------------------------------------------------

	// const修飾を反転
	template <typename T> struct reverse_const { using type = T const; };
	template <typename T> struct reverse_const<T const> { using type = T; };
	
}// detail_safe_ptr


////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      安全対策の為のポインタラッパークラス
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class SafePtr
{
	using self_type = SafePtr;
	
	using const_cast_helper = detail_safe_ptr::const_cast_helper<T>;
	using const_cast_type = typename const_cast_helper::type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	using value_type = T; ///< 扱う型
	using pointer_type = T*; ///< 扱うポインタの型
	using reference_type = typename detail_safe_ptr::ptr_traits<T>::reference; /// <扱う型の参照型
	
	using     add_const_type = SafePtr<typename std::add_const<T>::type    >; ///< TがconstのPtrクラスへキャスト
	using  remove_const_type = SafePtr<typename std::remove_const<T>::type >; ///< TがconstじゃないPtrクラスへキャスト
	using reverse_const_type = SafePtr<typename detail_safe_ptr::reverse_const<T>::type>; ///< Tのconst修飾を反転した型
	
	static const bool  kIsConst = std::is_const<T>::value; ///< Tがconstかどうか
	
// FUNCTION
	
	SafePtr() noexcept : m_p(nullptr) {}
	
	SafePtr( const SafePtr& rhs ) noexcept : m_p(rhs.m_p) {}
	
	/*explicit*/ SafePtr( pointer_type p_ ) noexcept : m_p(p_) {}
	
	//------------------------------------------------------------------------------
	
	SafePtr&  operator=( const SafePtr& rhs ) noexcept
	{
		this->m_p = rhs.m_p;
		return *this;
	}
	
	SafePtr&  operator=( pointer_type ptr ) noexcept
	{
		this->m_p = ptr;
		return *this;
	}
	
	//------------------------------------------------------------------------------
	
	/// ポインタがnullならアサートにする
	void  null_assert() const  { TOFU_ASSERT(m_p != nullptr); }
	
	/// 生のポインタにキャスト
	operator pointer_type() const noexcept  { return m_p; }
	
	/// const付与した生のポインタにキャスト
	operator const_cast_type() const  { return const_cast_helper::cast(m_p); }
	
	/// bool値にキャスト
	operator bool() const noexcept  { return nullptr != m_p; }
	
	/// 空（=null）かどうか
	bool  empty() const noexcept  { return nullptr == m_p; }
	
	/// ポインタ取得
	pointer_type  get() const noexcept  { return m_p; }
	
	/// nullじゃないことを保証（nullならアサート）するポインタ取得
	pointer_type  safe() const  { null_assert(); return m_p; }
	
	//------------------------------------------------------------------------------
	// ポインタ演算子
	//------------------------------------------------------------------------------
	
	/// アロー演算子
	pointer_type operator->() const { null_assert(); return m_p; }
	
	/// 参照 *
	reference_type operator*() const { null_assert(); return *m_p; }
	
	/// 参照 []
	reference_type operator[](int n) const { null_assert(); return m_p[n]; }
	
	/// ++前置
	self_type& operator++() { null_assert(); ++m_p; return *this; }
	
	/// 後置++
	self_type operator++(int) { null_assert(); return m_p++; }
	
	/// --前置
	self_type& operator--() { null_assert(); --m_p; return *this; }
	
	/// 後置--
	self_type operator--(int) { null_assert(); return m_p--; }
	
	/// 加算
	self_type& operator+=( int n ) { null_assert(); m_p += n; return *this; }
	
	/// 減算
	self_type& operator-=( int n ) { null_assert(); m_p -= n; return *this; }

	//------------------------------------------------------------------------------
	// 特種な拡張
	//------------------------------------------------------------------------------
	
	/// クリア
	void  clear() noexcept { m_p = nullptr; }
	
	/// deleteを実行
	void  del()
	{
		delete m_p;
		clear();
	}
	
	/// delete[]を実行（本当に配列かどうかは、使う側の責任）
	void  delArray()
	{
		delete[] m_p;
		clear();
	}
	
// VARIABLE
	
	pointer_type  m_p; ///< 生のポインタ（あえてpublicに）

private:
	
	/// 整数への暗黙のキャスト抑制
	//operator int() const;
};
// << SafePtr

//------------------------------------------------------------------------------
// SafePtr間の２項演算子
//------------------------------------------------------------------------------

/// 加算
template <typename T>
inline SafePtr<T> operator+( const SafePtr<T>& ptr, int n )
	{ return ptr.safe() + n; }

/// 減算
template <typename T>
inline SafePtr<T> operator-( const SafePtr<T>& ptr, int n )
	{ return ptr.safe() - n; }

/// 距離
template <typename T, typename U>
inline ptrdiff_t operator -(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.safe() - b.safe(); }

/// 比較 ==
template <typename T, typename U>
inline bool operator ==(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p == b.m_p; }

/// 比較 !=
template <typename T, typename U>
inline bool operator !=(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p != b.m_p; }

/// 比較 <
template <typename T, typename U>
inline bool operator <(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p < b.m_p; }

/// 比較 <=
template <typename T, typename U>
inline bool operator <=(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p <= b.m_p; }

/// 比較 >
template <typename T, typename U>
inline bool operator >(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p > b.m_p; }

/// 比較 >=
template <typename T, typename U>
inline bool operator >=(const SafePtr<T>& a, const SafePtr<U>& b)
	{ return a.m_p >= b.m_p; }

//------------------------------------------------------------------------------
// nullptrとの比較
//------------------------------------------------------------------------------

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(const SafePtr<T>& a, nullptr_t)
	{ return a.m_p == nullptr; }

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(nullptr_t, const SafePtr<T>& a)
	{ return nullptr == a.m_p; }

/// 比較 (nullptr) !=
template <typename T>
inline bool operator !=(const SafePtr<T>& a, nullptr_t)
	{ return a.m_p != nullptr; }

/// 比較 (nullptr) !=
template <typename T>
inline bool operator !=(nullptr_t, const SafePtr<T>& a)
	{ return nullptr != a.m_p; }

//------------------------------------------------------------------------------

} // tofu
