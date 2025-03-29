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
template <typename T, template <class> typename Holder = SafePtr >
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
	
	using AddConstType     = AnyBasePtr<typename std::add_const<T>::type    >; ///< TがconstのPtrクラスへキャスト
	using RemoveConstType  = AnyBasePtr<typename std::remove_const<T>::type >; ///< TがconstじゃないPtrクラスへキャスト
	using ReverseConstType = AnyBasePtr<typename detail_safe_ptr::reverse_const<T>::type>; ///< Tのconst修飾を反転した型
	
	static constexpr bool  IsConst = std::is_const<T>::value; ///< Tがconstかどうか
	
	//------------------------------------------------------------------------------
	using value_type = base_type;
	using pointer    = pointer_type;
	using reference  = reference_type;
	
public:
	
	AnyBasePtr() noexcept = default;
	~AnyBasePtr() = default;
	
	// -- move
	
	AnyBasePtr( AnyBasePtr&& rhs ) noexcept = default;
	AnyBasePtr& operator=( AnyBasePtr&& rhs ) noexcept = default;

	// -- copy

	AnyBasePtr( const AnyBasePtr& rhs ) noexcept = default;
	AnyBasePtr&  operator=( const AnyBasePtr& rhs ) noexcept = default;
	
	// --

	/// nullptr代入コンストラクタ
	/*explicit*/ //AnyBasePtr( nullptr_t ) noexcept {}
	
	/// 基底ポインタ代入コンストラクタ
	/*explicit*/ AnyBasePtr( pointer p ) noexcept { *this = p; }

	/// 派生ポインタ代入コンストラクタ
	template <typename Derived>
	/*explicit*/ AnyBasePtr( Derived* p ) noexcept { *this = p; }
	
	/// 基底ポインタ＋型を直接指定
	AnyBasePtr( pointer p, TypeId typeId ) noexcept { assign( p, typeId ); }
	
	//==============================
	
	// -- holder copy
	
	explicit AnyBasePtr( holder_type p ) noexcept { *this = std::move(p); }
	
	AnyBasePtr&  operator=( holder_type p ) noexcept
	{
		m_ptr = std::move(p);
		m_typeId = MakeTypeId<T>();
		return *this;
	}

	// -- holder<U> copy
	
	template <typename U>
	AnyBasePtr( Holder<U> p ) noexcept { *this = std::move(p); }
	
	template <typename U>
	AnyBasePtr&  operator=( Holder<U> p ) noexcept
	{
		m_ptr = std::move(p);
		// 基底がconst、かつ、Uが非constの場合、
		// TypeIdがconstになるようにする。
		if constexpr ( IsConst && !std::is_const_v<U> ){
			m_typeId = MakeTypeId<const U>();
		}
		else{
			m_typeId = MakeTypeId<U>();
		}
		return *this;
	}
	
	//==============================

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
		return *this = holder_type(p);
	}
	
	/// 代入（派生ポインタから）
	template <typename U>
	AnyBasePtr&  operator=( U* p ) noexcept
	{
		return *this = Holder<U>(p);
	}
	
	//==============================
	
	/// 設定
	void  assign( pointer p, TypeId typeId ) noexcept
	{
		m_ptr = p;
		m_typeId = typeId;
	}
	
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
		if(m_typeId.empty()) return nullptr;
		// constとvolatileは外せない
		if(m_typeId.info().isConst() && !std::is_const_v<Derived> ) return nullptr;
		if(m_typeId.info().isVolatile() && !std::is_volatile_v<Derived> ) return nullptr;

		if( _castTest( (Derived*)nullptr ) ){
			return _doCast<Derived>();
		}
		// アップキャストを試みる
		return m_typeId.info().upcast<Derived>(_getVoidPtr());
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
		tmp_type* ptr = const_cast<tmp_type*>(m_ptr.get());
		return static_cast<Derived*>(ptr);
	}
	
	//------------------------------------------------------------------------------
	
	const void*  _getConstVoidPtr() const noexcept { return m_ptr.get(); }
	void*  _getVoidPtr() const noexcept { return const_cast<void*>(_getConstVoidPtr()); }

	// キャスト可能か判定
	constexpr bool  _castTest( std::add_const_t<T>* ) const noexcept
	{
		return true;
	}
	
	// キャスト可能か判定
	constexpr bool  _castTest( std::remove_const_t<T>* ) const noexcept
	{
		// constから非constへのキャストはダメ
		if constexpr ( IsConst ){
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
		TypeId  my_type_id = std::is_const_v<Derived>
			? m_typeId.makeAddConst()
			: m_typeId
		;
		// 同一型？
		return MakeTypeId<Derived>() == my_type_id;
	}

// VARIABLE
	
	holder_type  m_ptr{};
	TypeId  m_typeId{};
};
// << AnyBasePtr

//------------------------------------------------------------------------------
// AnyBasePtrの２項演算子
//------------------------------------------------------------------------------

/// 比較 ==
template <typename T, typename U>
inline bool operator ==(const AnyBasePtr<T>& x, const AnyBasePtr<U>& y)
	{ return x.get() == y.get(); }

/// 三方比較 <=>
template <typename T, typename U>
constexpr auto operator <=>(AnyBasePtr<T>a, AnyBasePtr<U>b)
	{ return a.get() <=> b.get(); }

//------------------------------------------------------------------------------

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(const AnyBasePtr<T>& x, nullptr_t)
	{ return x.get() == nullptr; }

/// 比較 (nullptr) ==
template <typename T>
inline bool operator ==(nullptr_t, const AnyBasePtr<T>& x)
	{ return nullptr == x.get(); }

//------------------------------------------------------------------------------

} // tofu
