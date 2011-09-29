#ifndef __INPUT_H__
#define __INPUT_H__

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <list>
#include <map>

#define KEY_HANDLER(x) boost::function<void (char)>(boost::bind(&x, this, _1))

class InputProcessor
{
public:
	InputProcessor();
	~InputProcessor();

	typedef boost::function<void (char)> InputHandler;
	typedef std::list<int> InputSequence;

	class InputTreeNode
	{
	public:
		InputTreeNode();
		~InputTreeNode();

		void add_node(int key, InputTreeNode* node);
		InputTreeNode* get_node(int key);
		void set_handler(InputProcessor::InputHandler handler)
		{
			_handler = handler;
		}

		InputProcessor::InputHandler get_handler()
		{
			return _handler;
		}
		void operator()(char c);

		typedef std::map<int, InputTreeNode*> InputTreeNodeKeys;

	private:
		InputTreeNodeKeys _keys;
		InputProcessor::InputHandler _handler;
	};

	void register_input_sequence(InputSequence& seq, InputHandler handler);
	void wait_for_input();
	static std::string key_name(int ch);

private:
	InputTreeNode _root;
};

std::ostream& operator<<(std::ostream& os, InputProcessor::InputSequence& seq);

#endif

