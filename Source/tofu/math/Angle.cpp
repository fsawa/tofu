//------------------------------------------------------------------------------
/**
 * @file    Angle.cpp
 * @brief   角度を扱う機能
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2017 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <tofu/math/Angle.hpp>

#include <type_traits>

namespace tofu {
namespace math {

namespace detail
{
	//------------------------------------------------------------------------------
	#if 0
	template <typename T, template <typename> class Policy>
	bool  BasicAngles<T, Policy>::nearlyEqual( self_type target, value_type range ) const noexcept
	{
		auto diff = Abs(target.value() - value());
		return diff <= range;
	}
	#endif
	
	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	BasicAngles<T, Policy>  BasicAngles<T, Policy>::normalize() const noexcept
	{
		if( m_value >= Zero().value() && m_value < One().value() ){
			return *this;
		}
		
		// if constexpr にしたい
		if( std::is_same<policy, AnglePolicy<T>>::value )
		{
			return self_type( m_value - floor(m_value) );
		}
		else
		{
			return angle().normalize();
		}
	}

	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	BasicAngles<T, Policy>  BasicAngles<T, Policy>::signedNormalize() const noexcept
	{
		if( m_value > -One().value() && m_value < One().value() ){
			return *this;
		}
		
		// if constexpr にしたい
		if( std::is_same<policy, AnglePolicy<T>>::value )
		{
			value_type v = Abs(m_value);
			return self_type( copysign( v - floor(v), m_value ) );
		}
		else
		{
			return angle().signedNormalize();
		}
	}

	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	BasicAngles<T, Policy>  BasicAngles<T, Policy>::distanceFrom( self_type from ) const noexcept
	{
		auto diff = *this - from;
		// 0.0～1.0に正規化
		diff = diff.normalize();
		// -0.5～0.5に変換
		if( diff >= Half() ){
			diff -= One();
		}
		return diff;
	}
	
	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	BasicAngles<T, Policy>  BasicAngles<T, Policy>::reflect( self_type axis ) const noexcept
	{
		return *this + axis.distanceFrom( *this ) * static_cast<value_type>(2);
	}
	
	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	T  BasicAngles<T, Policy>::sin() const
	{
		return std::sin( radian().value() );
	}
	
	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	T  BasicAngles<T, Policy>::cos() const
	{
		return std::cos( radian().value() );
	}
	
	//------------------------------------------------------------------------------
	template <typename T, template <typename> class Policy>
	T  BasicAngles<T, Policy>::tan() const
	{
		return std::tan( radian().value() );
	}

	//------------------------------------------------------------------------------

	// インスタンス化

	template class BasicAngles<float, AnglePolicy>;
	template class BasicAngles<float, RadianPolicy>;
	template class BasicAngles<float, DegreePolicy>;

	template class BasicAngles<double, AnglePolicy>;
	template class BasicAngles<double, RadianPolicy>;
	template class BasicAngles<double, DegreePolicy>;

	template class BasicAngles<long double, AnglePolicy>;
	template class BasicAngles<long double, RadianPolicy>;
	template class BasicAngles<long double, DegreePolicy>;

} // detail

} // math
} // tofu
