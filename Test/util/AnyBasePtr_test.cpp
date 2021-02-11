//------------------------------------------------------------------------------
/**
 * @file    AnyBasePtr_test.cpp
 * @brief   AnyBasePtr�e�X�g
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2020 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------
#include <tofu/util/AnyBasePtr.hpp>

#include <iostream>
#include <iutest.hpp>

namespace test
{
	class A
	{
	public:
		int a;
		A() : a(0) {}
	};

	class B
	{
	public:
		int b;
		B() : b(0) {}
	};

	class AB : public A, public B
	{
	public:
		int ab;
		AB() : ab(0) {}
	};
}

IUTEST(util, AnyBasePtr)
{
	using namespace test;

	A a;
	AB ab;
	
	{
		// �R���X�g���N�^
		tofu::AnyBasePtr<A>  p1;
		tofu::AnyBasePtr<A>  p2( p1 );
		tofu::AnyBasePtr<A>  p3( nullptr );
		tofu::AnyBasePtr<A>  p4( &a );
		tofu::AnyBasePtr<A>  p5( &ab );
		tofu::AnyBasePtr<A>  p6( p5.get(), p5.type() );
		
		// �R�s�[
		p2 = p1;
		p3 = nullptr;
		p4 = &a;
		p5 = &ab;
		
		//
		p6.assign( p5.get(), p5.type() );
		
		IUTEST_ASSERT_EQ( p5.get(), p6.get() );
		IUTEST_ASSERT_TRUE( p5.type() == p6.type() );
		
		IUTEST_ASSERT_EQ( p6.get(), &ab );
		IUTEST_ASSERT_TRUE( p6.type() == tofu::MakeTypeId<AB>() );
		
		// clear
		p6.clear();
		IUTEST_ASSERT_TRUE( p6.get() == nullptr );
		IUTEST_ASSERT_TRUE( p6.type() == tofu::TypeId() );
		
		//------------------------------------------------------------------------------
		
		// null_assert
		//p1.null_assert(); // ng
		p4.null_assert(); // ok
		
		// �L���X�g
		{
			A* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			AB* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			const A* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		{
			const AB* p = p5;
			IUTEST_ASSERT_EQ( p, &ab );
		}
		
		// bool�L���X�g
		IUTEST_ASSERT_FALSE( p1 );
		IUTEST_ASSERT_TRUE( p5 );
		
		// empty
		IUTEST_ASSERT_TRUE( p1.empty() );
		IUTEST_ASSERT_FALSE( p5.empty() );
		
		// safe
		//p1.safe_get(); // ng
		p4.safe_get(); // ok
		
		// makeAddConst
		tofu::AnyBasePtr<const A>  p10 = p5.makeAddConst();
		p10 = p5.makeAddConst();
		
		// to const
		tofu::AnyBasePtr<const A>  p11( p5 );
		//p11 = p5; // ��肭�����ĂȂ�
		
		// ���Z�q
		p5->a = 1;
		IUTEST_ASSERT_EQ( p5->a, (*p10).a );
		
		// del
		tofu::AnyBasePtr<A>  p20( new AB() );
		p20.del();
		
		//------------------------------------------------------------------------------
		
		// ��r AnyBasePtr<T>���m
		{
			AB ab_array[2];
			tofu::AnyBasePtr<A>  p100( ab_array + 0 );
			tofu::AnyBasePtr<A>  p200( ab_array + 1 );
			tofu::AnyBasePtr<A>  p300( p100 );
			tofu::AnyBasePtr<A>  p400;
			
			// ==
			IUTEST_ASSERT_FALSE( p100 == p200 );
			IUTEST_ASSERT_TRUE ( p100 == p300 );
			IUTEST_ASSERT_FALSE( p100 == p400 );
		
			// !=
			IUTEST_ASSERT_TRUE ( p100 != p200 );
			IUTEST_ASSERT_FALSE( p100 != p300 );
			IUTEST_ASSERT_TRUE ( p100 != p400 );
			
			// <
			IUTEST_ASSERT_TRUE ( p100 < p200 );
			IUTEST_ASSERT_FALSE( p100 < p300 );
			IUTEST_ASSERT_FALSE( p100 < p400 );
			
			// <=
			IUTEST_ASSERT_TRUE ( p100 <= p200 );
			IUTEST_ASSERT_TRUE ( p100 <= p300 );
			IUTEST_ASSERT_FALSE( p100 <= p400 );
			
			// >
			IUTEST_ASSERT_FALSE( p100 > p200 );
			IUTEST_ASSERT_FALSE( p100 > p300 );
			IUTEST_ASSERT_TRUE ( p100 > p400 );

			// >=
			IUTEST_ASSERT_FALSE( p100 >= p200 );
			IUTEST_ASSERT_TRUE ( p100 >= p300 );
			IUTEST_ASSERT_TRUE ( p100 >= p400 );
		}
		// ��r nullptr ��
		{
			// ==
			IUTEST_ASSERT_FALSE( p5 == nullptr ); // 0 == null
			IUTEST_ASSERT_FALSE( nullptr == p5 ); // 0 == null
		
			// !=
			IUTEST_ASSERT_TRUE( p5 != nullptr ); // 0 != null
			IUTEST_ASSERT_TRUE( nullptr != p5 ); // 0 != null
		}
	}
	//------------------------------------------------------------------------------

	const A c_a;
	
	tofu::AnyBasePtr<A>  p1;
	p1 = &a;
	p1 = &ab;
	
	IUTEST_ASSERT_TRUE( tofu::MakeTypeId<AB>() == p1.type() );
	
	// cast ���
	p1 = &a;
	IUTEST_ASSERT_TRUE( &a == p1.cast<A>() ); // ����̌^��OK
	IUTEST_ASSERT_TRUE( &a == p1.cast<const A>() ); // const���i��OK
//	IUTEST_ASSERT_TRUE( nullptr == p1.cast<B>() ); // �ʂ̌^��NG, ������assert�Ɉ���������
	
	IUTEST_ASSERT_TRUE( &a == p1.tryCast<A>() ); // ����̌^��OK
	IUTEST_ASSERT_TRUE( &a == p1.tryCast<const A>() ); // const���i��OK
//	IUTEST_ASSERT_TRUE( nullptr == p1.tryCast<B>() ); // �ʂ̌^��NG
	
	// cast �h��
	p1 = &ab;
	IUTEST_ASSERT_TRUE( &ab == p1.cast<A>() ); // ���N���X�ւ̃L���X�g
	IUTEST_ASSERT_TRUE( &ab == p1.cast<const A>() ); // const���N���X�ւ̃L���X�g
	IUTEST_ASSERT_TRUE( &ab == p1.cast<AB>() ); // ����̌^��OK
	IUTEST_ASSERT_TRUE( &ab == p1.cast<const AB>() ); // const���i��OK
//	IUTEST_ASSERT_TRUE( nullptr == p1.cast<B>() ); // �ʂ̌^��NG, �R���p�C���G���[
	
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<const A>() ); // 
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<AB>() ); // ����̌^��OK
	IUTEST_ASSERT_TRUE( &ab == p1.tryCast<const AB>() ); // const���i��OK
//	IUTEST_ASSERT_TRUE( nullptr == p1.tryCast<B>() ); // �ʂ̌^��NG, �R���p�C���G���[
	
	IUTEST_ASSERT_TRUE( &ab == (A*)(p1) ); // ���N���X�ւ̃L���X�g���Z��OK
	IUTEST_ASSERT_TRUE( &ab == (const A*)(p1) ); // ���N���X�ւ̃L���X�g���Z��OK
	A* pa = p1;
	IUTEST_ASSERT_TRUE( &ab == pa ); // ���N���X�ւ̃L���X�g���Z��OK

	// const�֌W
	{
		tofu::AnyBasePtr<const A>  p2;
		p2 = &c_a; // const
		p2 = &a; // const���i

		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<A>() ); // ��const�ւ̓L���X�g�s��
		IUTEST_ASSERT_TRUE( &a == p2.tryCast<const A>() ); //
		
		p2 = &ab;
		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<A>() ); // ��const�ւ̓L���X�g�s��
		IUTEST_ASSERT_TRUE( &ab == p2.tryCast<const A>() ); //
		IUTEST_ASSERT_TRUE( nullptr == p2.tryCast<AB>() ); // ��const�ւ̓L���X�g�s��
		IUTEST_ASSERT_TRUE( &ab == p2.tryCast<const AB>() ); //
	}

	// �ʃe���v���[�g���m�̔�r
	{
		tofu::AnyBasePtr<A>  p100;
		tofu::AnyBasePtr<const A>  p200;
		
		IUTEST_ASSERT_TRUE( p100 == p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 != p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 < p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 > p200 ); // nullptr���m
		IUTEST_ASSERT_TRUE( p100 <= p200 ); // nullptr���m
		IUTEST_ASSERT_TRUE( p100 >= p200 ); // nullptr���m
	}
	{
		tofu::AnyBasePtr<A>  p100;
		tofu::AnyBasePtr<const AB>  p200;
		
		IUTEST_ASSERT_TRUE( p100 == p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 != p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 < p200 ); // nullptr���m
		IUTEST_ASSERT_FALSE( p100 > p200 ); // nullptr���m
		IUTEST_ASSERT_TRUE( p100 <= p200 ); // nullptr���m
		IUTEST_ASSERT_TRUE( p100 >= p200 ); // nullptr���m
	}
}
