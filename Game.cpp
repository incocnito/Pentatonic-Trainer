#include "Game.h"

Game::Game() :
    mWindow(sf::VideoMode(800, 600), "Pentatonic-Trainer 2.1.1", sf::Style::Close)
{
    addButton(50, 30, sf::Vector2f(220, 35));
    addButton(50, 30, sf::Vector2f(490, 35));
    addButton(200, 30, sf::Vector2f(560, 270));
    addButton(200, 30, sf::Vector2f(560, 230));
    addButton(200, 30, sf::Vector2f(560, 310));
    addButton(200, 30, sf::Vector2f(560, 350));
    mWindow.setFramerateLimit(30);
    #ifdef SOUND_ON
    addText(100, 100, "Loading Sounds...", sf::Color::White);
    mWindow.draw(mTextStack.top());
    mWindow.display();
    mPlayer.loadSounds();
    #endif
}

void Game::addButton(int width, int height, const sf::Vector2f& position)
{
    sf::RectangleShape button;
    button.setPosition(position);
	button.setSize(sf::Vector2f(width, height));
	button.setOutlineColor(sf::Color::Black);
	button.setOutlineThickness(2.f);
	mButtons.push_back(button);
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
        mPlayer.handleEvent(event, mButtons, commands);
        if(event.type == sf::Event::Closed ||
			(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			mWindow.close();
    }
    mPlayer.handleRealtimeInput(mWindow, mButtons, commands);
}

void Game::render()
{
    mWindow.clear();
    mWindow.draw(mWorld);
    if(mWorld.bassStateActive())
        mButtons[4].setOutlineColor(sf::Color::Blue);
    if(mPlayer.showingSolutionState())
        mButtons[2].setOutlineColor(sf::Color::Blue);
    if(mPlayer.isMute())
        mButtons[5].setOutlineColor(sf::Color::Blue);
    for(auto& button : mButtons)
        mWindow.draw(button);
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
    addText(230, 35, "[<-]", sf::Color::Black, 20);
    addText(500, 35, "[->]", sf::Color::Black, 20);
    addText(565, 270, "Show solution [S]", sf::Color::Black, 20);
    addText(565, 230, "Reset [R]", sf::Color::Black, 20);
    addText(565, 310, "Toggle bass mode [B]", sf::Color::Black, 20);
    addText(565, 350, "Mute [M]", sf::Color::Black, 20);
    addText(290, 35, mWorld.getActivePatternName(), sf::Color::Black, 20);
    if(mWorld.getActiveQuestionName() == "None")
        addText(280, 101, "You finished this pattern. Hooray!");
    else
        addText(280, 101, "Click on the " + mWorld.getActiveQuestionName() + " notes!");
    addText(590, 175, "Mistakes: " + std::to_string(mWorld.getTotalErrorCount()), sf::Color::Red, 20);
}

