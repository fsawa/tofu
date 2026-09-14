//------------------------------------------------------------------------------
/**
 * @file   tofu_TypeInfo.h
 * @brief  型情報クラスと型IDクラス
 * 
 * @author      y.fujisawa
 * @par         copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.h>

#include <type_traits>
#include <string>
#include <string_view>

#include <tofu_TypeTraits.h>
#include <tofu_TypeName.h>
//#include <detail/tofu_BaseClassInfo.h>

namespace tofu {

// 前方宣言
namespace rtti_detail { class BaseClassInfo; }

class TypeInfo;

template <typename T>
requires (!std::is_const_v<T> && !std::is_volatile_v<T> && !std::is_reference_v<T>)
class TypeInfoOf;

////////////////////////////////////////////////////////////////////////////////
/// @brief      型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeInfo
{
	using self_type = TypeInfo;

	friend class rtti_detail::BaseClassInfo;
	using BaseClassInfo = rtti_detail::BaseClassInfo;

public:
	
	/// 型名取得
	const char* GetNamePtr() const noexcept { return m_Name.data(); }
	
	/// 型名取得
	std::string_view GetName() const noexcept { return m_Name; }
	
	//------------------------------------------------------------------------------
	
	/// 比較 ==
	bool operator==( const TypeInfo& rhs ) const noexcept
		{ return this == &rhs; }
	
	/// 三方比較 <=>
	constexpr auto operator <=>(const TypeInfo& rhs) const noexcept
		{ return this <=> &rhs; }
	
	//------------------------------------------------------------------------------

	// 対象型のvoid*を、BaseType*へアップキャストを試みる
	template <typename BaseT>
	BaseT* TryUpcast(copy_cv_to_t<BaseT, void>* p) const noexcept
	{
		using base_type_no_cv = std::remove_cv<BaseT>::type;
		auto result = TryUpcast(p, TypeInfoOf<base_type_no_cv>::Instance());
		return static_cast<BaseT*>(result);
	}
	
	// 対象型のvoid*を、指定のTypeInfoの型へアップキャストする
	void* TryUpcast(void* p, const TypeInfo& target_type_info) const noexcept;
	const void* TryUpcast(const void* p, const TypeInfo& target_type_info) const noexcept;

	// 型がTと同じか
	template <typename T>
	bool IsSame() const noexcept
	{
		const TypeInfo* const target_info = &TypeInfoOf<T>::Instance();
		if(target_info == this){
			return true;
		}
		return false;
	}

	// base_infoから派生しているかどうか
	bool IsDerivedFrom(const TypeInfo& base_info) const noexcept;

	// BaseTから派生しているかどうか
	template <typename BaseT>
	bool IsDerivedFrom() const noexcept
	{
		using base_t = std::remove_cv_t<BaseT>;
		const auto& base_info = TypeInfoOf<base_t>::Instance();
		return IsDerivedFrom(base_info);
	}
	
protected:

	TypeInfo() = delete;
	explicit TypeInfo(std::string_view name) noexcept : m_Name{name} {}
	virtual ~TypeInfo() = default;

private:
	const BaseClassInfo* AddBaseInfo(const BaseClassInfo& info);
	
private:
	const std::string_view m_Name;
	
	const BaseClassInfo* m_BaseClassInfo = nullptr; // 基底クラス情報
};
// << TypeInfo

////////////////////////////////////////////////////////////////////////////////
/// @brief      型毎の型情報クラス
/// 
////////////////////////////////////////////////////////////////////////////////
template <typename T>
requires (!std::is_const_v<T> && !std::is_volatile_v<T> && !std::is_reference_v<T>)
class TypeInfoOf final : public TypeInfo
{
	using self_type = TypeInfoOf;
	
public:
	
	using type = T;
	
public:

	static constexpr auto Name = TypeName<T>::Value;
	
	/// インスタンス取得
	static self_type& Instance() noexcept
	{
		static self_type sInstance;
		return sInstance;
	}
	
	//------------------------------------------------------------------------------
	
	TypeInfoOf() noexcept
		: TypeInfo(Name.view())
	{
	}
	virtual ~TypeInfoOf() = default;
	
};
// << TypeInfoOf

//------------------------------------------------------------------------------

/// TypeInfo取得
template <typename T>
inline TypeInfoOf<std::remove_cvref_t<T>>& GetTypeInfo() noexcept
{
	return TypeInfoOf<std::remove_cvref_t<T>>::Instance();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief      型ID
/// 
////////////////////////////////////////////////////////////////////////////////
class TypeId final
{
	// ポインタの下位2bitにconst,volatileの情報を埋め込む
	static constexpr uintptr_t ConstBit    = 0b01;
	static constexpr uintptr_t VolatileBit = 0b10;
	static constexpr uintptr_t CVMask      = 0b11;
	
	template <typename T>
	static consteval uintptr_t MakeCVBits()
	{
		uintptr_t value = 0;
		if constexpr (std::is_const_v<T>) value |= ConstBit;
		if constexpr (std::is_volatile_v<T>) value |= VolatileBit;
		return value;
	}

	//explicit constexpr TypeId( const TypeInfo* Info ) noexcept : m_type_info(Info) {}
	
	explicit constexpr TypeId( uintptr_t info_value ) noexcept
		: m_data(info_value)
	{}

public:
	
	/// TypeId作成
	template <typename T>
	static TypeId Make() noexcept
	{
		auto value = reinterpret_cast<uintptr_t>(&GetTypeInfo<T>()) | MakeCVBits<T>();
		return TypeId( value );
	};

public:
	
	constexpr TypeId() noexcept : m_type_info(nullptr) {}
	
	/// 空かどうか
	constexpr bool IsEmpty() const noexcept { return m_data <= CVMask; }
	
	/// クリア
	constexpr void Clear() noexcept
	{
		m_type_info = nullptr;
	}
	
	/// TypeInfo参照
	const TypeInfo& info() const { TOFU_ASSERT(GetInfoPtr() != nullptr); return *GetInfoPtr(); }
	
	/// TypeInfo取得
	const TypeInfo* GetInfoPtr() const noexcept
	{
		return reinterpret_cast<const TypeInfo*>(m_data & ~CVMask);
	}
	
	/// const修飾ありか
	constexpr bool IsConst() const noexcept
	{
		return (m_data & ConstBit) != 0;
	}

	/// volatile修飾ありか
	constexpr bool IsVolatile() const noexcept
	{
		return (m_data & VolatileBit) != 0;
	}

	/// const修飾ありのTypeId取得
	constexpr TypeId ToConst() const noexcept
	{
		return TypeId{ m_data | ConstBit };
	}
	
	/// const修飾なしのTypeId取得
	constexpr TypeId ToNotConst() const noexcept
	{
		return TypeId{ m_data & ~ConstBit };
	}

	/// cv修飾なしのTypeId取得
	constexpr TypeId ToUnqualified() const noexcept
	{
		return TypeId{ m_data & ~CVMask };
	}
	
	// 修飾なしの型自体がTかどうか
	template <typename T>
	constexpr bool RawTypeIs() const noexcept
	{
		return Make<std::remove_cvref_t<T>>().GetInfoPtr() == GetInfoPtr();
	}

	/// 比較 ==
	constexpr bool operator ==(const TypeId& rhs) const noexcept
		{ return m_type_info == rhs.m_type_info; }

	/// 三方比較 <=>
	constexpr auto operator <=>(const TypeId& rhs) const noexcept
		{ return m_type_info <=> rhs.m_type_info; }

private:
	union
	{
		const TypeInfo* m_type_info;
		uintptr_t m_data;
	};
};

//------------------------------------------------------------------------------

/// TypeId作成
template <typename T>
constexpr TypeId MakeTypeId() noexcept
{
	return TypeId::Make<T>();
};

/// 変数からTypeId作成
template <typename T>
constexpr TypeId MakeTypeId( T& ) noexcept
{
	return TypeId::Make<T>();
};

} // tofu
