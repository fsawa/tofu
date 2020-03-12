//------------------------------------------------------------------------------
/**
 * @file    Nearly.hpp
 * @brief   許容誤差判定
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu.hpp>

namespace tofu {
namespace math {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class NearlyChecker
{
	using self_type = NearlyChecker;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	using value_type = T;
	
	static constexpr T kDefaultTorerance = static_cast<T>(0.0001); ///< デフォルト許容誤差
	
// FUNCTION
	
	/// constructor
	NearlyChecker() noexcept
		: m_value( static_cast<T>(0.0) )
		, m_torerance( kDefaultTorerance )
	{
	}
	
	explicit NearlyChecker( T torerance ) noexcept
		: m_value( static_cast<T>(0.0) )
		, m_torerance(torerance)
	{
	}
	
	/// 値の設定
	self_type&  set( T value )
	{
		m_value = value;
		return *this;
	}
	
	/// 値を設定した別オブジェクトを作成する
	self_type  make( T value ) const
	{
		return NearlyChecker(m_torerance).set(value);
	}
	
	/// 許容最小値取得
	T  min() const { return m_value - m_torerance; }
	
	/// 許容最大値取得
	T  max() const { return m_value + m_torerance; }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
// VARIABLE
	
	value_type m_value;
	value_type m_torerance;
};
// << NearlyChecker

/// 
template <typename T>
bool operator==( T value, NearlyChecker<T> n )
{
	return n.min() <= value && value <= n.max();
}

/// 
template <typename T>
bool operator==( NearlyChecker<T> n, T value )
{
	return n.min() <= value && value <= n.max();
}

/// 
template <typename T>
bool operator!=( T value, NearlyChecker<T> n )
{
	return !(value == n);
}

/// 
template <typename T>
bool operator!=( NearlyChecker<T> n, T value )
{
	return !(n == value);
}


/// 誤差許容判定オブジェクト作成
template <typename T>
NearlyChecker<T> Nearly( T value )
{
	return NearlyChecker<T>().set(value);
}

/// 誤差許容判定オブジェクト作成
template <typename T>
NearlyChecker<T> Nearly( T value, T torerance )
{
	return NearlyChecker<T>( torerance ).set(value);
}

} // math
} // tofu
