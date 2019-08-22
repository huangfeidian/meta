#include "component_base.h"

namespace test
{
    class transform: public component_base
    {
        static component_type static_type()
        {
            return component_type::transform;
        }
        void on_created();
        void on_enter_space();
        void on_leave_space();
        void on_destroy();
    }
}