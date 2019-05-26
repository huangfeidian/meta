#include <iostream>

using namespace std;
namespace A::B
{
    struct nothing
    {
        int a;
    };
}
namespace C = A::B;
int main()
{
    C::nothing temp;
    cout<<"hello world"<<endl;
}