public:
void register_components()
{
    add_component(_aoi, _aoi->type());
    add_component(_transform, _transform->type());
}
private:


public:
void on_created()
{
    _aoi->on_created(); // priority(20)
    _transform->on_created(); // priority(0)
}
void on_destroy()
{
    _transform->on_destroy(); // priority(10)
    _aoi->on_destroy(); // priority(0)
}
void on_enter_space()
{
    _aoi->on_enter_space(); // priority(75)
    _transform->on_enter_space(); // priority(11)
}
void on_leave_space()
{
    _aoi->on_leave_space(); // priority(71)
    _transform->on_leave_space(); // priority(5)
}
private:

