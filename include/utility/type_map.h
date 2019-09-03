#pragma once
#include <utility>
#include <functional>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <optional>
#include <type_traits>
namespace meta::utils
{
    template <typename V>
    class type_map 
    {
        public:
        template <class K>
        std::optional<V> find() const
        { 
            auto cur_iter = m_map.find(get_type_id<K>()); 
            if(cur_iter == m_map.end())
            {
                return std::nullopt;
            }
            else
            {
                return cur_iter->second;
            }
            
        }

        template <class K>
        bool exist_type() const
        {
            auto cur_iter = m_map.find(get_type_id<K>()); 
            if(cur_iter == m_map.end())
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        bool exist_id(int cur_idx) const
        {
            auto cur_iter = m_map.find(cur_idx); 
            if(cur_iter == m_map.end())
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        template <class K>
        void register_type(const V& type_name) 
        {
            m_map[get_type_id<K>()] = type_name;
        }  

        template <class K>
        bool can_convert_to(int dest_type_id) const
        {
			if (get_type_id<K>() == dest_type_id)
			{
				return true;
			}
			using B = std::remove_const_t<std::remove_reference_t<K>>;
			if (std::is_lvalue_reference_v<K>)
			{
				// k is B& or const B&
				if (std::is_const_v<std::remove_reference_t<K>>)
				{
					// K is const B&
					// dest should be B/constB/ const B&
					if (get_type_id<B>() == dest_type_id)
					{
						return true;
					}
					if (get_type_id<const B> == dest_type_id)
					{
						return true;
					}
					
					return false;
				}
				else
				{
					// K is B &
					// dest should be B/B&/const B/ const B&
					if (get_type_id<B>() == dest_type_id)
					{
						return true;
					}
					
					if (get_type_id<const B> == dest_type_id)
					{
						return true;
					}
					if (get_type_id<const B &>() == dest_type_id)
					{
						return true;
					}
					return false;
				}
			}
			else
			{
				//K is B or const B
				// dest should be B/ /constB/ const B&
				if (get_type_id<B>() == dest_type_id)
				{
					return true;
				}
				
				if (get_type_id<const B> == dest_type_id)
				{
					return true;
				}
				if (get_type_id<const B&> == dest_type_id)
				{
					return true;
				}
				return false;
			}
        }
		template <class K>
		bool can_convert_from(int dest_type_id) const
		{
			if (get_type_id<K>() == dest_type_id)
			{
				return true;
			}
			using B = std::remove_const_t<std::remove_reference_t<K>>;
			if (std::is_lvalue_reference_v<K>)
			{
				// k is B& or const B&
				if (std::is_const_v<std::remove_reference_t<K>>)
				{
					// K is const B&
					// dest should be B/B&/constB/ const B&
					if (get_type_id<B>() == dest_type_id)
					{
						return true;
					}
					if (get_type_id<B &>() == dest_type_id)
					{
						return true;
					}
					if (get_type_id<const B> == dest_type_id)
					{
						return true;
					}
					return false;
				}
				else
				{
					// K is B &
					// dest should be B/B&
					if (get_type_id<B>() == dest_type_id)
					{
						return true;
					}
					if (get_type_id<B &>() == dest_type_id)
					{
						return true;
					}
					return false;
				}
			}
			else
			{
				//K is B or const B
				// dest should be B/B&/constB/ const B&
				if (get_type_id<B>() == dest_type_id)
				{
					return true;
				}
				if (get_type_id<B &>() == dest_type_id)
				{
					return true;
				}
				if (get_type_id<const B> == dest_type_id)
				{
					return true;
				}
				if (get_type_id<const B&> == dest_type_id)
				{
					return true;
				}
				return false;
			}

		}
        template <typename... Args, std::size_t... idx>
        bool can_convert_to_vector_impl(const std::vector<int>& dest_indexes, std::index_sequence<idx...> indexes)
        {
            if(dest_indexes.size() != std::tuple_size_v<std::tuple<Args...>>)
            {
                return false;
            }
            return (can_convert_to<Args>(dest_indexes[idx]) &&...);
        }
        template <typename... Args>
        bool can_convert_to(const std::vector<int>& dest_indexes)
        {
			return can_convert_to_vector_impl<Args...>(dest_indexes, std::index_sequence_for<Args...>{});
        }
        public:
        template <class Key>
        inline static int get_type_id() 
        {
            static const int id = last_id++;
            return id;
        }
        private:
        static std::atomic_int last_id;
        static std::unordered_map<int, V> m_map;
    };

    template <typename V>
    std::atomic_int type_map<V>::last_id(0);

	template <typename V>
	std::unordered_map<int, V> type_map<V>::m_map({});

    template <typename V>
    std::optional<std::size_t> all_args_registered(const std::vector<int>& _indexes)
    {
        auto cur_type_map = type_map<V>();
        for(std::size_t i = 0; i< _indexes.size(); i++)
        {
            if(!cur_type_map.exist_id(_indexes[i]))
            {
                return i;
            }
        }
        return std::nullopt;

    }
	template <typename F>
	class function_arguments;
	template <typename R, typename... Args> 
	class function_arguments<R(Args...)>
	{
	public:
		using type = std::tuple<Args...>;
	};

	template <typename R, typename T, typename... Args>
	class function_arguments<R(T::*)(Args...)>
	{
	public:
		using type = std::tuple<Args...>;
	};

	template <typename V, typename... Args>
	class func_arg_type_ids
	{
	public:
		static std::vector<int> result()
		{
			auto& cur_type_map = type_map<V>();
			std::vector<int> result;
			(result.push_back(cur_type_map.get_type_id<Args>()), ...);
			return result;
		}
		
	};
	template <typename V, typename... Args>
	class func_arg_type_ids<V, std::tuple<Args...>>
	{
	public:
		static std::vector<int> result()
		{
			auto& cur_type_map = type_map<V>();
			std::vector<int> result;
			(result.push_back(cur_type_map.get_type_id<Args>()), ...);
			return result;
		}

	};
}