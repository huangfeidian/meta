#pragma once
#include "entity_base.h"
#include "component_aoi.h"
#include "component_transform.h"

namespace test
{
    class avatar: public entity_base
    {
    public:
        Meta(stub_func) void on_enter();
        Meta(stub_func) void on_created();
        Meta(stub_func) void on_leave();
        Meta(stub_func) void on_destroy();
    private:
        Meta(component) aoi _aoi;
        Meta(component) transform _transform;
    public:
        avatar():
        _aoi(this),
        _transform(this)
        {

        }
    #include "avatar_generated.h"
    }Meta(component_owner, stub_func, rpc_interface);
}
