//------------------------------------------------------------------------------
/**
 * @file    Finalizer.h
 * @brief   インスタンスの開放や終了処理を一括で実行する為のクラス
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>
#include <tofu_DisallowCopy.h>

#include <list>
#include <functional>

namespace tofu {

//------------------------------------------------------------------------------
/// @brief  インスタンスの開放や終了処理を一括で実行する為のクラス
/// @note
/// 登録と逆順で処理を行う。
class Finalizer final
{
	using SelfType = Finalizer;
	
	// コピー禁止
	TOFU_DISALLOW_COPY(Finalizer);
	
	using FunctionType = std::move_only_function<void ()>;

public:
	
	Finalizer() = default;
	
	~Finalizer()
	{
		Finalize();
	}

	/// 実行する関数オブジェクトの登録
	void PushFunctor( FunctionType func )
	{
		// 登録と逆順で処理しやすいように前に追加
		m_list.push_front(std::move(func));
	}
	
	/// 何かのインスタンスを登録
	template <class T>
	void Push( T&& inst )
	{
		// ラムダ式を使って関数オブジェクトに保持する
		PushFunctor( [x = std::forward<T>(inst)] {} );
	}
	
	/// 何かのインスタンスを登録
	template <class T>
	Finalizer& operator+=( T&& inst )
	{
		// ラムダ式を使って関数オブジェクトに保持する
		PushFunctor( [x = std::forward<T>(inst)] {} );
		return *this;
	}
	
	/// 終了処理を実行
	void Finalize()
	{
		for (auto& func : m_list)
		{
			func(); // 関数オブジェクトだったら呼び出し
			func = {}; // インスタンス解放
		}
		m_list.clear();
	}
	
private:
	std::list<FunctionType> m_list;
};
// << Finalizer

} // tofu
