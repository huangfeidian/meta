#pragma once
#include <vector>
#include <unordered_map>
#include <deque>
#include <string>
#include <meta/serialize/decode.h>
#include <meta/serialize/any_value.h>
#include "timer_manager.hpp"
using namespace meta::serialize;
namespace bahavior
{
	class node;
	class agent;
	using event_type = std::string;
	enum class arg_value_choice
	{
		plain_value = 0,
		blackboard_value,
	};
	using node_arg_value_type = std::pair< arg_value_choice, std::string>;
	enum class node_type
	{
		invalid = 0,
		node_root,
		node_not,
		node_true,
		node_sequence,
		node_always_seq,
		node_if_else,
		node_while,
		node_wait_event,
		node_reset,
		node_sub_tree,
	};
	enum class node_state
	{
		init = 0,
		enter,
		revisit,
		leave,
		finish,
		destroy
	};
	class agent
	{
	public:
		bool poll(); // first handle events then handle fronts
		void dispatch_event(event_type new_event);
	public:
		bool during_poll;
		std::vector<node*> _fronts; // node ready to run
		std::vector<event_type> _events; // events to be handled;
		any_str_map _blackboard;
		std::unordered_map<const node*, timer_handler> _timers;
	private:
		node* current_poll_node;
	private:
		bool poll_fronts(); // run the nodes
		bool poll_events(); // handle the events;
		void poll_node(node* cur_node);// run one node

	private:
		// actions return true or false for immediate result
		// if the result is not immediate then return nullopt
		bool has_key(const std::string& bb_key);
		bool set_key_value(const std::string& bb_key, const any_value_type& new_value);
		bool has_key_value(const std::string& bb_key, const any_value_type& value);
		bool number_add(const std::string& bb_key, const any_value_type& value);
		bool number_dec(const std::string& bb_key, const any_value_type& value);
		bool number_multiply(const std::string& bb_key, const any_value_type& value);
		bool number_div(const std::string& bb_key, const any_value_type& value);
		bool number_larger_than(const std::string& bb_key, const any_value_type& other_value);
		bool number_less_than(const std::string& bb_key, const any_value_type& other_value);


		std::optional<bool> wait_for_second(double duration);
	};
}