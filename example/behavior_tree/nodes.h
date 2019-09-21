#include "agent.h"
#include <algorithm>
#include <random>
namespace bahavior
{
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
		node_parallel
	};
	enum class node_state
	{
		init = 0,
		entering, 
		awaken,
		revisiting,
		blocking,
		leaving,
		wait_child,
		dead,
	};
	

	class node_closure
	{
	public:
		node_closure(node* _in_node):
			_node(_in_node)
		{

		}
		virtual void operator()() = 0;
		virtual ~node_closure() = 0;
	protected:
		node* _node;
	};
	class timeout_closure : public node_closure
	{
		virtual void operator()()
		{
			_node->set_result(true);

		}
		virtual ~timeout_closure()
		{

		}
	};
	class node
	{
	protected:
		static std::mt19937 _generator;
		static std::uniform_int_distribution<std::uint32_t> _distribution;
	public:
		bool in_fronts = false;
		node* _parent = nullptr;
		std::vector<node*> children;
		bool result = false;
		bool running = false;
		bool _ready = false;
		node_state _state;
		node_type _type;
		std::uint8_t next_child_idx = 0;
		agent* _agent;
		const std::uint16_t _node_idx;
		const btree_desc& btree_config;
		const node_desc& node_config;
		std::shared_ptr<node_closure> _closure;
		std::shared_ptr<spdlog::logger> _logger;

		node(node* in_parent, std::uint16_t in_node_idx, const btree_desc& in_btree, node_type in_type) :
			_parent(in_parent),
			_node_idx(in_node_idx),
			btree_config(in_btree),
			_state(node_state::init),
			next_child_idx(0),
			_agent(in_parent->_agent),
			_type(in_type),
			node_config(in_btree.nodes[in_node_idx]),
			_logger(std::move(meta::utils::logger_mgr::instance().create_logger("btree")))
		{

		}
		bool node_state_is_ready()
		{
			return _state == node_state::init || _state == node_state::awaken;
		}
		bool node_state_is_forbid_enter()
		{
			return _state == node_state::leaving || _state == node_state::dead;
		}
		void set_result(bool new_result)
		{
			if (node_state_is_forbid_enter())
			{
				_logger->warn("current state is {} while set result {} at node {}", int(_state), new_result, node_config.idx);
				_agent->notify_stop();
				return;
			}
			result = new_result;
			_state = node_state::dead;
			backtrace();
		}
		virtual bool handle_event(const event_type& cur_event)
		{
			return false;
		}
		bool ready() const
		{
			return _ready;
		}
		void visit()
		{
			
			switch (_state)
			{
			case bahavior::node_state::init:
				create_children();
				on_enter();
				if (_state == node_state::entering)
				{
					_logger->warn("btree {} on_enter node {} while after state {}", 
						btree_config.tree_name, node_config.idx, int(_state));
					_agent->notify_stop();
				}
				break;
			case bahavior::node_state::awaken:
				on_revisit();
				if (_state == node_state::revisiting)
				{
					_logger->warn("btree {} revisit node {} while after state {}",
						btree_config.tree_name, node_config.idx, int(_state));
					_agent->notify_stop();
				}
				break;
			
			default:
				break;
			}


		}
		void create_children()
		{
			if (!node_config.children.empty() && children.empty())
			{
				// 初始化所有的子节点
				for (auto one_child_idx : node_config.children)
				{
					children.push_back(create_node_by_idx(btree_config, one_child_idx));
				}
			}
		}
		virtual void on_enter()
		{
			_state = node_state::entering;
			next_child_idx = 0;
			result = false;
		}
		virtual void on_revisit()
		{
			_state = node_state::revisiting;
		}
		virtual void visit_child(std::uint16_t child_idx)
		{
			if (child_idx >= children.size())
			{
				_logger->warn("btree {} visit child {} at node {} while children size is {}",
					btree_config.tree_name, child_idx, node_config.idx, children.size());
				_agent->notify_stop();
				return;
			}
			children[child_idx]->_state = node_state::init;
			_agent->_fronts.push_back(children[child_idx]);

		}
		virtual void leave()
		{
			_closure.reset();
			_state = node_state::leaving;
		}
		virtual void interrupt()
		{
			if (_closure)
			{
				_closure.reset();
			}
			_state = node_state::dead;
			next_child_idx = 0;
		}
		virtual void backtrace()
		{
			if (_parent)
			{
				_agent->_fronts.push_back(_parent);
				_parent->_state = node_state::awaken;
			}
			else
			{
				_agent->_fronts.push_back(this);
				_state = node_state::awaken;
			}
		}
		const std::string& tree_name() const
		{
			return btree_config.tree_name;
		}
		void create_sub_node()
		{

		}
		std::string debug_info() const
		{
			return fmt::format("btree {} node {} child_idx {} extra {}", 
				btree_config.tree_name, node_config.idx, next_child_idx, meta::serialize::encode(node_config.extra));
		}
	};

	
	static node* create_node_by_idx(const btree_desc& btree_config, std::uint16_t node_idx, node* parent);
	class root :public node
	{
	public:
		void on_enter()
		{
			node::on_enter();
			visit_child(0);
		}
		void on_revisit()
		{
			node::on_revisit();
			if (!_parent)
			{
				on_enter();
			}
			else
			{
				bool result = children[0]->result;
				set_result(result);
			}
			
		}
	};
	class sequence :public node
	{
		void on_enter()
		{
			node::on_enter();
			next_child_idx = 0;
			visit_child(0);
		}
		void on_revisit()
		{
			node::on_revisit();
			if (!children[next_child_idx]->result)
			{
				set_result(false);
				return;
			}
			next_child_idx += 1;
			if (next_child_idx == children.size())
			{
				set_result(true);
				return;
			}
			visit_child(next_child_idx);
		}
	};
	class always_sequence :public sequence
	{
		void on_revisit()
		{
			node::on_revisit();

			next_child_idx += 1;
			if (next_child_idx == children.size())
			{
				set_result(true);
				return;
			}
			visit_child(next_child_idx);
		}
	};
	class random_sequence : public node
	{
	private:
		std::vector<std::uint16_t> _shuffle;
		
		void on_enter()
		{
			node::on_enter();
			if (_shuffle.empty())
			{
				_shuffle.reserve(children.size());
				for (int i = 0; i < children.size(); i++)
				{
					_shuffle.push_back(i);
				}
			}
			std::shuffle(_shuffle.begin(), _shuffle.end(), _generator);
			visit_child(_shuffle[0]);
		}
		void on_revisit()
		{
			node::on_revisit();
			if (!children[_shuffle[next_child_idx]]->result)
			{
				set_result(false);
				return;
			}
			next_child_idx += 1;
			if (next_child_idx == children.size())
			{
				set_result(true);
				return;
			}
			visit_child(_shuffle[next_child_idx]);
		}

	};
	class select : public node
	{
		void on_enter()
		{
			node::on_enter();
			next_child_idx = 0;
			visit_child(0);
		}
		void on_revisit()
		{
			node::on_revisit();
			if (children[next_child_idx]->result)
			{
				set_result(true);
				return;
			}
			next_child_idx += 1;
			if (next_child_idx == children.size())
			{
				set_result(false);
				return;
			}
			visit_child(next_child_idx);
		}
	};
	class probility : public node
	{
	private:
		std::vector<std::uint32_t> _probilities;
		
		void on_enter()
		{
			node::on_enter();
			if (_probilities.empty())
			{
				if (!init_prob_parameters())
				{
					_logger->warn("{} init_prob_parameters fail", debug_info());
					_probilities.clear();
					_agent->notify_stop();
					return;
				}
			}
			next_child_idx = prob_choose_child_idx();
			visit_child(next_child_idx);


		}
		bool init_prob_parameters()
		{
			auto prob_iter = node_config.extra.find("prob");
			if (prob_iter == node_config.extra.end())
			{
				return false;
			}
			if (!prob_iter->second.is_vector())
			{
				return false;
			}
			const meta::serialize::any_vector& prob_vec = std::get<meta::serialize::any_vector>(prob_iter->second);
			if (prob_vec.size() != children.size())
			{
				return false;
			}
			for (auto& one_prob : prob_vec)
			{
				if (!one_prob.is_int())
				{
					return false;
				}
				_probilities.push_back(std::get<int>(one_prob));
			}
			return true;
		}
		std::uint16_t prob_choose_child_idx() const
		{
			std::uint16_t prob_sum = std::accumulate(_probilities.begin(), _probilities.end(), 0);
			auto cur_choice = _distribution(_generator);
			std::uint16_t temp = cur_choice % prob_sum;
			for (int i = 0; i < children.size(); i++)
			{
				temp -= _probilities[0];
				if (temp <= 0)
				{
					return i;
				}
			}
			return 0;

		}
		void on_revisit()
		{
			node::on_revisit();
			set_result(children[next_child_idx]->result);
		}

	};
	class if_else : public node
	{
		void on_enter()
		{
			node::on_enter();
			visit_child(0);
		}
		void on_revisit()
		{
			switch (next_child_idx)
			{
			case 0:
				// the if node
				if (children[0]->result)
				{
					next_child_idx = 1;
				}
				else
				{
					next_child_idx = 2;
				}
				visit_child(next_child_idx);
				break;
			case 1:
				set_result(children[1]->result);
				break;
			case 2:
				set_result(children[2]->result);
				break;
			default:
				_logger->warn("{} invalid state visit if else node ", debug_info);
				break;
			}

		}
	};
	class while_loop : public node
	{
		void on_enter()
		{
			node::on_enter();
			visit_child(0);
		}
		void on_revisit()
		{
			switch (next_child_idx)
			{
			case 0:
				// the if node
				if (children[0]->result)
				{
					next_child_idx = 1;
					visit_child(next_child_idx);
				}
				else
				{
					set_result(false);
				}
				break;
			case 1:
				next_child_idx = 0;
				visit_child(next_child_idx);
				break;
			default:
				_logger->warn("{} invalid state visit while_node node ", debug_info);
				break;
			}
		}
	};
	class negative : public node
	{
		void on_enter()
		{
			node::on_enter();
			visit_child(0);
		}
		void on_revisit()
		{
			node::on_revisit();
			set_result(!children[0]->result);
		}
	};
	class always : public node
	{
		void on_enter()
		{
			node::on_enter();
			visit_child(0);
		}
		void on_revisit()
		{
			node::on_revisit();
			set_result(true);
		}
	};
	class sub_tree : public node
	{
		void on_enter()
		{
			node::on_enter();
			if (children.empty())
			{
				if (!create_sub_tree_node())
				{
					_logger->warn("{} fail to load sub tree", debug_info());
					_agent->notify_stop();
					return;
				}
				visit_child(0);
			}

		}
		bool create_sub_tree_node()
		{
			auto sub_tree_iter = node_config.extra.find("sub_tree");
			if (sub_tree_iter == node_config.extra.end())
			{
				return false;
			}
			if (!sub_tree_iter->second.is_str())
			{
				return false;
			}
			auto sub_tree_name = std::get<std::string>(sub_tree_iter->second);
			const btree_desc* cur_tree_desc = btree_desc::load(sub_tree_name);
			if (!cur_tree_desc)
			{
				return false;
			}
			auto new_root = create_node_by_idx(*cur_tree_desc, 0, this);
			if (!new_root)
			{
				return false;
			}
			children.push_back(new_root);
			return true;
		}
		void on_revisit()
		{
			set_result(children[0]->result);
		}

	};

	class parallel : public node
	{
		void on_enter()
		{
			node::on_enter();
			for (int i = 0; i < children.size(); i++)
			{
				visit_child(i);
			}
		}
		void on_revisit()
		{
			// 如果获得返回结果的那个节点
			for (auto one_child : children)
			{
				one_child->interrupt();
			}
			set_result(true);
		}
	};
	class action : public node
	{
		void on_enter()
		{
			node::on_enter();
		}
	};
}
