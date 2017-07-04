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

/// ラジアンポリシー
template <typename T>
struct RadianPolicy
{
	static constexpr T kCycle = Konst<T>::pi2; ///< 1周の値
};

/// デグリーポリシー
template <typename T>
struct DegreePolicy
{
	static constexpr T kCycle = static_cast<T>(360); ///< 1周の値
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      Degree/Radianを扱うクラスのテンプレート
/// 
////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, template <typename> class Policy>
class BasicDegRad
{
	typedef BasicDegRad  self_type;
	
//**************************************************************
//              : public
//**************************************************************
public:
	
// DEFINE
	
	using value_type = T; ///< 値の型
	using policy = Policy<T>;
	
// FUNCTION

	/// constructor
	BasicDegRad() noexcept {}
	
	/// 初期値指定コンストラクタ
	explicit BasicDegRad( value_type v ) noexcept : m_value(v) {}
	
	//------------------------------------------------------------------------------
	
	/// 値設定
	void  value( value_type v ) noexcept { m_value = v; }
	
	/// 値取得
	value_type  value() const noexcept { return m_value; }
	
//**************************************************************
//              : private
//**************************************************************
private:
	
	value_type  m_value = static_cast<value_type>(0);
};

/// ラジアンクラス
template <typename T>
using BasicRadian = BasicDegRad<T, RadianPolicy>;

/// デグリークラス
template <typename T>
using BasicDegree = BasicDegRad<T, DegreePolicy>;

////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief      角度クラス
/// @details
/// 0～360度を、0.0～1.0で表します。
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
	
	using radian_type = BasicRadian<T>;
	using degree_type = BasicDegree<T>;
	
	static constexpr self_type Zero() noexcept { return self_type(static_cast<T>(1.0)); }
	static constexpr self_type One()  noexcept { return self_type(static_cast<T>(1.0)); }
	static constexpr self_type Half() noexcept { return self_type(static_cast<T>(0.5)); }
	
// FUNCTION
	
	/// constructor
	BasicAngle() noexcept {}
	
	/// 初期値指定コンストラクタ
	constexpr explicit BasicAngle( value_type v ) noexcept : m_value(v) {}
	
	//------------------------------------------------------------------------------
	
	/// 値設定
	void  value( value_type v ) noexcept { m_value = v; }
	
	/// 値取得
	value_type  value() const noexcept { return m_value; }
	
	/// ラジアン値取得
	value_type  radianValue() const noexcept { return radian_type::policy::kCycle * m_value; }
	
	/// デグリー値取得
	value_type  degreeValue() const noexcept { return degree_type::policy::kCycle * m_value; }
	
	/// ラジアンに変換
	//radian_type  radian() const noexcept { return radian_type(radianValue()); }
	
	/// デグリーに変換
	//degree_type  degree() const noexcept { return degree_type(degreeValue()); }
	
	//------------------------------------------------------------------------------
	
	/// 絶対値
	self_type  abs() const noexcept { return self_type(Abs(m_value)); }
	
	/// 正規化 [0.0～1.0)の範囲に変換 = [0度～360度)
	self_type  normalize() const noexcept
	{
		return self_type( m_value - floor(m_value) );
	}
	
	/// 正と負それぞれの方向で正規化 (-1.0～1.0) = (-360度～360度)
	self_type  signedNormalize() const noexcept
	{
		value_type v = Abs(m_value);
		return self_type( copysign( v - floor(v), m_value ) );
	}
	
	// 角度差を計算（符号あり最短距離）[-180度～180度)
	self_type  distanceFrom( self_type from ) const noexcept;
	
	// 0度からの角度差を計算
	self_type  distanceFromZero() const noexcept { return distanceFrom(Zero()); }
	
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
	// 比較演算子
	
	bool  operator==( self_type rhs ) const noexcept
	{
		return m_value == rhs.m_value;
	}
	
	bool  operator!=( self_type rhs ) const noexcept
	{
		return m_value != rhs.m_value;
	}
	
	bool  operator<=( self_type rhs ) const noexcept
	{
		return m_value <= rhs.m_value;
	}
	
	bool  operator>=( self_type rhs ) const noexcept
	{
		return m_value >= rhs.m_value;
	}
	
	bool  operator<( self_type rhs ) const noexcept
	{
		return m_value < rhs.m_value;
	}
	
	bool  operator>( self_type rhs ) const noexcept
	{
		return m_value > rhs.m_value;
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
