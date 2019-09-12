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
	class timer_scheduler
	{
	public:

	};
	class node;
	using event_type = std::string;
	enum class arg_value_choice
	{
		plain_value = 0,
		blackboard_value,
	};
	using node_arg_value_type = std::pair< arg_value_choice, std::string>;
	class agent
	{
	public:
		bool poll(); // first handle events then handle fronts
		void dispatch_event(event_type new_event);
	public:
		bool during_poll;
		std::deque<node*> _fronts; // node ready to run
		std::deque<event_type> _events; // events to be handled;
		any_str_map _blackboard;
	private:
		bool poll_fronts(); // run the nodes
		bool poll_events(); // handle the events;
		
	private:
		// actions return true or false for immediate result
		// if the result is not immediate then return nullopt
		bool key_in_bb(const std::string& bb_key)
		{
			auto cur_iter = _blackboard.find(bb_key);
			return cur_iter == _blackboard.end();
		}
		bool value_equal(const std::string& bb_key, const any_value_type& value)
		{
			auto cur_iter = _blackboard.find(bb_key);
			if (cur_iter == _blackboard.end())
			{
				return false;
			}
			
			const auto& cur_value = cur_iter->second;
			return cur_value == value;

		}
		bool add_int(const std::string& bb_key, const int& value)
		{
			auto cur_iter = _blackboard.find(bb_key);
			if (cur_iter == _blackboard.end())
			{
				return false;
			}
			auto& cur_value = cur_iter->second;
			if (cur_value.is_double())
			{
				std::get<double>(cur_value) += value;
				return true;
			}
			else if (cur_value.is_float())
			{
				std::get<float>(cur_value) += value;
				return true;
			}
			else if (cur_value.is_int())
			{
				std::get<int>(cur_value) += value;
				return true;
			}
			else if (cur_value.is_int64())
			{
				std::get<std::int64_t>(cur_value) += value;
				return true;
			}
			else
			{
				return false;
			}
		}
		bool set_key_value(const std::string& bb_key, const any_value_type& new_value)
		{
			_blackboard[bb_key] = new_value;
			return true;
		}
		template <typename T>
		bool numeric_larger_than(const std::string& bb_key, const any_value_type& other_value)
		{
			auto cur_iter = _blackboard.find(bb_key);
			if (cur_iter == _blackboard.end())
			{
				return false;
			}
			auto& cur_value = cur_iter->second;
			if (!std::holds_alternative<T>(cur_value))
			{
				return false;
			}
			else
			{
				const auto& cur_raw_value = std::get<T>(cur_value);
				if (other_value.is_double())
				{
					return cur_raw_value > std::get<double>(other_value);
				}
				else if (other_value.is_float())
				{
					return cur_raw_value > std::get<float>(other_value);
				}
				else if (other_value.is_int())
				{
					return cur_raw_value > std::get<int>(other_value);
				}
				else if (other_value.is_int64())
				{
					return cur_raw_value > std::get<std::int64_t>(other_value);
				}
				return false;
			}

		}
		bool int_larger_than(const std::string& bb_key, const any_value_type& other_value)
		{
			return numeric_larger_than<int>(bb_key, other_value);
		}
		bool int64_larger_than(const std::string& bb_key, const any_value_type& other_value)
		{
			return numeric_larger_than<std::int64_t>(bb_key, other_value);
		}
		bool float_larger_than(const std::string& bb_key, const any_value_type& other_value)
		{
			return numeric_larger_than<float>(bb_key, other_value);
		}
		bool double_larger_than(const std::string& bb_key, const any_value_type& other_value)
		{
			return numeric_larger_than<double>(bb_key, other_value);
		}
		std::optional<bool> wait_for_second(double duration)
		{

			return std::nullopt;
		}

	};
}