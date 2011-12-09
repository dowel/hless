#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

#include <string>

class MessageBox
{
public:
	MessageBox(std::string);

	/**
	 * Will show the message box with OK button.
	 */
	void show();

	/**
	 * Will show the message box for specified period of time, without OK button.
	 * 
	 * @param timeout 
	 */
	void show(int timeout);
};

#endif

