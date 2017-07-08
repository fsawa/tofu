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

namespace detail
{
	/// 正規化角度ポリシー
	template <typename T>
	struct AnglePolicy
	{
		static constexpr T kCycle = static_cast<T>(1.0); ///< 1周の値
	};
	
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
	/// @brief      角度クラス
	/// 
	////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T, template <typename> class Policy>
	class BasicAngles
	{
		using self_type = BasicAngles;
		
	//**************************************************************
	//              : public
	//**************************************************************
	public:
		
	// DEFINE
		
		using value_type = T; ///< 値の型
		using policy = Policy<T>; ///< ポリシー
		
		using radian_type = BasicAngles<T, RadianPolicy>;
		using degree_type = BasicAngles<T, DegreePolicy>;
		using angle_type  = BasicAngles<T, AnglePolicy>;
		
		static constexpr self_type Zero() noexcept { return self_type(static_cast<T>(0.0)); }
		static constexpr self_type One()  noexcept { return self_type(policy::kCycle); }
		static constexpr self_type Half() noexcept { return self_type(static_cast<T>(policy::kCycle*0.5)); }
		
	// FUNCTION

		/// constructor
		BasicAngles() noexcept {}
		
		/// 初期値指定コンストラクタ
		constexpr explicit BasicAngles( value_type v ) noexcept : m_value(v) {}
		
		/// 別ポリシーのクラスからのコピーコンストラクタ
		template <template <typename> class P2>
		constexpr BasicAngles( BasicAngles<T,P2> rhs ) noexcept
			: m_value(rhs.value() * (policy::kCycle / BasicAngles<T,P2>::policy::kCycle))
		{
		}
		
		/// 別ポリシーのクラスから代入
		template <template <typename> class P2>
		self_type&  operator=( BasicAngles<T,P2> rhs ) noexcept
		{
			m_value = rhs.value() * (policy::kCycle / BasicAngles<T,P2>::policy::kCycle);
			return *this;
		}
		
		//------------------------------------------------------------------------------
		
		/// 値設定
		void  value( value_type v ) noexcept { m_value = v; }
		
		/// 値取得
		constexpr value_type  value() const noexcept { return m_value; }
		
		/// 正規化角度値（0.0～1.0変換）取得
		constexpr value_type  angleValue() const noexcept { return m_value / policy::kCycle; }
		
		/// ラジアン値取得
		constexpr value_type  radianValue() const noexcept { return radian_type::policy::kCycle * m_value; }
		
		/// デグリー値取得
		constexpr value_type  degreeValue() const noexcept { return degree_type::policy::kCycle * m_value; }
		
		angle_type   angle()  const noexcept { return *this; }
		radian_type  radian() const noexcept { return *this; }
		degree_type  degree() const noexcept { return *this; }
		
		//------------------------------------------------------------------------------
		
		/// 絶対値
		self_type  abs() const noexcept { return self_type(Abs(m_value)); }
		
		/// 正規化 [0.0～1.0)の範囲に変換 = [0度～360度)
		self_type  normalize() const noexcept;
		
		/// 正と負それぞれの方向で正規化 (-1.0～1.0) = (-360度～360度)
		self_type  signedNormalize() const noexcept;
		
		/// 角度差を計算（符号あり最短距離）[-180度～180度)
		self_type  distanceFrom( self_type from ) const noexcept;
		
		/// 0度からの角度差を計算
		self_type  distanceFromZero() const noexcept { return distanceFrom(Zero()); }
		
		/// 指定した角度を中心に反転させた角度を計算する
		//self_type  reflection( self_type axis ) const noexcept;
		
		//------------------------------------------------------------------------------
		
		// sin
		// cos
		// tan
		// cossin (２次元平面でx,yの順になるようにcos,sinの順にしている）
		
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
		
		value_type  m_value = static_cast<value_type>(0);
	};
} // detail

/// 正規化角度クラス
template <typename T>
using BasicAngle = detail::BasicAngles<T, detail::AnglePolicy>;

/// ラジアンクラス
template <typename T>
using BasicRadian = detail::BasicAngles<T, detail::RadianPolicy>;

/// デグリークラス
template <typename T>
using BasicDegree = detail::BasicAngles<T, detail::DegreePolicy>;

//------------------------------------------------------------------------------

/// 標準の正規化角度クラス
using Angle = BasicAngle<float>;

/// 標準のラジアンクラス
using Radian = BasicRadian<float>;

/// 標準のデグリークラス
using Degree = BasicDegree<float>;

//------------------------------------------------------------------------------

/// 正規化角度インスタンス作成
template <typename T>
constexpr BasicAngle<T>  MakeAngle( T value ) noexcept { return BasicAngle<T>(value); }

/// ラジアンインスタンス作成
template <typename T>
constexpr BasicRadian<T>  MakeRadian( T value ) noexcept { return BasicRadian<T>(value); }

/// デグリーインスタンス作成
template <typename T>
constexpr BasicDegree<T>  MakeDegree( T value ) noexcept { return BasicDegree<T>(value); }

} // math
} // tofu
