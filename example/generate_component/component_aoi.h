#include "component_base.h"
namespace test
{
    class aoi:public component_base
    {
    public:
        static component_type static_type()
        {
            return component_type::aoi;
        }
        aoi(entity_base* _in_owner):
        component_base(_in_owner, component_type::aoi)
        {

        }
        Meta(stub_func(20)) void on_created()
        {
            std::cout<<"on_create called from test"<<std::endl;
        }
        Meta(stub_func(75)) void on_enter_space()
        {
            std::cout<<"on_enter_space called from test"<<std::endl;
        }
        Meta(stub_func(71)) void on_leave_space()
        {
            std::cout<<"on_leave_space called from test"<<std::endl;
        }
        Meta(stub_func) void on_destroy()
        {
            std::cout<<"on_destroy called from test"<<std::endl;
        }
        Meta(rpc) void add_attention(const std::string& id);
        Meta(rpc) void remove_attention(const std::string& id);
        Meta(rpc) void clear_attention();
        Meta(rpc) void entities_in_range(float radius);
    };
}