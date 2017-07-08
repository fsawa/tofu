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
	template <typename T, template <typename> class Policy>
	BasicAngles<T, Policy>  BasicAngles<T, Policy>::normalize() const noexcept
	{
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
		// if constexpr にしたい
		if( std::is_same<policy, AnglePolicy<T>>::value )
		{
			self_type diff = *this - from;
			// 0.0～1.0に正規化
			diff = diff.normalize();
			// -0.5～0.5に変換
			if( diff >= Half() ){
				diff -= One();
			}
			return diff;
		}
		else
		{
			return angle().distanceFrom( from.angle() );
		}
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
