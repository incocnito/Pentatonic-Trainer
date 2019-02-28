#include "CircleNode.h"

CircleNode::CircleNode(const sf::Font& font, sf::Vector2f position, const unsigned& category, const std::string& noteName) :
	mGuessed(false),
	mRootGuessed(false),
	mErrorCount(0),
	mNoteName(noteName),
	mIsShowingSolution(false)
{
    mCircle.setPosition(sf::Vector2f(position.x+5, position.y-1));
	mCircle.setRadius(13);
	mCircle.setFillColor(sf::Color::Transparent);
	mCircle.setOutlineColor(sf::Color::Green);

	setCategory(category);

	mText.setFont(font);
	mText.setCharacterSize(20);
	mText.setFillColor(sf::Color::White);
	mText.setPosition(sf::Vector2f(position.x + 12, position.y-1));
}

void CircleNode::setText(const std::string& s)
{
	mText.setString(s);
}

void CircleNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mCircle);
	target.draw(mText);
}

bool CircleNode::contains(sf::Vector2f position) const
{
	return mCircle.getGlobalBounds().contains(position);
}

void CircleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mCircle, states);
	target.draw(mText);
}

void CircleNode::updateCurrent()
{

}

void CircleNode::align(sf::Vector2f position)
{

}

bool CircleNode::correctGuess() const
{
    if(mGuessed && (mCurrentQuestionState & (Category::Pentatonic | Category::Diatonic)))
        return false;
    if(mRootGuessed && (mCurrentQuestionState & (Category::MajorRoot | Category::MinorRoot)))
       return false;
    if(mCurrentQuestionState == getCategory() ||
       (getCategory() & (Category::MajorRoot | Category::MinorRoot)) && mCurrentQuestionState == Category::Pentatonic)
        return true;
    return false;
}

bool CircleNode::guessed() const
{
    return mGuessed;
}

bool CircleNode::rootGuessed() const
{
    return mRootGuessed;
}

void CircleNode::setGuessedToTrue()
{
    if(mCurrentQuestionState == Category::MajorRoot && getCategory() & Category::MajorRoot ||
       mCurrentQuestionState == Category::MinorRoot && getCategory() & Category::MinorRoot)
       mRootGuessed = true;

    mGuessed = true;
    if(getCategory() & Category::Pentatonic || getCategory() & Category::MajorRoot || getCategory() & Category::MinorRoot)
        mCircle.setFillColor(sf::Color::Black);
    else if(getCategory() & Category::Diatonic)
        mCircle.setFillColor(sf::Color::Red);
    if(mRootGuessed)
    {
        if(getCategory() & Category::MajorRoot)
            setText("C");
        else if(getCategory() & Category::MinorRoot)
            setText("A");
    }
}

void CircleNode::setQuestionState(const unsigned& state)
{
    mCurrentQuestionState = state;
}

void CircleNode::resetStates()
{
    mGuessed = false;
    mRootGuessed = false;
    mIsShowingSolution = false;
    setText("");
    mCircle.setFillColor(sf::Color::Transparent);
    mErrorCount = 0;
}

void CircleNode::increaseErrorCount()
{
    ++mErrorCount;
}

int CircleNode::getErrorCount() const
{
    return mErrorCount;
}

void CircleNode::showCircle()
{
    mCircle.setOutlineThickness(2);
}

void CircleNode::hideCircle()
{
    mCircle.setOutlineThickness(0);
}

std::string CircleNode::getNoteName() const
{
    return mNoteName;
}

void CircleNode::enableSolutionState()
{
    mIsShowingSolution = true;
    mGuessed = true;
    mRootGuessed = true;
    setGuessedToTrue();
}

bool CircleNode::isShowingSolution() const
{
    return mIsShowingSolution;
}

unsigned CircleNode::getCurrentQuestionState() const
{
    return mCurrentQuestionState;
}
