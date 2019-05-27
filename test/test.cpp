#include <iostream>
#include <vector>
using namespace std;
namespace A::B
{
    struct nothing
    {
        int a;
        nothing(int b);
        ~nothing();
        template <typename T>
        void accept(T c)
        {
            a = 0;
        }
    };
    int test_1(const std::vector<int>& hehe)
    {
        return hehe.size();
    }
    std::vector<int> test_2(int capacity)
    {
        return std::vector<int>(capacity, 0);
    } 
    void test_3(void)
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