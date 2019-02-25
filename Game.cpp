#include "Game.h"

Game::Game() :
    mWindow(sf::VideoMode(800, 600), "Pentatonic-Trainer 1.0", sf::Style::Close)
{
    mWindow.setFramerateLimit(30);
    #ifdef SOUND_ON
    addText(100, 100, "Loading Sounds...", sf::Color::White);
    mWindow.draw(mTextStack.top());
    mWindow.display();
    mPlayer.loadSounds();
    #endif
}

void Game::update()
{
    updateHeadline();
    mWorld.update();
}

void Game::processInput()
{
    CommandQueue& commands = mWorld.getCommandQueue();
    sf::Event event;
    while(mWindow.pollEvent(event))
    {
        mPlayer.handleEvent(event, commands);
        if(event.type == sf::Event::Closed ||
			(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			mWindow.close();
    }
    mPlayer.handleRealtimeInput(mWindow, commands);
}

void Game::render()
{
    mWindow.clear();
    mWindow.draw(mWorld);
    while(!mTextStack.empty())
    {
        mWindow.draw(mTextStack.top());
        mTextStack.pop();
    }
    mWindow.display();
}

void Game::run()
{
	while(mWindow.isOpen())
    {
        processInput();
        update();
        render();
    }
}

void Game::addText(float x, float y, const std::string& string, const sf::Color& color, int characterSize)
{
	static sf::Text text;
	text.setFont(mWorld.getFont());
	text.setCharacterSize(characterSize);
	text.setFillColor(color);
	text.setPosition(x, y);
	text.setString(string);
	mTextStack.push(text);
}

void Game::updateHeadline()
{
    addText(217, 35, mWorld.getActivePatternName(), sf::Color::Black, 20);
    addText(227, 101, "Click on the " + mWorld.getActiveQuestionName() + " notes!");
    addText(591, 177, "Fehler: " + std::to_string(mWorld.getTotalErrorCount()), sf::Color::Red, 20);
}

