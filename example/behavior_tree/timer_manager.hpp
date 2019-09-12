﻿#include <deque>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <memory>
#include <queue>
class timer_manager
{
public:
	using callback_type = std::function<void()>;
	using handler_t = std::uint64_t;
	using ts_t = std::chrono::time_point<std::chrono::system_clock>;
private:
	timer_manager()
	{

	}
private:
	handler_t max_handler_used = 0;
	std::vector<handler_t> handler_for_reuse;
	std::unordered_map<handler_t, callback_type> _callbacks;
	std::priority_queue<std::pair<ts_t, handler_t>> ts_handlers;
private:
	handler_t gen_handler()
	{
		if (handler_for_reuse.empty())
		{
			max_handler_used += 1;
			return max_handler_used;
		}
		else
		{
			auto result = handler_for_reuse.back();
			handler_for_reuse.pop_back();
			return result;
		}
	}
public:
	static timer_manager& instance()
	{
		static timer_manager _the_instance;
		return _the_instance;
	}

	handler_t add_timer_with_ts(ts_t cur_expire_ts, callback_type cur_callback)
	{
		auto cur_handler = gen_handler();
		_callbacks[cur_handler] = cur_callback;
		ts_handlers.emplace(cur_expire_ts, cur_handler);
	}
	handler_t add_timer_with_gap(std::chrono::microseconds cur_expire_gap, callback_type cur_callback)
	{
		if (cur_expire_gap.count() < 0)
		{
			return 0;
		}
		auto cur_handler = gen_handler();
		
		auto expire_ts = std::chrono::system_clock::now() + cur_expire_gap;
		return add_timer_with_ts(expire_ts, cur_callback);
	}
	bool is_timer_active(handler_t cur_handler)
	{
		auto cur_iter = _callbacks.find(cur_handler);
		return cur_iter == _callbacks.end();
	}
	bool cancel_timer(handler_t cur_handler)
	{
		auto cur_iter = _callbacks.find(cur_handler);
		if (cur_iter == _callbacks.end())
		{
			return false;
		}
		else
		{
			_callbacks.erase(cur_iter);
			return true;
		}
	}
	std::size_t poll()
	{
		auto now = std::chrono::system_clock::now();
		std::size_t count = 0;
		while (!ts_handlers.empty())
		{
			auto cur_item = ts_handlers.top();
			if (cur_item.first < now)
			{
				auto cur_handler = cur_item.second;
				ts_handlers.pop();
				auto cur_iter = _callbacks.find(cur_handler);
				if (cur_iter != _callbacks.end())
				{
					auto cur_callback = cur_iter->second;
					_callbacks.erase(cur_iter);
					cur_callback();
				}
				handler_for_reuse.push_back(cur_handler);
			}
		}
	}
};
class timer_handler
{
private:
	const timer_manager::handler_t _handler;
public:
	timer_handler(timer_manager::handler_t in_handler) :
		_handler(in_handler)
	{

	}
	bool is_active()
	{
		return timer_manager::instance().is_timer_active(_handler);
	}
	bool cancel()
	{
		return timer_manager::instance().cancel_timer(_handler);
	}
};