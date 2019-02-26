#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <functional>

#include "CircleNode.h"

class CommandQueue;
class World;

typedef std::vector<std::vector<std::tuple<sf::SoundBuffer, sf::Sound, std::string>>> Buffer;

class Player
{
private:
    //Helpterstruct to work with the sound files
    struct Note
    {
        Note(const std::string& name) :
            mName(name)
        { }
        std::string     mName;
        Note& operator++()
        {
            int number = static_cast<int>(mName[1]) - 48;
            if(++mName[1] == '6')
            {
                mName[1] = '0';
                switch(mName[0])
                {
                case 'X':
                    mName[0] = 'B';
                    break;
                case 'B':
                    mName[0] = 'G';
                    break;
                case 'G':
                    mName[0] = 'D';
                    break;
                case 'D':
                    mName[0] = 'A';
                    break;
                case 'A':
                    mName[0] = 'E';
                    break;
                case 'E':
                default:
                    break;
                }
            }
            return *this;
        }
    };

    bool                    mBassPitchActive;

private:
    void					createCommand(	const std::function<void(SceneNode&)>& action,
											unsigned category);
    #ifdef SOUND_ON
    sf::SoundBuffer         mWrongSoundBuffer;
    sf::Sound               mWrongSound;
    Buffer                  mNotes;
    #endif

public:
	Player();
	void	handleEvent(const sf::Event& event, std::vector<sf::RectangleShape>& buttons, CommandQueue& commands);
	void	handleRealtimeInput(const sf::RenderWindow& window, std::vector<sf::RectangleShape>& buttons, CommandQueue& commands);
	#ifdef SOUND_ON
	void    loadSounds();
	#endif

public:
    //List of commands
	void    selectCircle(const sf::Vector2f& mousePosition, CommandQueue& commands);
	void    showSolution(CommandQueue& commands);
	void    skipToNextPattern(CommandQueue& commands);
	void    skipToPreviousPattern(CommandQueue& commands);
	void    resetCurrentPattern(CommandQueue& commands);
	void    toggleBassState(CommandQueue& commands);
};

#endif // PLAYER_H_INCLUDED
