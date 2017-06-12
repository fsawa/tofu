
#include <iostream>
#include <iutest.hpp>

int main(int argc, char* argv[])
{
	std::cout << "Hello Test!" << std::endl;
	IUTEST_INIT(&argc, argv);
	return IUTEST_RUN_ALL_TESTS();
}

IUTEST(Test, Version)
{
	unsigned long v = (IUTEST_MAJORVER << 24) | (IUTEST_MINORVER << 16) | (IUTEST_BUILD << 8) | IUTEST_REVISION;
	IUTEST_ASSERT_EQ(IUTEST_VER, v);
}
