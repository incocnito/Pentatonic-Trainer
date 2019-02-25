#ifndef COMMANDQUEUE_H_INCLUDED
#define COMMANDQUEUE_H_INCLUDED

#include "Command.h"

#include <memory>
#include <queue>

class CommandQueue
{
public:
	void			push(const Command& command);
	Command			pop();
	bool			isEmpty() const;

private:
	std::queue<Command> mQueue;
};

#endif // COMMANDQUEUE_H_INCLUDED
