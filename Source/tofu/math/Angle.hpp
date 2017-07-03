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
/// @brief      角度クラス
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
	
	/// ラジアン値取得
	value_type  radian() const noexcept { return Konst<T>::pi2 * m_value; }
	
	/// 弧度値取得
	value_type  degree() const noexcept { return 360 * m_value; }
	
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
	
	/// 正規化 [0度～360度)の範囲に変換
	self_type  normalize() const noexcept
	{
		return self_type( m_value - floor(m_value) );
	}
	
	/// 負方向も含めた正規化 (-360度～360度)の範囲に変換
	self_type  signedNormalize() const noexcept
	{
		value_type v = Abs(m_value);
		return self_type( copysign( v - floor(v), m_value ) );
	}
	
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
