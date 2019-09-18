#include "agent.h"
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
		enter, // first visit
		revisit,
		waiting,
		leave,
		finish,
		destroy
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
	class node
	{
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

		void set_result(bool new_result)
		{
			if (_state != node_state::waiting)
			{
				return;
			}
			result = new_result;
			_closure.reset();
			_agent->_fronts.push_back(this);
			_state = node_state::finish;
			_agent->poll();
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
			if (!node_config.children.empty() && children.empty())
			{
				// 初始化所有的子节点
				for (auto one_child_idx : node_config.children)
				{
					children.push_back(create_node_by_idx(btree_config, one_child_idx));
				}
			}
			switch (_state)
			{
			case bahavior::node_state::init:
				break;
			case bahavior::node_state::enter:
				on_enter();
				break;
			case bahavior::node_state::revisit:
				on_revisit();
				break;
			case bahavior::node_state::waiting:
				_logger->warn("btree {} visit node {} while state is waiting", btree_config.tree_name, node_config.idx);
				break;
			case bahavior::node_state::leave:
				break;
			case bahavior::node_state::finish:
				break;
			case bahavior::node_state::destroy:
				_logger->warn("btree {} visit node {} while state is destroy", btree_config.tree_name, node_config.idx);
				break;
			default:
				break;
			}


		}
		virtual void on_enter()
		{

		}
		virtual void on_revisit()
		{

		}
		virtual void interrupt()
		{
			if (_closure)
			{
				_closure.reset();
			}
			_state = node_state::finish;
			next_child_idx = 0;
		}
		const std::string& tree_name() const
		{
			return btree_config.tree_name;
		}
		void notify_stop()
		{
			_logger->warn("notify stop at tree {} node {}", btree_config.tree_name, node_config.idx);
		}
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
	static node* create_node_by_idx(const btree_desc& btree_config, std::uint16_t node_idx);
}
