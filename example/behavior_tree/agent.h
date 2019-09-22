#pragma once
#include <vector>
#include <unordered_map>
#include <deque>
#include <string>
#include <meta/serialize/decode.h>
#include <meta/serialize/any_value.h>
#include <meta/macro.h>

#include "timer_manager.hpp"
#include "btree.h"

namespace bahavior
{
	class node;
	class agent;
	class btree;
	using event_type = std::string;
	enum class arg_value_choice
	{
		plain_value = 0,
		blackboard_value,
	};
	class agent
	{
	public:
		
		friend class node;
	public:
		bool poll(); // first handle events then handle fronts
		void dispatch_event(const event_type& new_event);
		bool is_running() const
		{
			return _enabled;
		}
		void notify_stop();
	public:
		bool during_poll;
		std::vector<node*> _fronts; // node ready to run
		std::vector<node*> pre_fronts;
		std::vector<event_type> _events; // events to be handled;
		meta::serialize::any_str_map _blackboard;
		std::unordered_map<const node*, timer_handler> _timers;
		std::optional<bool> agent_action(node* cur_node, const std::string& action_name, 
			const meta::serialize::any_vector& action_args);
	protected:
		node* current_poll_node;
		std::shared_ptr<spdlog::logger> _logger;
		bool _enabled = false;
	private:
		bool poll_fronts(); // run the nodes
		bool poll_events(); // handle the events;
		void poll_node(node* cur_node);// run one node
		

	protected:
		
		static const btree* load_tree(const std::string& tree_name);
		static std::string btree_respository;

		static std::unordered_map<std::string, const btree*> _btrees;
	public:
		static bool set_btree_directory(const std::string& cur_directory);
		
	};
	class action_agent : public agent
	{
		// actions return true or false for immediate result
		// if the result is not immediate then return nullopt
		Meta(behavior_action) bool has_key(const std::string& bb_key);
		Meta(behavior_action) bool set_key_value(const std::string& bb_key, const meta::serialize::any_value_type& new_value);
		Meta(behavior_action) bool has_key_value(const std::string& bb_key, const meta::serialize::any_value_type& value);
		Meta(behavior_action) bool number_add(const std::string& bb_key, const meta::serialize::any_value_type& value);
		Meta(behavior_action) bool number_dec(const std::string& bb_key, const meta::serialize::any_value_type& value);
		Meta(behavior_action) bool number_multiply(const std::string& bb_key, const meta::serialize::any_value_type& value);
		Meta(behavior_action) bool number_div(const std::string& bb_key, const meta::serialize::any_value_type& value);
		Meta(behavior_action) bool number_larger_than(const std::string& bb_key, const meta::serialize::any_value_type& other_value);
		Meta(behavior_action) bool number_less_than(const std::string& bb_key, const meta::serialize::any_value_type& other_value);
		std::optional<bool> wait_for_seconds(double duration);
	};
}