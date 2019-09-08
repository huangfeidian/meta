#pragma once

#if defined(__meta_parse__)
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#define Meta(...) 
#endif

#include <vector>
#include <unordered_map>
#include <meta/serialize/encode.h>
using namespace std;
namespace test
{
	
	class test_class
	{
	private:
		Meta(encode) std::vector<int> a;
		Meta(encode) std::unordered_map<int, int> b;
		Meta(encode) int c;
	public:
#include "test_class.generated_h"

	}Meta(encode(auto));
	class test_class_B: public test_class
	{
		private:
		Meta(encode) std::vector<std::string> d;
	public:
#include "test_class_B.generated_h"
	}Meta(encode(auto));
}