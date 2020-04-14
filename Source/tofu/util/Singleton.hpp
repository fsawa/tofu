//------------------------------------------------------------------------------
/**
 * @file    Singleton.hpp
 * @brief   シングルトン関連
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>
#include <tofu/util/DisallowCopy.hpp>
#include <tofu/util/SingletonUtil.hpp>
#include <tofu/util/SingletonPolicy.hpp>

// - Singleton
// - SingletonHolder

namespace tofu {

/// シングルトン関連
namespace singleton {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      シングルトン機能のMixinクラス
/// @tparam  T            : シングルトンにしたいクラス
/// @tparam  CreatePolicy : 生成を制御するポリシークラス
/// @details
/// ex. class Random : public tofu::Singleton<Random, tofu::singleton::StaticCreator><br>
/// 
/// CreatePolicy には、<br>
///   Instance()<br>
///   CreateInstance()<br>
///   ExistInstance()<br>
///   DestroyInstance()<br>
/// の関数を持つクラスを指定する。<br>
/// <br>
/// 派生クラス(=T)では、CreatePolicyがインスタンス生成できるように<br>
///   friend class MyCreatePolicy;<br>
/// を記述する必要がある。
/// <br>
/// シンプルなアクセス方法<br>
///   Random::Instance().Func();<br>
/// グローバルなポインタ変数風にアクセスしたい場合<br>
///   extern Random::SingletonAccessor gRandom;<br>
///   gRandom->Func();<br>
////////////////////////////////////////////////////////////////////////////////////////////////
template <
	typename T, 
	template <typename> class CreatePolicy_ = StaticCreator
>
class Singleton
{
	//typedef Singleton  self_type;
	using self_type = Singleton;
	
	// コピー禁止
	TOFU_DISALLOW_COPY(Singleton);
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	/// 生成ポリシー
	using MyCreatePolicy = CreatePolicy_<T>;
	
	/// アクセスクラス
	using SingletonAccessor = SingletonAccessor<T, self_type>;
	
// FUNCTION
	
	/// インスタンス取得（参照）
	static T&    Instance()         { return GetCreatePolicy().Instance(); }
	
	/// インスタンス取得（ポインタ）
	static T*    GetInstance()      { return &GetCreatePolicy().Instance(); }
	
	/// インスタンス生成
	static T*    CreateInstance()   { return GetCreatePolicy().CreateInstance(); }
	
	/// インスタンス存在チェック
	static bool  ExistInstance()    { return GetCreatePolicy().ExistInstance(); }
	
	/// インスタンス破壊
	static void  DestroyInstance()  { (void)GetCreatePolicy().DestroyInstance(); }
	
//**************************************************************
//              : protected
//**************************************************************
protected:
	
	Singleton()
	{
		m_pSelfAsSingleton = this; // デバッグ用途
	}
	
	/*virtual*/ ~Singleton()
	{
		TOFU_ASSERT(m_pSelfAsSingleton); // ポインタnullはおかしい
		m_pSelfAsSingleton = nullptr;
	}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
	/// 生成ポリシーの実体
	static inline MyCreatePolicy&  GetCreatePolicy()
	{
		static MyCreatePolicy  policy;
		return policy;
	}
	
// VARIABLE
	
	self_type*  m_pSelfAsSingleton = nullptr;
};
// << Singleton

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      既存のクラスをシングルトンとして保持するクラス
/// @tparam  T            : シングルトンにしたいクラス
/// @tparam  CreatePolicy : 生成を制御するポリシークラス
/// @tparam  Tag          : 同一のクラスで別シングルトンを作りたい場合のユニークタグ
/// @details
/// ex.
/// @code
/// class Random {...};
/// using GlobalRandom = tofu::SingletonHolder<Random>;
/// GlobalRandom::CreateInstance();
/// GlobalRandom::Instance().getU32();
/// 
/// // 同じクラスに対して、別のSingletonHolderを定義、生成する
/// struct PlayerRandomTag{};
/// tofu::SingletonHolder<Random, PlayerRandomTag>  PlayerRandom;
/// PlayerRandom::CreateInstance();
/// PlayerRandom::Instance().getU32();
/// @endcode
////////////////////////////////////////////////////////////////////////////////////////////////
template <
	typename T, 
	template <typename> class CreatePolicy = StaticCreator, 
	typename Tag = void
>
class SingletonHolder : public Singleton<T, CreatePolicy>
{
private:
	// このクラス自体のインスタンスは生成できないように
	SingletonHolder() = delete;
	~SingletonHolder() = delete;

public:
	
	using tag_type = Tag; ///< タグの型
};
// << SingletonHolder

} // singleton

// 上の名前空間で使えるようにする
using singleton::Singleton;
using singleton::SingletonHolder;

} // tofu
