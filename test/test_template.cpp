#include <memory>
#include <unordered_map>
#include <utility>
#include <variant>
#include <optional>
#include <string>
using namespace std;
namespace A
{
    struct s_1
    {
        optional<int> a;
        pair<int, optional<float>> b;
        tuple<int, float, string> c;
    };
    class s_2:public enable_shared_from_this<s_2>
    {
        variant<int, float, string> d;
    };
    class MyString: private string
    {
        public:
        MyString(string s):string(s)
        {

        }
        using string::size;
    };
    class s_3;
    class s_3:public variant<bool, int, double, string, vector<s_3>>
    {
        public:
        using base = variant<bool, int, double, string, vector<s_3>>;
        using base::base;
        using base::operator=;
    };
    template <typename T, typename B = vector<T>>
    struct s_4
    {
        T _v;
        B _s;
    };
}
int main()
{
    A::s_3 aa("heheh");


}