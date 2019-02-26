#ifndef SquareNode_H_INCLUDED
#define SquareNode_H_INCLUDED

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Category.h"
#include "SceneNode.h"
#include "World.h"

#include <string>

class CircleNode : public SceneNode
{
private:
	sf::CircleShape     mCircle;
    sf::Text 			mText;
    sf::Vector2i		mGridPosition;
    unsigned            mCurrentQuestionState;
    bool                mGuessed;
    bool                mRootGuessed;
    int                 mErrorCount;
    std::string         mNoteName;
    bool                mIsShowingSolution;

public:
						CircleNode(const sf::Font& font, sf::Vector2f position, const unsigned& category, const std::string& noteName);

	virtual void		drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void		updateCurrent();

    void 				setText(const std::string& s);
    bool				contains(sf::Vector2f position) const;
    bool                correctGuess() const;
    bool                guessed() const;
    bool                rootGuessed() const;
    void                setGuessedToTrue();
    void                setQuestionState(const unsigned& state);
    void                resetStates();
    int                 getErrorCount() const;
    void                increaseErrorCount();
    void                showCircle();
    void                hideCircle();
    void                enableSolutionState();
    bool                isShowingSolution() const;
    unsigned            getCurrentQuestionState() const;

    std::string         getNoteName() const;

private:
    void 				draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void				align(sf::Vector2f position);
};

#endif // SquareNode_H_INCLUDED
