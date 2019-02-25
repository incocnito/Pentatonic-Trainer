#include "Player.h"
#include "Command.h"
#include "CommandQueue.h"
#include "CircleNode.h"

#include <tuple>

Player::Player()
{

}

void Player::handleRealtimeInput(const sf::RenderWindow& window, CommandQueue& commands)
{
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
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
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.button == sf::Mouse::Left)
            selectCircle(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), commands);
    }
    else if(event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::A)
            showSolution(commands);
        else if(event.key.code == sf::Keyboard::N)
            skipToNextPattern(commands);
        else if(event.key.code == sf::Keyboard::R)
            resetCurrentPattern(commands);
    }
}

void Player::selectCircle(const sf::Vector2i& mousePosition, CommandQueue& commands)
{
    Command command;
    command.category = Category::All;
    command.action = [=] (SceneNode& s)
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
                    if(!node->isInPauseState())
                    {
                        for(int i = 0; i < 6; ++i)
                        {
                            for(int j = 0; j < 6; ++j)
                            {
                                std::get<1>(mNotes[i][j]).stop();
                                if(std::get<2>(mNotes[i][j]) == node->getNoteName())
                                    std::get<1>(mNotes[i][j]).play();
                            }
                        }
                    }
                    #endif
                }

                else
                {
                    node->increaseErrorCount();
                    #ifdef SOUND_ON
                    if(!node->isInPauseState())
                        mWrongSound.play();
                    #endif
                }
            }
        }
    };
    commands.push(command);
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
    Command command;
    command.category = Category::All;
    command.action = [=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
            node->setPauseState();
    };
    commands.push(command);
}

void Player::skipToNextPattern(CommandQueue& commands)
{
    Command command;
    command.category = Category::All;
    command.action = [=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
        {
            node->skip();
        }

    };
    commands.push(command);
}

void Player::resetCurrentPattern(CommandQueue& commands)
{
    Command command;
    command.category = Category::All;
    command.action = [=] (SceneNode& s)
    {
        CircleNode* node = dynamic_cast<CircleNode*>(&s);
        if(node != nullptr)
        {
            node->resetStates();
        }

    };
    commands.push(command);
}
