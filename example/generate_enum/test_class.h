#pragma once

#if defined(__meta_parse__)
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#define Meta(...) 
#endif

#include <vector>
#include <unordered_map>
#include <optional>
#include <string>
using namespace std;
namespace test
{
	
	enum class test_class
	{
		ERR_OK = 0,
		ERR_1 = 1,
		ERR_2 = 2,
		ERR_INVALID = 3

	}Meta(enum);
#include "test_class.generated_h"
}