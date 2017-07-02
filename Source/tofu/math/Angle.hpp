//------------------------------------------------------------------------------
/**
 * @file    Angle.hpp
 * @brief   角度を扱う機能
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#pragma once

#include <tofu/math/MathCommon.hpp>

namespace tofu {
namespace math {

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      型クラス
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class BasicAngle
{
	typedef BasicAngle  self_type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	using value_type = T; ///< 値の型
	
// FUNCTION
	
	/// constructor
	BasicAngle() noexcept {}
	
	/// 初期値指定コンストラクタ
	explicit BasicAngle( value_type v ) noexcept : m_value(v) {}
	
	//------------------------------------------------------------------------------
	
	/// 値設定
	void  value( value_type v ) noexcept { m_value = v; }
	
	/// 値取得
	value_type  value() const noexcept { return m_value; }
	
	//------------------------------------------------------------------------------
	// 四則演算
	
	/// 加算
	self_type&  operator+=( self_type rhs ) noexcept
	{
		m_value += rhs.value();
		return *this;
	}
	
	/// 減算
	self_type&  operator-=( self_type rhs ) noexcept
	{
		m_value -= rhs.value();
		return *this;
	}
	
	/// 乗算
	self_type&  operator*=( value_type v ) noexcept
	{
		m_value *= v;
		return *this;
	}
	
	/// 除算
	self_type&  operator/=( value_type v ) noexcept
	{
		m_value /= v;
		return *this;
	}
	
	/// 剰余
	self_type&  operator%=( value_type v ) noexcept
	{
		m_value = Mod( m_value, v );
		return *this;
	}
	
	//------------------------------------------------------------------------------
	
	/// 加算
	self_type  operator+( self_type rhs ) const noexcept
	{
		return self_type(m_value + rhs.value());
	}
	
	/// 減算
	self_type  operator-( self_type rhs ) const noexcept
	{
		return self_type(m_value - rhs.value());
	}
	
	/// 乗算
	self_type  operator*( value_type v ) const noexcept
	{
		return self_type(m_value * v);
	}
	
	/// 除算
	self_type  operator/( value_type v ) const noexcept
	{
		return self_type(m_value / v);
	}
	
	/// 剰余
	self_type  operator%( value_type v ) const noexcept
	{
		return self_type(Mod( m_value, v ));
	}
	
	//------------------------------------------------------------------------------
	
	/// 絶対値
	self_type  abs() const noexcept { return self_type(Abs(m_value)); }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
// DEFINE
	
// FUNCTION
	
// VARIABLE
	
	value_type  m_value = static_cast<value_type>(0);
};
// << BasicAngle

/// 標準の型クラス
using Angle = BasicAngle<float>;

} // math
} // tofu
