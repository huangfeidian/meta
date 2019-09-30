#include "decode.h"
#include "any_value.h"
#include "../utility/logger.h"

namespace meta::serialize
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
	using var_idx_type = std::uint8_t;
	using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	const static std::uint8_t depth_max = 8;
	using var_prefix_idx_type = std::vector<var_idx_type>;
	enum class var_mutate_cmd : var_cmd_type
	{
		clear = 0,
		set = 1,
		mutate_item = 2,
		vector_push_back = 10,
		vector_idx_mutate = 11,
		vector_idx_delete = 12,
		vector_insert_head = 13,
		vector_pop_back = 14,
		map_insert = 20,
		map_erase = 21,
		set_add = 30,
		set_erase = 31,
		item_update = 41,

	};
	enum class notify_kind
	{
		no_notify = 0,
		self_notify = 1,
		all_notify = 2
	};
	
	using mutate_msg = std::tuple<var_prefix_idx_type, var_idx_type, var_mutate_cmd, json>;
	class msg_queue_base
	{
	public:
		virtual void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data) = 0;
	};
	class msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg>& _queue;
		const var_prefix_idx_type& parent_idxes;
	public:
		msg_queue(std::deque<mutate_msg>& _in_msg_queue,
			const var_prefix_idx_type& _in_parent_idxes)
			: _queue(_in_msg_queue)
			, parent_idxes(_in_parent_idxes)
		{

		}
		msg_queue(const msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
		{
			_queue.emplace_back(parent_idxes, offset, _cmd, _data);
			return;
		}

	};
	template <typename T>
	class item_msg_queue : msg_queue_base
	{
		std::deque<mutate_msg>& _queue;
		const var_prefix_idx_type& parent_idxes;
		const T& _item_key;
	public:
		item_msg_queue(std::deque<mutate_msg>& _in_msg_queue,
			const var_prefix_idx_type& _in_parent_idxes,
			const T& _in_key)
			: _queue(_in_msg_queue)
			, parent_idxes(_in_parent_idxes)
			, _item_key(_in_key)
		{

		}
		item_msg_queue(const item_msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
		{
			std::tuple<T, var_idx_type, var_mutate_cmd, json> _new_data;
			_queue.emplace_back(parent_idxes, 0, _cmd, encode_multi(_item_key, offset, _cmd, _data));
			return;
		}
	};
    template <typename T>
    class prop_proxy
    {
    public:
        prop_proxy(T& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& in_offset,
			notify_kind in_notify_kind = notify_kind::self_notify):
        _data(_in_data),
		_msg_queue(_in_msg_queue),
		_notify_kind(in_notify_kind),
		_offset(in_offset)

        {

        }
		T& get()
		{
			return _data;
		}
		operator const T&() const
		{
			return _data;
		}
		void set(const T& _in_data)
		{
			_data = _in_data;
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::set, encode(_data));
			}
			
		}
		
		void clear()
		{
			_data = {};
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::clear, json());
			}
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data = {};
			return true;
		}
    private:
        T& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type& _offset;
		const notify_kind _notify_kind;
	};
	template<typename T>
	class prop_proxy<std::vector<T>>
	{
	public:
		prop_proxy(std::vector<T>& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& _in_offset) :
			_data(_in_data),
			_msg_queue(_in_msg_queue),
			_offset(_in_offset)
		{

		}
		std::vector<T>& get()
		{
			return _data;
		}
		operator const std::vector<T>&() const
		{
			return _data;
		}
		void set(const std::vector<T>& _in_data)
		{
			_data = _in_data;
			_msg_queue.add(_offset, var_mutate_cmd::set, encode(_data));
		}
		
		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}
		
		void push_back(const T& _new_data)
		{
			_data.push_back(_new_data);
			_msg_queue.add(_offset, var_mutate_cmd::vector_push_back, encode(_new_data));
		}
		
		void pop_back()
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_pop_back, json());
		}
		
		void idx_mutate(std::size_t idx, const T& _new_data)
		{
			if (idx < _data.size())
			{
				_data[idx] = _new_data;
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_idx_mutate, encode_multi(idx, _new_data));
		}
		
		void idx_delete(std::size_t idx)
		{
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_idx_mutate, encode(idx));
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			case var_mutate_cmd::vector_push_back:
				return replay_push_back(j_data);
			case var_mutate_cmd::vector_pop_back:
				return replay_pop_back(j_data);
			case var_mutate_cmd::vector_idx_mutate:
				return replay_idx_mutate(j_data);
			case var_mutate_cmd::vector_idx_delete:
				return replay_idx_delete(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_push_back(const json& j_data)
		{
			T temp;
			if (decode(j_data, temp))
			{
				_data.push_back(temp);
				return true;
			}
			else
			{
				return false;
			}
		}
		bool replay_pop_back(const json& j_data)
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			return true;
		}
		bool replay_idx_mutate(const json& j_data)
		{
			std::size_t idx;
			T temp;
			if (!decode_multi(j_data, idx, temp))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data[idx] = temp;
			}
			return true;
		}
		bool replay_idx_delete(const json& j_data)
		{
			std::size_t idx;
			if (!decode(j_data, idx))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			return true;
		}
	private:
		std::vector<T>& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};

	template <typename T1, typename T2>
	class prop_proxy<std::unordered_map<T1, T2>>
	{
	public:
		prop_proxy(std::unordered_map<T1, T2>& _in_data,
			msg_queue_base& _in_msg_queue,
			const var_idx_type& _in_offset) :
			_data(_in_data),
			_msg_queue(_in_msg_queue),
			_offset(_in_offset)
		{

		}
		std::unordered_map<T1, T2>& get()
		{
			return _data;
		}
		operator const std::unordered_map<T1, T2>&() const
		{
			return _data;
		}
		void set(const std::unordered_map<T1, T2>& _in_data)
		{
			_data = _in_data;
			_msg_queue.add(_offset, var_mutate_cmd::set, encode(_data));
		}

		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}

		void insert(const T1& key, const T2& value)
		{
			_data[key] = value;
			_msg_queue.add(_offset, var_mutate_cmd::map_insert, encode_multi(key, value));
		}

		void erase(const T1& key)
		{
			_data.erase(key);
			_msg_queue.add(_offset, var_mutate_cmd::map_erase, encode(key));
		}

		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			case var_mutate_cmd::map_insert:
				return replay_insert(j_data);
			case var_mutate_cmd::map_erase:
				return replay_erase(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_insert(const json& j_data)
		{
			T1 key;
			T2 value;
			if (!decode_multi(j_data, key, value))
			{
				return false;
			}
			_data[key] = value;
			return true;
		}
		bool replay_erase(const json& j_data)
		{
			T1 key;
			if (!decode(j_data, key))
			{
				return false;
			}
			_data.erase(key);
			return true;
		}
	private:
		std::unordered_map<T1, T2>& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};

	
	class property_item_base;
	class property_bag_base;
	class property_item_base
	{
	private:
		property_bag_base* _container;
	public:
		property_item_base(property_bag_base* _in_container)
			: _container(_in_container)
		{

		}
		virtual json encode() const = 0;
		virtual std::string type_name() = 0;
		virtual bool replay_mutate_msg(std::size_t field_index,
			var_mutate_cmd cmd, const json& data) = 0;
	};
	class property_bag_base
	{
	public:
		const var_prefix_idx_type _depth;
		virtual json encode() const = 0;
		virtual bool decode(const json& data) = 0;
		virtual std::string type_name() = 0;
		std::deque<mutate_msg>& _dest_buffer;
		property_bag_base(var_prefix_idx_type _in_depth,
			std::deque<mutate_msg>& _in_cmd_queue) :
			_depth(_in_depth),
			_dest_buffer(_in_cmd_queue)
		{

		}
		virtual bool replay_mutate_msg(std::size_t field_index,
			var_mutate_cmd cmd, const json& data) = 0;

	};
	template <typename T>
	class property_item : public property_item_base
	{
	private:
		T _id;
	public:
		const T& ref_id() const
		{
			return _id;
		}
		property_item(property_bag_base* _in_container, 
			const T& _in_id,
			std::deque<mutate_msg>& _dest_queue):
			property_item_base(_in_container),
			_id(_in_id),
			_cmd_buffer(_dest_queue, _in_container->_depth, _id)

		{

		}
		item_msg_queue<T> _cmd_buffer;
		json encode() const
		{
			json result;
			result["id"] = _id;
			return result;
		}
		bool decode(const json& data)
		{
			if (!data.is_object())
			{
				return false;
			}
			auto iter = data.find("id");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, _id))
			{
				return false;
			}
			return true;
		}
	};
	
	template <typename K, typename Item>
	class property_bag:public property_bag_base
	{
		std::vector<Item> _data;
		std::unordered_map<K, std::size_t> _index;
		std::shared_ptr<spdlog::logger> _logger;
	public:
		using property_bag_base::property_bag_base;
		using item_type = Item;
		using key_type = K;
		json encode() const
		{
			return encode(_data);
		}
		bool decode(const json& in_data)
		{
			if (_data.size())
			{
				_logger->error("the bag is not empty while decode data {} to property_bag {}",
					in_data.dump(), type_name());
				return false;
			}
			if (!::decode(in_data, _data))
			{
				_logger->error("fail to decode data {} to property_bag {}",
					in_data.dump(), type_name());
				return false;
			}
			for (std::size_t i = 0; i < _data.size(); i++)
			{
				_index[_data[i].id()] = i;
			}
			return ::decode(in_data, _data);
		}
		bool has_item(const K& _key) const
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool create(const json& data)
		{
			Item temp_item(this, _dest_buffer, T());
			if (!::decode(data, temp_item))
			{
				_logger->error("property_bag {} fail to create item with data {}", 
					type_name(), data.dump());
				return false;
			}
			_index[temp_item.ref_id()] = _data.size();
			_data.emplace_back(std::move(temp_item));
			return true;
		}
		Item& create(const K& key)
		{
			Item temp_item(this, _dest_buffer, key);
			_index[temp_item.ref_id()] = _data.size();
			_data.emplace_back(std::move(temp_item));
			return _data.back();
		}
		void clear()
		{
			_index.clear();
			_data.clear();
		}
		bool erase(const K& key)
		{
			auto cur_iter = _index.find(key);
			if (cur_iter == _index.end())
			{
				return false;
			}
			else
			{
				if (cur_iter->second == _data.size())
				{
					_index.erase(cur_iter);
					_data.pop_back();
				}
				else
				{
					auto pre_index = cur_iter->second;
					_index.erase(cur_iter);
					_index[_data[pre_index].id()] = pre_index;
					std::swap(_data[cur_iter->second], _data.back());
				}
				return true;
			}
		}
		std::optional<std::reference_wrapper<const Item>> get(const K& key) const
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::cref(_data[cur_iter->second]);
			}
		}
		std::optional<std::reference_wrapper<Item>> get_mut(const K& key)
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::ref(_data[cur_iter->second]);
			}
		}
	};
	template <typename K, typename Item>
	class prop_proxy<property_bag<K, Item>>
	{
		property_bag<K, Item>& _data;
	public:
		prop_proxy(property_bag<K, Item>& _in_data,
			msg_queue_base& _in_msg_queue,
			const var_idx_type& _in_offset)
		{

		}
		operator const std::vector<Item>&() const
		{
			return _data._data;
		}
		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}
		std::vector<K> keys() const
		{
			std::vector<K> result;
			result.reserve(_data._data.size());
			for (const auto& one_item : _data._data)
			{
				result.push_back(one_item.first);
			}
			return result;
		}
		void insert(const K& key, const json& value)
		{
			if (_data.create(value))
			{
				_msg_queue.add(_offset, var_mutate_cmd::map_insert, 
					encode_multi(key, value));
			}
			
		}

		void erase(const K& key)
		{
			if (_data.erase(key))
			{
				_msg_queue.add(_offset, var_mutate_cmd::map_erase, encode(key));
			}
			
		}

		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::map_insert:
				return replay_insert(j_data);
			case var_mutate_cmd::map_erase:
				return replay_erase(j_data);
			default:
				return false;
			}
		}
	};


}