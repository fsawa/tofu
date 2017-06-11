
#include <iostream>
#include <iutest.hpp>

#include <tofu/misc/TypeInfo.hpp>


int main(int argc, char* argv[])
{
	std::cout << "Hello Test!" << std::endl;
	IUTEST_INIT(&argc, argv);
	return IUTEST_RUN_ALL_TESTS();
}

namespace test
{
	struct A {};
}

IUTEST(misc, TypeInfo)
{
	{
		auto& info = tofu::GetTypeInfo<int>();
		std::cout << info.getName() << std::endl;
		//IUTEST_ASSERT_EQ(tofu::MakeTypeId<int>().makeAddConst(), tofu::MakeTypeId<const int>());
	}
	{
		auto& info = tofu::GetTypeInfo<test::A>();
		std::cout << info.getName() << std::endl;
	}
}

IUTEST(Test, Version)
{
	unsigned long v = (IUTEST_MAJORVER << 24) | (IUTEST_MINORVER << 16) | (IUTEST_BUILD << 8) | IUTEST_REVISION;
	IUTEST_ASSERT_EQ(IUTEST_VER, v);
}
