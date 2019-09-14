#include "agent.h"
namespace bahavior
{
	class node
	{
	public:
		bool in_fronts = false;
		node* parent = nullptr;
		std::vector<node*> children;
		bool result = false;
		bool running = false;
		bool _ready = false;
		node_state _state;
		std::uint8_t next_child_idx = 0;
		agent* _agent;
		void set_result(bool new_result)
		{
			result = new_result;
		}
		virtual bool handle_event(const event_type& cur_event)
		{
			return false;
		}
		bool ready() const
		{
			return _ready;
		}
		virtual void visit()
		{

		}
	};
}
