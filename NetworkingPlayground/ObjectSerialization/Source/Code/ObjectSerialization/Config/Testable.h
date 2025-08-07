#pragma once

#ifdef USING_GTEST
	#define GENERATE_TEST_EXTENSION(ClassName)	\
			friend class ClassName##_Testable;				
#else
	#define GENERATE_TEST_EXTENSION(ClassName)
#endif


