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
#include <tofu_Singleton.h>

namespace tofu {

// 動的リフレクション
namespace reflection {

// 動的生成したインスタンスを保持するポインター
using InstancePtr = AnyPtr<std::shared_ptr>;

// クラスの動的リフレクションクラス
class ClassReflection
{
protected:
	ClassReflection() = default;

public:

	// 型ID取得
	virtual TypeId GetTypeId() const noexcept = 0;

	// 型情報取得
	const TypeInfo& GetTypeInfo() const noexcept
	{
		return GetTypeId().info();
	}

	// インスタンス生成
	virtual InstancePtr Create() const noexcept = 0;
};
	
namespace detail
{
	template <typename T>
	class ClassReflectionOf : public ClassReflection, public Singleton<ClassReflectionOf<T>>
	{
	public:
		ClassReflectionOf() noexcept
		{
			EntryClass(*this);
		}
		
		TypeId GetTypeId() const noexcept override
		{
			return MakeTypeId<T>();
		}

		InstancePtr Create() const noexcept override
		{
			return InstancePtr(new T);
		}
	};

	// 型名から生成できるクラスを登録する
	void EntryClass(const ClassReflection& creator);
}

/// 型名から生成できるクラスを登録する
template <class T>
inline void EntryClass()
{
	detail::ClassReflectionOf<T>::CreateInstance();
}

/// 型名から生成できるクラスを登録するマクロ
#define TOFU_REFLECTION_CLASS(type)  TOFU_STATIC_CALL(::tofu::reflection::detail::ClassReflectionOf<type>::CreateInstance)

/// 型名からクラスリフレクションクラスを取得する
SafePtr<const ClassReflection> FindClassReflection(std::string_view name) noexcept;

/// 型名からクラスインスタンスを生成する
InstancePtr Create(std::string_view type_name);

} // reflection
} // tofu
