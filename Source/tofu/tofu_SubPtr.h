//------------------------------------------------------------------------------
/**
 * @file    SubPtr.h
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
	
/// @brief concept : cv修飾なしか
template <class T>
concept not_cv =
	!std::is_const_v<T> && !std::is_volatile_v<T>;

/// @brief concept : cv修飾がcast可能な関係か
template <class From, class To>
concept castable_cv_to =
	!(std::is_const_v<From> && !std::is_const_v<To>) &&
	!(std::is_volatile_v<From> && !std::is_volatile_v<To>);

/// @brief concept : 安全にcast可能か
template <class From, class To>
concept safe_castable_to =
	castable_cv_to<From, To> && 
	std::derived_from<From, To>;

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      特定のクラスから派生したクラスのポインタと型情報を保持するポインタクラス
/// @note 特定のクラス(T)から派生したクラスのポインタと型情報を保持し、参照時に指定した型でなければnullを返す
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, template <class> typename Holder = SafePtr >
class SubPtr final
{
	using self_type = SubPtr;
	
	using const_cast_helper = detail_safe_ptr::const_cast_helper<T>;
	using const_cast_type = typename const_cast_helper::type;
	
	template <typename BaseU, template <class> typename HolderU>
    friend class SubPtr;

//**************************************************************
//              : public
//**************************************************************
public:
	
	using base_type = T; ///< 扱う基底クラス
	using pointer_type = T*; ///< 扱うポインタの型
	using holder_type = Holder<T>; ///< ポインタを保持する型
	
	using reference_type = typename detail_safe_ptr::ptr_traits<T>::reference;
	
	using ConstType     = SubPtr<typename std::add_const<T>::type    >; ///< TがconstのPtrクラスへキャスト
	using NotConstType  = SubPtr<typename std::remove_const<T>::type >; ///< TがconstじゃないPtrクラスへキャスト
	using ReverseConstType = SubPtr<typename detail_safe_ptr::reverse_const<T>::type>; ///< Tのconst修飾を反転した型
	
	static constexpr bool IsConst = std::is_const_v<T>;      ///< Tがconstかどうか
	static constexpr bool IsVolatile = std::is_volatile_v<T>;      ///< Tがvolatileかどうか
	
	//------------------------------------------------------------------------------
	using value_type = base_type;
	using pointer    = pointer_type;
	using reference  = reference_type;
	
public:
	
	SubPtr() noexcept = default;
	~SubPtr() = default;
	
	// -- move
	
	SubPtr( SubPtr&& rhs ) noexcept { *this = rhs; }
	SubPtr& operator=( SubPtr&& rhs ) noexcept
	{
		m_ptr = std::move(rhs.m_ptr);
		m_typeId = rhs.type();
		rhs.reset();
		return *this;
	}

	// -- copy

	SubPtr( const SubPtr& rhs ) noexcept = default;
	SubPtr& operator=( const SubPtr& rhs ) noexcept = default;
	
	// -- move <U> / derived

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( SubPtr<U, Holder>&& rhs ) noexcept { *this = std::move(rhs); }

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( SubPtr<U, Holder>&& rhs ) noexcept
	{
		m_ptr = std::move(rhs.m_ptr);
		iSetTypeId(rhs.type());
		rhs.reset();
		return *this;
	}

	// -- copy <U> / derived

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( const SubPtr<U, Holder>& rhs ) noexcept { *this = rhs; }

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( const SubPtr<U, Holder>& rhs ) noexcept
	{
		m_ptr = rhs.GetHolder();
		iSetTypeId(rhs.type());
		return *this;
	}
	
	// -- from raw-pointer <U> / derived

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( U* p ) noexcept
		: m_ptr(p)
		, m_typeId()
	{
		iSetTypeId<U>();
	}

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( U* p ) noexcept
	{
		m_ptr.reset(p);
		iSetTypeId<U>();
		return *this;
	}
	
	// -- holder<U> copy / derived

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr( Holder<U> p ) noexcept { *this = p; }

	template <typename U>
	requires safe_castable_to<U, T>
	SubPtr& operator=( Holder<U> p ) noexcept
	{
		m_ptr = p;
		iSetTypeId<U>();
		return *this;
	}
	
	// -- nullptr

	/// nullptr代入コンストラクタ
	SubPtr( nullptr_t ) noexcept {}

	/// 代入（nullptr）
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
	void null_assert() const { TOFU_ASSERT(m_ptr); }
	
	/// 基底の生ポインタにキャスト
	operator pointer() const noexcept { return m_ptr; }
	
	/// 基底の生ポインタにキャスト ( const付加 )
	operator const_cast_type() const noexcept { return m_ptr; }

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
	Derived* Cast() const
	{
		Derived* ret = TryCast<Derived>();
		TOFU_ASSERT_MSG( ret, "[SubPtr::Cast] failed Cast.\n" );
		return ret;
	}
	
	/// ポインタ変換（変換出来なかったらnullptr）
	template <typename Derived>
	Derived* TryCast() const noexcept
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
			if(m_typeId.IsEmpty()) return nullptr;
			// constとvolatileは外せない
			// →T=非constの場合にconstが入ることはないし、T=constの場合は↑でチェックしている
			//if(!std::is_const_v<Derived> && m_typeId.info().IsConst()) return nullptr;
			//if(!std::is_volatile_v<Derived> && m_typeId.info().IsVolatile()) return nullptr;

			return iTryCastImpl<Derived>();
		}
	}
	
	/// 暗黙的キャスト（型が違ったらnullptr）
	template <typename U>
	operator U*() const noexcept { return TryCast<U>(); }
	
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
		// 継承関係を自動定義
		DefineDerivedFromAuto<T>();
	}

	/// TypeIdのCV修飾を、このクラスのbase_typeに合わせる
	static inline TypeId ConvertType(TypeId id)
	{
		if constexpr ( IsConst ){
			return id.GetAddConst();
		}
		else{
			return id;
		}
	}

	// 代入可能チェック
	// 非constにconstは入れられない
	//template <typename U>
	
	void iSetTypeId(TypeId id) noexcept
	{
		m_typeId = ConvertType(id);
	}
	
	template <typename U>
	requires safe_castable_to<U, T>
	constexpr TypeId iMakeTypeId() noexcept
	{
		// 継承関係を自動定義
		DefineDerivedFrom<U, T>();
		//DefineDerivedFromAuto<T>();
		DefineDerivedFromAuto<U>();

		// cvは必ず同じになる
		return MakeTypeId<copy_cv_to_t<T,U>>();
	}

	template <typename U>
	void iSetTypeId() noexcept
	{
		m_typeId = iMakeTypeId<U>();
	}
	
	// upcast or same type cast
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
		// cv修飾の変換可能チェックはTryCast側で済ましている
#if 0
		// Uが、保持している形と一致しているか判定
		// Tがconst (m_typeIdはconst)
		if constexpr (IsConst)
		{
			// Uはconstのみ
			static_assert(std::is_const_v<U>);
			if(MakeTypeId<U>() == m_typeId)
			{
				// T*をU*にキャストする
				return static_cast<U*>(m_ptr.get());
			}
		}
		// Tが非const (m_typeIdは非const)
		else
		{
			// Uはconst or 非const
			if(MakeTypeId<std::remove_const_t<U>>() == m_typeId)
			{
				// T*をU*にキャストする
				return static_cast<U*>(m_ptr.get());
			}
		}
#endif

		// UもしくはUから派生したクラスを保持しているか
		if(m_typeId.info().IsDerivedFrom<U>())
		{
			// T*をU*にキャストする
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
constexpr bool operator ==(const SubPtr<T, Holder>& x, std::nullptr_t)
	{ return x.get() == nullptr; }

/// 三方比較 (nullptr) <=>
template <typename T, template <class> typename Holder>
constexpr bool operator <=>(const SubPtr<T, Holder>&x, std::nullptr_t)
	{ return x.get() <=> nullptr; }

//------------------------------------------------------------------------------

} // tofu
