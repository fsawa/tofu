//------------------------------------------------------------------------------
/**
 * @file    tofu_Reflection.h
 * @brief   リフレクション
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>
#include <tofu_TypeInfo.h>
#include <tofu_AnyPtr.h>
#include <tofu_AnyBasePtr.h>
#include <tofu_Singleton.h>

namespace tofu {
namespace reflection {

using InstancePtr = AnyPtr<std::shared_ptr>;
	
namespace detail
{
	class ClassCreator
	{
	public:
		virtual TypeId GetTypeId() const = 0;
		virtual InstancePtr Create() const = 0;
	};

	template <typename T>
	class ClassCreatorOf : public ClassCreator, public Singleton<ClassCreatorOf<T>>
	{
	public:
		ClassCreatorOf()
		{
			EntryClass(*this);
		}
		
		TypeId GetTypeId() const override
		{
			return MakeTypeId<T>();
		}

		InstancePtr Create() const override
		{
			return InstancePtr(new T);
		}
	};

	// 型名から生成できるクラスを登録する
	void EntryClass(const ClassCreator& creator);
}

/// @brief 型名から生成できるクラスを登録する
template <class T>
inline void EntryClass()
{
	detail::ClassCreatorOf<T>::CreateInstance();
}

// 型名から生成できるクラスを登録するマクロ
#define TOFU_REFLECTION_CLASS(type)  TOFU_STATIC_CALL(::tofu::reflection::detail::ClassCreatorOf<type>::CreateInstance)

/// @brief 型名からクラスインスタンスを生成する
InstancePtr Create(std::string_view typeName);

/// @brief 型名から派生クラスのshared_ptrを作る
/// @tparam T 基底クラス
/// @param typeName 生成するクラス名
/// @return 生成したクラスを基底クラスのshared_ptrとして取得
#if 0
template <class T>
inline std::shared_ptr<T> CreateInstance(std::string_view typeName)
{
}
#endif

} // reflection
} // tofu
