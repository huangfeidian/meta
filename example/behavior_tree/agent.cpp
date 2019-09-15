#include "agent.h"
#include "nodes.h"
namespace bahavior
{
	bool agent::has_key(const std::string& bb_key)
	{
		auto cur_iter = _blackboard.find(bb_key);
		return cur_iter != _blackboard.end();
	}
	bool agent::set_key_value(const std::string& bb_key, const meta::serialize::any_value_type& new_value)
	{
		_blackboard[bb_key] = new_value;
		return true;
	}
	bool agent::has_key_value(const std::string& bb_key, const meta::serialize::any_value_type& value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}

		const auto& cur_value = cur_iter->second;
		return cur_value == value;

	}
	bool agent::number_add(const std::string& bb_key, const meta::serialize::any_value_type& value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_cal_add(value);
		return !!result;
	}
	bool agent::number_dec(const std::string& bb_key, const meta::serialize::any_value_type& value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_cal_dec(value);
		return !!result;
	}
	bool agent::number_multiply(const std::string& bb_key, const meta::serialize::any_value_type& value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_cal_multiply(value);
		return !!result;
	}
	bool agent::number_div(const std::string& bb_key, const meta::serialize::any_value_type& value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_cal_div(value);
		return !!result;
	}



	bool agent::number_larger_than(const std::string& bb_key, const meta::serialize::any_value_type& other_value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_larger_than(other_value);
		return result.value_or(false);
	}
	bool agent::number_less_than(const std::string& bb_key, const meta::serialize::any_value_type& other_value)
	{
		auto cur_iter = _blackboard.find(bb_key);
		if (cur_iter == _blackboard.end())
		{
			return false;
		}
		auto& cur_value = cur_iter->second;
		auto result = cur_value.numeric_less_than(other_value);
		return result.value_or(false);
	}
	std::optional<bool> agent::wait_for_seconds(double duration)
	{
		auto cur_timeout_closure = std::make_shared<timeout_closure>(current_poll_node);
		std::weak_ptr<timeout_closure> weak_closure = cur_timeout_closure;
		current_poll_node->_closure = std::move(cur_timeout_closure);
		auto timeout_lambda = [=]()
		{
			auto temp_callback = weak_closure.lock();
			if (temp_callback)
			{
				temp_callback->operator();
			}
		};
		duration = std::max(0.5, duration);
		auto cur_timer_handler = timer_manager::instance().add_timer_with_gap(
			std::chrono::microseconds(static_cast<int>(duration * 1000)), timeout_lambda);
		_timers[current_poll_node] = cur_timer_handler;
		return std::nullopt;
	}
	bool agent::poll()
	{
		if (during_poll)
		{
			return false;
		}
		during_poll = true;
		std::size_t poll_count = 0;
		while (true)
		{
			
			bool poll_result = false;
			poll_result |= poll_events();
			poll_result |= poll_fronts();
			if (!poll_result)
			{
				break;
			}
			poll_count += 1;
		}
		during_poll = false;
		if (poll_count)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool agent::poll_events()
	{
		if (_events.empty())
		{
			return false;
		}
		for (const auto& one_event : _events)
		{
			for (auto one_node : _fronts)
			{
				if (one_node->handle_event(one_event))
				{
					break;
				}
			}
		}
		_events.clear();
		return true;
		
	}
	bool agent::poll_fronts()
	{
		if (_fronts.empty())
		{
			return false;
		}
		std::vector<node*> pre_fronts;
		std::swap(pre_fronts, _fronts);
		int ready_count = 0;
		for (const auto& one_node : pre_fronts)
		{
			if (one_node->ready())
			{
				ready_count++;
				poll_node(one_node);
			}
			else
			{
				_fronts.push_back(one_node);
			}
		}
		return ready_count > 0;
	}
	void agent::poll_node(node* cur_node)
	{
		current_poll_node = cur_node;
		cur_node->visit();
		current_poll_node = nullptr;
	}
	void agent::dispatch_event(const event_type& new_event)
	{
		_events.push_back(new_event);
		poll();
	}
}