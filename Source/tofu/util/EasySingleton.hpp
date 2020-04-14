//------------------------------------------------------------------------------
/**
 * @file    EasySingleton.hpp
 * @brief   インスタンスの生成方法に関与しない、シングルトンMixin
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>
#include <tofu/util/DisallowCopy.hpp>
#include <tofu/util/SingletonUtil.hpp>

namespace tofu {
namespace singleton {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      インスタンスの生成方法に関与しない、シングルトンMixinクラス
/// @tparam  T : シングルトンにしたいクラス
/// @details
/// インスタンスの生成方法は提供せず、ユーザーコード側に任せる。<br>
/// メンバ変数でも、static変数でも、newしても、どの方法でも良い。<br>
/// このクラスは各種関数と、動的な２重生成エラーチェックのみ行う。
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class EasySingleton
{
	typedef EasySingleton  self_type;
	
	// コピー禁止
	TOFU_DISALLOW_COPY(EasySingleton);
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	/// アクセスクラス
	using SingletonAccessor = SingletonAccessor<T, self_type>;
	
// FUNCTION
	
	/// インスタンス参照
	static T&    Instance()    { TOFU_ASSERT(s_pInstance); return *s_pInstance; }
	
	/// インスタンス参照
	static T*    GetInstance()    { return &Instance(); }
	
	/// インスタンス生成（はしないが、Singletonとインターフェースを合わせたい場合用に定義）
	static T*    CreateInstance()   { return GetInstance(); }
	
	/// インスタンス存在チェック
	static bool  ExistInstance()  { return nullptr != s_pInstance; }
	
	/// インスタンス破壊（はしないが、Singletonとインターフェースを合わせたい場合用に定義）
	static void  DestroyInstance()  {}
	
//**************************************************************
//              : protected
//**************************************************************
protected:
	
	EasySingleton()
	{
		TOFU_ASSERT( nullptr == s_pInstance );
		s_pInstance = static_cast<T*>(this);
	}
	
	/*virtual*/ ~EasySingleton()
	{
		TOFU_ASSERT( nullptr != s_pInstance );
		s_pInstance = nullptr;
	}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// VARIABLE
	
	static T*  s_pInstance;
};
// << EasySingleton

// ポインタ
template <typename T>
T*  EasySingleton<T>::s_pInstance = nullptr;


////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      既存のクラスをシングルトンとして保持するタイプのEasySingleton
/// @tparam  T   : シングルトンにしたいクラス
/// @tparam  Tag : 同一のクラスで別シングルトンを作りたい場合のユニークタグ
/// @details
/// ex.
/// @code
/// class Random {...};
/// 
/// using GlobalRandom = tofu::EasySingletonHolder<Random>;
/// new GlobalRandom();
/// GlobalRandom::Instance().getU32();
/// 
/// // 同じRandomに対して、別のSingletonLikeを定義／生成できる
/// struct SubGlobalRandomTag{};
/// using SubGlobalRandom = tofu::EasySingletonHolder<Random, SubGlobalRandomTag>;
/// new SubGlobalRandom();
/// SubGlobalRandom::Instance().getU32();
/// @endcode
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, typename Tag = void>
class EasySingletonHolder
{
	typedef EasySingletonHolder  self_type;
	
	// コピー禁止
	TOFU_DISALLOW_COPY(EasySingletonHolder);
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	/// アクセスクラス
	using SingletonAccessor = SingletonAccessor<T, self_type>;
	
	/// ユニークタグ
	using tag_type = Tag;
	
// FUNCTION
	
	/// インスタンス参照
	static T&    Instance()    { TOFU_ASSERT(s_pInstance); return s_pInstance->content; }
	
	/// インスタンス参照
	static T*    GetInstance()    { return &Instance(); }
	
	/// インスタンス生成（はしないが、Singletonとインターフェースを合わせたい場合用に定義）
	static T*    CreateInstance()   { return GetInstance(); }
	
	/// インスタンス存在チェック
	static bool  ExistInstance()  { return nullptr != s_pInstance; }
	
	/// インスタンス破壊（はしないが、Singletonとインターフェースを合わせたい場合用に定義）
	static void  DestroyInstance()  {}
	
	EasySingletonHolder()
	{
		TOFU_ASSERT( nullptr == s_pInstance );
		s_pInstance = this;
	}
	
	/*virtual*/ ~EasySingletonHolder()
	{
		TOFU_ASSERT( nullptr != s_pInstance );
		s_pInstance = nullptr;
	}

// VARIABLE
	
	T  content; ///< 目的のインスタンス ( public )
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// VARIABLE
	
	static self_type*  s_pInstance; ///< 2重生成チェック用ポインタ
};
// << EasySingletonHolder

// ポインタ
template <typename T, typename Tag>
EasySingletonHolder<T, Tag>*  EasySingletonHolder<T, Tag>::s_pInstance = nullptr;

} // singleton

using singleton::EasySingleton;
using singleton::EasySingletonHolder;

} // tofu
