#pragma once
#include "agent.h"
#include <algorithm>
#include <random>
namespace behavior
{
	enum class node_type
	{
		invalid = 0,
		root,
		negative,
		sequence,
		always_seq,
		random_seq,
		if_else,
		while_loop,
		wait_event,
		reset,
		sub_tree,
		parallel,
		action,
		always_true
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
	class node_creator;
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
		const node_idx_type _node_idx;
		const btree_desc& btree_config;
		const node_desc& node_config;
		std::shared_ptr<node_closure> _closure;
		std::shared_ptr<spdlog::logger> _logger;

		node(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
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
		void set_result(bool new_result);
		virtual bool handle_event(const event_type& cur_event)
		{
			return false;
		}
		bool ready() const
		{
			return _ready;
		}
		void visit();
		void create_children();
		virtual void on_enter();
		virtual void on_revisit();
		virtual void visit_child(node_idx_type child_idx);
		virtual void leave();
		virtual void interrupt();
		virtual void backtrace();
		const std::string& tree_name() const;
		std::string debug_info() const;
		friend class node_creator;
	};

	
	
	class root :public node
	{
		root(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class sequence :public node
	{
	protected:
		sequence(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class always_seq :public sequence
	{
	protected:
		always_seq(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			sequence(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		void on_revisit();
		friend class node_creator;
	};
	class random_seq : public node
	{
	protected:
		random_seq(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		std::vector<node_idx_type> _shuffle;
		
		void on_enter();
		void on_revisit();
		friend class node_creator;

	};
	class select : public node
	{
	protected:
		select(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class probility : public node
	{
		probility(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
	private:
		std::vector<std::uint32_t> _probilities;
		
		void on_enter();
		bool init_prob_parameters();
		node_idx_type prob_choose_child_idx() const;
		void on_revisit();
		friend class node_creator;

	};
	class if_else : public node
	{
		if_else(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class while_loop : public node
	{
		while_loop(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class negative : public node
	{
		negative(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class always_true : public node
	{
		always_true(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class sub_tree : public node
	{
		sub_tree(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		bool create_sub_tree_node();
		void on_revisit();
		friend class node_creator;

	};

	class parallel : public node
	{
		parallel(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		void on_enter();
		void on_revisit();
		friend class node_creator;
	};
	class action : public node
	{
		action(node* in_parent, node_idx_type in_node_idx, 
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		std::string action_name;
		meta::serialize::any_vector action_args;
		void on_enter();
		bool load_action_config();
		friend class node_creator;
	};
	class wait_event : public node
	{
		wait_event(node* in_parent, node_idx_type in_node_idx,
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		friend class node_creator;
	};
	class reset : public node
	{
		reset(node* in_parent, node_idx_type in_node_idx,
			const btree_desc& in_btree, node_type in_type) :
			node(in_parent, in_node_idx, in_btree, in_type)
		{

		}
		friend class node_creator;
	};
	class node_creator
	{
	public:
		static node* create_node_by_idx(const btree_desc& btree_config, 
			node_idx_type node_idx, node* parent);
	};
}
