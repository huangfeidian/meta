#include <cstdint>
#include <unordered_map>
#include <vector>
#include <deque>
namespace meta::utils
{
	template <typename E, typename F>
	class dispatcher
	{
	public:
		using handler_type = std::size_t;
		using callback_type = void(E, F);
	private:
		handler_type _handler_idx = 0;
		std::unordered_map<E, std::unordered_map<handler_type, callback_type>> event_callbacks;
		std::deque<std::pair<E, F>> _delayed_events;
		bool during_process = false;
		std::deque<std::pair<E, handler_type>> _remove_handlers;
		std::deque<std::tuple<E, handler_type, callback_type>> _delayed_add_listener;
	public:
		handler_type listen(E e, callback_type _callback)
		{


			auto cur_handler = _handler_idx++;
			if(!during_process)
			{
				_listen_impl(e, cur_handler, _callback);
			}
			else
			{
				_delayed_add_listener.emplace_back(e, cur_handler, _callback);
			}
			return cur_handler;
		}
		void notify(E e, F args)
		{
			_delayed_events.emplace_back(e, args);
			if(!during_process)
			{
				poll();
			}
			
		}
		void detach(E e, handler_type _handler)
		{
			if(during_process)
			{
				_remove_handlers.emplace_back(e, _handler);
			}
			else
			{
				_detach_impl(e, _handler);
			}
			
		}
	private:
		void _detach_impl(E e, handler_type _handler)
		{
			auto temp_iter1 = event_callbacks.find(e);
			if(temp_iter1 == event_callbacks.end())
			{
				return;
			}
			temp_iter1->second.erase(_handler);
			return;
		}
		void _listen_impl(E e, handler_type cur_handler, callback_type _callback)
		{
			auto cur_iter = event_callbacks.find(e);
			if(cur_iter == event_callbacks.end())
			{
				event_callbacks[e] = std::unordered_map<handler_type, callback_type>();
			}
			auto& pre_callbacks = event_callbacks[e];
			pre_callbacks[cur_handler] = _callback;
		}
		void poll()
		{
			during_process = true;
			while(!_delayed_events.empty())
			{
				auto [e, arg] = _delayed_events.front();
				_delayed_events.pop_front();
				auto cur_iter = event_callbacks.find(e);
				if(cur_iter != event_callbacks.end())
				{
					for(const auto& one_callback: cur_iter->second)
					{
						one_callback.second(e, arg);
					}
				}
				while(!_remove_handlers.empty())
				{
					auto [e2, handler2] = _remove_handlers.front();
					_remove_handlers.pop_front();
					_detach_impl(e2, handler2);
				}
				while(!_delayed_add_listener.empty())
				{
					auto [e3, cur_handler, callback3] = _delayed_add_listener.front();
					_delayed_add_listener.pop_front();
					_listen_impl(e3, cur_handler, callback3);
				}
			}
			during_process = false;
		}

	};
}