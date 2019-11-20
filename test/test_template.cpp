#include <memory>
#include <unordered_map>
#include <utility>
#include <variant>
#include <optional>
#include <string>
#include <vector>
using namespace std;

#if defined(__clang__)
    #define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
    #define Meta(...) 
#endif

//namespace A
namespace A
{
    //struct s_1
    struct s_1
    {
        
        optional<int> a = 0;
        pair<int, optional<float>> b;
        tuple<int, float, string> c;
    };
    //class s_2
    class s_2:public enable_shared_from_this<s_2>
    {
        
        variant<int, float, string> d;
    };
    //class MyString
    class MyString: private string 
    {
        
        public:
        MyString(string s = ""):string(s)
        {

        }
        using string::size;
        std::string _Convert_size(char del = ',') const
        {
            return "";
        }
    }Meta(xxx,bb(ccc));
    class s_3;
    // class s_3
    class s_3:public variant<bool, uint32_t, double, string, std::vector<s_3>>
    {
        public:
        using base = variant<bool, uint32_t, double, string, std::vector<s_3>>;
        using base::base;
        using base::operator=;
    };
    
    //class s_4
    template <typename T, typename B = std::vector<T>>
    struct s_4
    {
        Meta(xxx,bb(ccc)) vector<T> _v ;
        Meta(xxx,bb(ccc)) B _s ;
		Meta(xxx,bb(ccc)) int d = -1 ;
		Meta(xxx,bb(ccc)) std::string s_d = "hehe" ;
		Meta(xxx,bb(ccc)) bool take(T a)
        
		{
			return false;
		}
    }Meta(xxx,bb(ccc));
	using s_4_int = s_4<int>;
}
int main()
{
    A::s_3 aa("heheh");


}