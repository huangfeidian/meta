#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
using namespace std;
namespace A::B
{
    enum e_a
    {
        test_e_1
    };
    enum class e_b:std::uint8_t
    {
        test_e_2,
        test_e_3
    };
    struct C
    {
    public:
        int a;
    private:
        int c;
        static int d;

        static int fail_me();
        enum e_c
        {
            test_e_d
        };
    };
    int C::fail_me()
    {
        return 1;
    }
    struct nothing
    {
        int a;
        nothing(int b);
        ~nothing();
        template <typename T>
        void accept(const T c);
        void fail_nothing(const int a, const std::uint16_t b, const int* const c);
    };
    template <typename T>
    class silly
    {
        public:
        std::vector<T> _v;
    };
    template <typename T>
    void nothing::accept(const T c)
    {
        a = 0;
    }
    int test_1(const std::vector<std::unordered_map<int, nothing>>&  a,  std::vector<int>&& b, std::vector<std::uint32_t>& c)
    {
        return b.size();
    }
    template <typename A>
    int nothing_int(A a)
    {
        return 0;
    }
    template <typename A>
    int nothing_vector(std::vector<A> a)
    {
        return 0;
    }
    std::vector<int> test_2(const std::unordered_map<int, std::string> capacity)
    {
        return std::vector<int>(capacity.size(), 0);
    } 
    void test_3(const void* const wawa, const C c)
    {
        return;
    }
    void test_4(const void* & wawa, const int& bb, const nothing&  cc, const int volatile dd)
    {
        return;
    }
    void nothing::fail_nothing(const int  a, const std::uint16_t b, const int* const c)
    {
        return;
    }
    void test_array(int a[3], char b[4], std::array<int, 3> cc)
    {
        return;
    }
}
namespace C = A::B;
int main()
{
    C::nothing temp(1);
    cout<<"hello world"<<endl;
}