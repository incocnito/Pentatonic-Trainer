#ifndef SCENENODE_H_INCLUDED
#define SCENENODE_H_INCLUDED

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Drawable.hpp>

struct Command;

#include <vector>
#include <memory>

class SceneNode : private sf::NonCopyable, public sf::Drawable
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;

public:
							SceneNode();

	void 					attachChild(Ptr child);
	Ptr 					detachChild(const SceneNode& node);

	void					update();
	virtual unsigned		getCategory() const;
	void					onCommand(const Command& command);
	void                    setCategory(const unsigned& category);
	void                    clearTree();

	const std::vector<Ptr>&	getTree() const;

private:
	virtual void			updateCurrent();
	void					updateChildren();

	virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void					drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	std::vector<Ptr>		mChildren;
	SceneNode*				mParent;
	unsigned                mCategory;
};

#endif // SCENENODE_H_INCLUDED
