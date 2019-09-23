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

namespace behavior
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
		virtual std::optional<bool> agent_action(node* cur_node, const std::string& action_name, 
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
	private:

	};

}