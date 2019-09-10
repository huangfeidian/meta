#pragma once
#include <vector>
#include <unordered_map>
#include <deque>
#include <string>
#include <meta/serialize/decode.h>

namespace bahavior
{
	class node;
	using event_type = std::string;
	class agent
	{
	public:
		bool during_poll;
		std::deque<node*> _fronts; // node ready to run
		std::deque<event_type> _events; // events to be handled;
		bool poll_fronts(); // run the nodes
		bool poll_events(); // handle the events;
		bool poll(); // first handle events then handle fronts
		void dispatch_event(event_type new_event);
		

	};
}