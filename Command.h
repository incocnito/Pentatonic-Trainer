#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <SFML/System/Vector2.hpp>

#include <functional>

class SceneNode;

struct Command
{
									Command();
	std::function<void(SceneNode&)>	action;
	unsigned						category;
};

#endif // COMMAND_H_INCLUDED
