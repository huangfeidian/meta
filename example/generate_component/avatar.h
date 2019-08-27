#pragma once
#include "entity_base.h"
#include "component_aoi.h"
#include "component_transform.h"

namespace test
{
    class avatar: public entity_base
    {
    public:
        Meta(stub_func) void on_enter_space();
        Meta(stub_func) void on_created();
        Meta(stub_func) void on_leave_space();
        Meta(stub_func) void on_destroy();
        Meta(rpc) void test_1(const std::vector<int>& a);
        Meta(rpc) void test_2(std::unordered_map<std::string, int> a, std::string b);
    private:
        Meta(component) aoi _aoi;
        Meta(component) transform _transform;
    public:
        avatar(const std::string& _in_id, const json& args):
        entity_base(_in_id, args),
        _aoi(this),
        _transform(this)
        {

        }
    #include "avatar_generated.h"
    }Meta(component_owner, stub_func, rpc);
}
