#include <iomanip>
#include <nlohmann/json.hpp>
#include <iostream>
#include <unordered_map>
#include <meta/utility/string_utils.h>
using namespace spiritsaway::meta;
using json = nlohmann::json;
using namespace std;
void encode_test_print(const std::string& data)
{
	json encode_result = utils::string_utils::parse_annotation(data);
	cout << setw(4) << "parse data " << data << " with result " << encode_result<<endl;
}

void encode_test()
{
	encode_test_print("");
	encode_test_print("a,b");
	encode_test_print("a(b)");
	encode_test_print("a(b), c");
	encode_test_print("a(b), c(d)");
	encode_test_print("a(b, c)");
	encode_test_print("a(b, c), d(e, f)");
	encode_test_print("a(b=f, c=e), d(e=c, f=d)");
	encode_test_print("typename(const json&)");
}
int main()
{
	encode_test();
}