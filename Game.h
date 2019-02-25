#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SFML/System/Noncopyable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "World.h"
#include "Player.h"

#include <stack>

class Game : private sf::NonCopyable
{
private:
    sf::RenderWindow        mWindow;
    World                   mWorld;
    Player                  mPlayer;
    std::stack<sf::Text>    mTextStack;

private:
    void                    update();
    void                    processInput();
    void                    render();
    void					addText(float x, float y, const std::string& text, const sf::Color& color = sf::Color::Black, int characterSize = 15);
    void                    updateHeadline();

public:
                            Game();
    void                    run();
};

#endif // GAME_H_INCLUDED
