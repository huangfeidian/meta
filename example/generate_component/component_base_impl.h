namespace test
{
    template <typename T>
    std::optional<std::reference_wrapper<T>> component_base::owner_entity()
    {
        if(!_owner)
        {
            return std::nullopt;
        }
        if(T::static_name() != _owner->name())
        {
            return std::nullopt;
        }
        return std::cref(*dynamic_cast<T*>(_owner));
    }
    template <typename T>
    bool component_base::has_entity() const
    {
        if(!_owner)
        {
            return false;
        }
        if(T::static_name() != _owner->name())
        {
            return false;
        }
        return true;
    }

}
#include "entity_base.h"