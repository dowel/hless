#include <curses.h>
#include <iostream>
#include <string>
#include <sstream>

#include "input.h"
#include "log.h"

static __attribute__((unused)) const char* MODULE_NAME = "input"; 

InputProcessor::InputProcessor()
{

}

InputProcessor::~InputProcessor()
{

}

void InputProcessor::register_input_sequence(InputSequence& seq, InputHandler handler)
{
	Log1("Asked to register key sequence: " << seq);

	InputTreeNode* node = &_root;
	InputTreeNode* temp;
	for (InputProcessor::InputSequence::iterator it = seq.begin(); it != seq.end(); it++) {
		temp = node->get_node(*it);
		if (temp == 0) {
			temp = new InputTreeNode();
			node->add_node(*it, temp);
		}
		node = temp;
	}

	node->set_handler(handler);
}

void InputProcessor::wait_for_input()
{
	InputSequence seq;
	int ch;
	InputTreeNode* node = &_root;
	InputTreeNode* temp;
	while (true) {
		ch = getch();
		Log1("Received input character " << ch);
		seq.push_back(ch);
		temp = node->get_node(ch);
		if (temp) {
			Log1("Calling handler for sequence: " << seq);
			if (temp->get_handler()) {
				(*temp)(ch);
				return;
			}
			node = temp;
		} else {
			node = &_root;
			seq.clear();
		}
	}
}

InputProcessor::InputTreeNode::InputTreeNode()
	: _handler(0)
{

}

InputProcessor::InputTreeNode::~InputTreeNode()
{

}

void InputProcessor::InputTreeNode::add_node(int key, InputTreeNode* node)
{
	if (_keys.find(key) == _keys.end()) {
		_keys.insert(std::make_pair(key, node));
	}
}

InputProcessor::InputTreeNode* InputProcessor::InputTreeNode::get_node(int key)
{
	InputTreeNodeKeys::iterator it = _keys.find(key);
	if (it != _keys.end()) {
		return it->second;
	}
	return 0;
}

void InputProcessor::InputTreeNode::operator()(char c)
{
	if (_handler != 0) {
		_handler(c);
	}
}

std::string InputProcessor::key_name(int ch)
{
	std::ostringstream os;

	switch (ch) {
	case KEY_CODE_YES  : os << "KEY_CODE_YES";   break;
	case KEY_BREAK     : os << "KEY_BREAK";	     break;
	case KEY_SRESET    : os << "KEY_SRESET";     break;
	case KEY_RESET     : os << "KEY_RESET";	     break;
	case KEY_DOWN      : os << "KEY_DOWN"; 	     break;
	case KEY_UP        : os << "KEY_UP";   	     break;
	case KEY_LEFT      : os << "KEY_LEFT"; 	     break;
	case KEY_RIGHT     : os << "KEY_RIGHT";	     break;
	case KEY_HOME      : os << "KEY_HOME"; 	     break;
	case KEY_BACKSPACE : os << "KEY_BACKSPACE";  break;
	case KEY_F0        : os << "KEY_F0";   	     break;
	case KEY_F(1)      : os << "KEY_F1";   	     break;
	case KEY_F(2)      : os << "KEY_F2";   	     break;
	case KEY_F(3)      : os << "KEY_F3";   	     break;
	case KEY_F(4)      : os << "KEY_F4";   	     break;
	case KEY_F(5)      : os << "KEY_F5";   	     break;
	case KEY_F(6)      : os << "KEY_F6";   	     break;
	case KEY_F(7)      : os << "KEY_F7";   	     break;
	case KEY_F(8)      : os << "KEY_F8";   	     break;
	case KEY_F(9)      : os << "KEY_F9";   	     break;
	case KEY_F(10)     : os << "KEY_F10";  	     break;
	case KEY_F(11)     : os << "KEY_F11";  	     break;
	case KEY_F(12)     : os << "KEY_F12";  	     break;
	case KEY_F(13)     : os << "KEY_F13";  	     break;
	case KEY_F(14)     : os << "KEY_F14";  	     break;
	case KEY_DL        : os << "KEY_DL";   	     break;
	case KEY_IL        : os << "KEY_IL";   	     break;
	case KEY_DC        : os << "KEY_DC";   	     break;
	case KEY_IC        : os << "KEY_IC";   	     break;
	case KEY_EIC       : os << "KEY_EIC";  	     break;
	case KEY_CLEAR     : os << "KEY_CLEAR";	     break;
	case KEY_EOS       : os << "KEY_EOS";  	     break;
	case KEY_EOL       : os << "KEY_EOL";  	     break;
	case KEY_SF        : os << "KEY_SF";   	     break;
	case KEY_SR        : os << "KEY_SR";   	     break;
	case KEY_NPAGE     : os << "KEY_NPAGE";	     break;
	case KEY_PPAGE     : os << "KEY_PPAGE";	     break;
	case KEY_STAB      : os << "KEY_STAB"; 	     break;
	case KEY_CTAB      : os << "KEY_CTAB"; 	     break;
	case KEY_CATAB     : os << "KEY_CATAB";	     break;
	case KEY_ENTER     : os << "KEY_ENTER";	     break;
	case KEY_PRINT     : os << "KEY_PRINT";	     break;
	case KEY_LL        : os << "KEY_LL";   	     break;
	case KEY_A1        : os << "KEY_A1";   	     break;
	case KEY_A3        : os << "KEY_A3";   	     break;
	case KEY_B2        : os << "KEY_B2";   	     break;
	case KEY_C1        : os << "KEY_C1";   	     break;
	case KEY_C3        : os << "KEY_C3";   	     break;
	case KEY_BTAB      : os << "KEY_BTAB"; 	     break;
	case KEY_BEG       : os << "KEY_BEG";  	     break;
	case KEY_CANCEL    : os << "KEY_CANCEL";     break;
	case KEY_CLOSE     : os << "KEY_CLOSE";	     break;
	case KEY_COMMAND   : os << "KEY_COMMAND";    break;
	case KEY_COPY      : os << "KEY_COPY"; 	     break;
	case KEY_CREATE    : os << "KEY_CREATE";     break;
	case KEY_END       : os << "KEY_END";  	     break;
	case KEY_EXIT      : os << "KEY_EXIT"; 	     break;
	case KEY_FIND      : os << "KEY_FIND"; 	     break;
	case KEY_HELP      : os << "KEY_HELP"; 	     break;
	case KEY_MARK      : os << "KEY_MARK"; 	     break;
	case KEY_MESSAGE   : os << "KEY_MESSAGE";    break;
	case KEY_MOVE      : os << "KEY_MOVE"; 	     break;
	case KEY_NEXT      : os << "KEY_NEXT"; 	     break;
	case KEY_OPEN      : os << "KEY_OPEN"; 	     break;
	case KEY_OPTIONS   : os << "KEY_OPTIONS";    break;
	case KEY_PREVIOUS  : os << "KEY_PREVIOUS";   break;
	case KEY_REDO      : os << "KEY_REDO"; 	     break;
	case KEY_REFERENCE : os << "KEY_REFERENCE";  break;
	case KEY_REFRESH   : os << "KEY_REFRESH";    break;
	case KEY_REPLACE   : os << "KEY_REPLACE";    break;
	case KEY_RESTART   : os << "KEY_RESTART";    break;
	case KEY_RESUME    : os << "KEY_RESUME";     break;
	case KEY_SAVE      : os << "KEY_SAVE"; 	     break;
	case KEY_SBEG      : os << "KEY_SBEG"; 	     break;
	case KEY_SCANCEL   : os << "KEY_SCANCEL";    break;
	case KEY_SCOMMAND  : os << "KEY_SCOMMAND";   break;
	case KEY_SCOPY     : os << "KEY_SCOPY";	     break;
	case KEY_SCREATE   : os << "KEY_SCREATE";    break;
	case KEY_SDC       : os << "KEY_SDC";  	     break;
	case KEY_SDL       : os << "KEY_SDL";  	     break;
	case KEY_SELECT    : os << "KEY_SELECT";     break;
	case KEY_SEND      : os << "KEY_SEND"; 	     break;
	case KEY_SEOL      : os << "KEY_SEOL"; 	     break;
	case KEY_SEXIT     : os << "KEY_SEXIT";	     break;
	case KEY_SFIND     : os << "KEY_SFIND";	     break;
	case KEY_SHELP     : os << "KEY_SHELP";	     break;
	case KEY_SHOME     : os << "KEY_SHOME";	     break;
	case KEY_SIC       : os << "KEY_SIC";  	     break;
	case KEY_SLEFT     : os << "KEY_SLEFT";	     break;
	case KEY_SMESSAGE  : os << "KEY_SMESSAGE";   break;
	case KEY_SMOVE     : os << "KEY_SMOVE";	     break;
	case KEY_SNEXT     : os << "KEY_SNEXT";	     break;
	case KEY_SOPTIONS  : os << "KEY_SOPTIONS";   break;
	case KEY_SPREVIOUS : os << "KEY_SPREVIOUS";  break;
	case KEY_SPRINT    : os << "KEY_SPRINT";     break;
	case KEY_SREDO     : os << "KEY_SREDO";	     break;
	case KEY_SREPLACE  : os << "KEY_SREPLACE";   break;
	case KEY_SRIGHT    : os << "KEY_SRIGHT";     break;
	case KEY_SRSUME    : os << "KEY_SRSUME";     break;
	case KEY_SSAVE     : os << "KEY_SSAVE";	     break;
	case KEY_SSUSPEND  : os << "KEY_SSUSPEND";   break;
	case KEY_SUNDO     : os << "KEY_SUNDO";	     break;
	case KEY_SUSPEND   : os << "KEY_SUSPEND";    break;
	case KEY_UNDO      : os << "KEY_UNDO"; 	     break;
	case KEY_MOUSE     : os << "KEY_MOUSE";	     break;
	case KEY_RESIZE    : os << "KEY_RESIZE";     break;
	case KEY_EVENT     : os << "KEY_EVENT";	     break;
	default:
		if (isprint(ch)) {
			os << "'" << static_cast<char>(ch) << "'";
		} else if (ch == 27) {
			os << "ESC";
		} else {
			os << "KEY(" << ch << ")";
		}
	}

	std::string s(os.str());
	return s;
}

std::ostream& operator<<(std::ostream& os, InputProcessor::InputSequence& seq)
{
	bool first = true;
	for (InputProcessor::InputSequence::iterator it = seq.begin(); it != seq.end(); it++)
	{
		if (!first) {
			os << ", ";
		}
		first = false;
		os << InputProcessor::key_name(*it);
	}

	return os;
}

