#include <iostream>
#include <vector>
using namespace std;
namespace A::B
{
    void test_3();
    struct nothing
    {
        int a;
        nothing(int b);
        ~nothing();
        template <typename T>
        void accept(T c);
        void fail_nothing();
    };
    template <typename T>
    void nothing::accept(T c)
    {
        a = 0;
    }
    int test_1(int a, std::vector<int> hehe, int c)
    {
        return hehe.size();
    }
    template <typename A>
    int nothing_int(A a)
    {
        return 0;
    }
    std::vector<int> test_2(int capacity)
    {
        return std::vector<int>(capacity, 0);
    } 
    void test_3(void)
    {
        return;
    }
    void nothing::fail_nothing()
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