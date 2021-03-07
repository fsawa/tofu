//------------------------------------------------------------------------------
/**
 * @file    Finalizer.hpp
 * @brief   インスタンスの開放や終了処理を一括で実行する為のクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>
#include <tofu/util/DisallowCopy.hpp>

#include <list>
#include <functional>

namespace tofu {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      インスタンスの開放や終了処理を一括で実行する為のクラス
/// 
/// @note
/// 登録と逆順で処理を行う。
////////////////////////////////////////////////////////////////////////////////////////////////
class Finalizer
{
	typedef Finalizer  self_type;
	
	// コピー禁止
	TOFU_DISALLOW_COPY(Finalizer);
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
// FUNCTION
	
	/// constructor
	Finalizer() {}
	
	/// destructor
	/*virtual*/ ~Finalizer()
	{
		finalize();
	}
	
	//==============================
	
	/// deleteするインスタンスの登録
	template <typename T>
	T*  add( T* ptr )
	{
		addFunctor( Deleter<T>(ptr) );
		return ptr;
	}
	
	/// delete[]するインスタンスの登録
	template <typename T>
	T*  addArray( T* ptr )
	{
		addFunctor( ArrayDeleter<T>(ptr) );
		return ptr;
	}
	
	/// 実行する関数オブジェクトの登録
	template <typename Functor>
	void  addFunctor( const Functor& f )
	{
		// 登録と逆順で処理しやすいように前に追加
		m_funcList.push_front(f);
	}
	
	/// 終了処理を実行
	void  finalize()
	{
		for (auto& func : m_funcList)
		{
			func();
		}
		m_funcList.clear();
	}
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
	template <typename T>
	class Deleter
	{
	public:
		explicit Deleter(T* ptr) : m_ptr(ptr) {}
		
		void operator()()
		{
			delete m_ptr;
			m_ptr = nullptr;
		}
	private:
		T* m_ptr;
	};
	
	template <typename T>
	class ArrayDeleter
	{
	public:
		explicit ArrayDeleter(T* ptr) : m_ptr(ptr) {}
		
		void operator()()
		{
			delete[] m_ptr;
			m_ptr = nullptr;
		}
	private:
		T* m_ptr;
	};
	
// FUNCTION
	
// VARIABLE
	
	std::list< std::function<void ()> >  m_funcList;
};
// << Finalizer

} // tofu
