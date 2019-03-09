#include "Player.h"
#include "Command.h"
#include "CommandQueue.h"
#include "CircleNode.h"

#include <tuple>

Player::Player() :
    mBassPitchActive(false),
    mShowingSolution(false),
    mMute(false)
{

}

void Player::handleRealtimeInput(const sf::RenderWindow& window, std::vector<sf::RectangleShape>& buttons, CommandQueue& commands)
{
    sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
    Command command;
    command.category = Category::All;
    command.action = [=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
        {
            if(node->contains(mousePosition))
                node->showCircle();
            else
                node->hideCircle();
        }
    };
    commands.push(command);
    for(unsigned i = 0; i < buttons.size(); ++i)
    {
        if(buttons[i].getGlobalBounds().contains(mousePosition))
            buttons[i].setOutlineColor(sf::Color::Blue);
        else
            buttons[i].setOutlineColor(sf::Color::Black);
    }
}

void Player::handleEvent(const sf::Event& event, std::vector<sf::RectangleShape>& buttons, CommandQueue& commands)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f position = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            selectCircle(position, commands);
            for(unsigned i = 0; i < buttons.size(); ++i)
            {
                if(buttons[i].getGlobalBounds().contains(position))
                {
                    switch(i)
                    {
                    case 0:
                        skipToPreviousPattern(commands);
                        break;
                    case 1:
                        skipToNextPattern(commands);
                        break;
                    case 2:
                        showSolution(commands);
                        break;
                    case 3:
                        resetCurrentPattern(commands);
                        break;
                    case 4:
                        toggleBassState(commands);
                        break;
                    case 5:
                        muteSounds();
                        break;
                    }
                }
            }
        }
    }
    else if(event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::S)
            showSolution(commands);
        else if(event.key.code == sf::Keyboard::Right)
            skipToNextPattern(commands);
        else if(event.key.code == sf::Keyboard::Left)
            skipToPreviousPattern(commands);
        else if(event.key.code == sf::Keyboard::R)
            resetCurrentPattern(commands);
        else if(event.key.code == sf::Keyboard::B)
            toggleBassState(commands);
        else if(event.key.code == sf::Keyboard::M)
            muteSounds();
    }
}

void Player::createCommand(const std::function<void(SceneNode&)>& action, unsigned category, CommandQueue& commands)
{
	Command command;
	command.category = category;
	command.action = action;
	commands.push(command);
}

bool Player::isMute() const
{
    return mMute;
}

void Player::selectCircle(const sf::Vector2f& mousePosition, CommandQueue& commands)
{
    createCommand(
    [=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
        {
            if(node->contains(mousePosition))
            {
                if(node->correctGuess())
                {
                    node->setGuessedToTrue();
                    #ifdef SOUND_ON
                    if(!node->isShowingSolution() && node->getCurrentQuestionState() != Category::None)
                    {
                        for(int i = 0; i < 6; ++i)
                        {
                            for(int j = 0; j < 6; ++j)
                            {
                                std::get<1>(mNotes[i][j]).stop();
                                if(std::get<2>(mNotes[i][j]) == node->getNoteName() && !mMute)
                                    std::get<1>(mNotes[i][j]).play();
                            }
                        }
                    }
                    #endif
                }

                else
                {
                    if(node->getCurrentQuestionState() != Category::None)
                        node->increaseErrorCount();
                    #ifdef SOUND_ON
                    if(!node->isShowingSolution() && node->getCurrentQuestionState() != Category::None && !mMute)
                        mWrongSound.play();
                    #endif
                }
            }
        }
    }, Category::All, commands);
}

#ifdef SOUND_ON
void Player::loadSounds()
{
    mWrongSoundBuffer.loadFromFile("resources/falsch.wav");
    mWrongSound.setBuffer(mWrongSoundBuffer);
    Note start("X0");
    mNotes.resize(6);
    for(int i = 0; i < 6; ++i)
    {
        mNotes[i].resize(6);
        for(int j = 0; j < 6; ++j)
        {
            std::get<0>(mNotes[i][j]).loadFromFile("resources/noten/"+start.mName+".wav");
            std::get<1>(mNotes[i][j]).setBuffer(std::get<0>(mNotes[i][j]));
            std::get<2>(mNotes[i][j]) = start.mName;
            ++start;
        }
    }
}
#endif

void Player::showSolution(CommandQueue& commands)
{
    mShowingSolution = !mShowingSolution;
    if(!mShowingSolution)
    {
        resetCurrentPattern(commands);
        return;
    }
    createCommand([=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
            node->enableSolutionState();
    }, Category::All | Category::End, commands);
}

void Player::skipToNextPattern(CommandQueue& commands)
{
    createCommand([=] (SceneNode& s)
    {
        CommunicatorNode* node = dynamic_cast<CommunicatorNode*>(&s);
        if(node != nullptr)
            node->mShowNextPatternState = true;
    }, Category::Communicator, commands);
}

void Player::skipToPreviousPattern(CommandQueue& commands)
{
    createCommand([=] (SceneNode& s)
    {
        CommunicatorNode* node = dynamic_cast<CommunicatorNode*>(&s);
        if(node != nullptr)
            node->mShowPreviousPatternState = true;
    }, Category::Communicator, commands);
}

void Player::muteSounds()
{
    mMute = !mMute;
    if(mMute)
    {
        for(int i = 0; i < 6; ++i)
        {
            for(int j = 0; j < 6; ++j)
                std::get<1>(mNotes[i][j]).stop();
        }
    }
}

void Player::resetCurrentPattern(CommandQueue& commands)
{
    mShowingSolution = false;
    createCommand([=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
            node->resetStates();
    }, Category::All, commands);
    createCommand([=] (SceneNode& s)
    {
        CommunicatorNode* node = dynamic_cast<CommunicatorNode*>(&s);
        if(node != nullptr)
            node->mResetCurrentPatternState = true;
    }, Category::Communicator, commands);
}

void Player::toggleBassState(CommandQueue& commands)
{
    mBassPitchActive = !mBassPitchActive;
    createCommand([=] (SceneNode& s)
    {
        CommunicatorNode* node = dynamic_cast<CommunicatorNode*>(&s);
        if(node != nullptr)
            node->mToggleBassState = true;
    }, Category::Communicator, commands);
    #ifdef SOUND_ON
    for(int i = 0; i < 6; ++i)
    {
        for(int j = 0; j < 6; ++j)
        {
            std::get<1>(mNotes[i][j]).setPitch(mBassPitchActive? 0.5 : 1);
            std::get<1>(mNotes[i][j]).stop();
        }
    }
    #endif
}

bool Player::showingSolutionState() const
{
    return mShowingSolution;
}
