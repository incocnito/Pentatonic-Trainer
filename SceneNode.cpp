#include "SceneNode.h"
#include "Category.h"
#include "Command.h"
#include "CircleNode.h"

SceneNode::SceneNode() :
	mParent(nullptr)
{

}

void SceneNode::attachChild(Ptr child)
{
	child->mParent = this;
	mChildren.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node)
{
	auto found = std::find_if(mChildren.begin(), mChildren.end(),
							  [&] (const Ptr& child) -> bool { return child.get() == &node; } );
	Ptr result = std::move(*found);
	result->mParent = nullptr;
	mChildren.erase(found);
	return result;
}

void SceneNode::update()
{
	updateCurrent();
	updateChildren();
}

void SceneNode::updateCurrent()
{

}

void SceneNode::updateChildren()
{
	for(auto& i : mChildren)
		i->update();
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	drawCurrent(target, states);
	drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{

}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for(auto& i : mChildren)
		i->draw(target, states);
}

unsigned SceneNode::getCategory() const
{
	return mCategory;
}

void SceneNode::setCategory(const unsigned& category)
{
    mCategory = category;
}

void SceneNode::onCommand(const Command& command)
{
	if(command.category & getCategory())
		command.action(*this);
	for(auto& i : mChildren)
		i->onCommand(command);
}

const std::vector<SceneNode::Ptr>& SceneNode::getTree() const
{
	return mChildren;
}

