#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio.hpp>

#include "CommandQueue.h"
#include "SceneNode.h"
#include "CircleNode.h"

#include <algorithm>
#include <ctime>
#include <cstdlib>

typedef std::vector<std::vector<unsigned>> Vec2D;
typedef std::vector<std::vector<std::string>> Notes;

const int FIELD_SIZE = 6;

class World : public sf::Drawable, private sf::NonCopyable
{
public:
    struct Pattern
    {
        Pattern(const Vec2D& categoryInformation, const std::string& name) :
            mCategoryInformation(categoryInformation),
            mName(name)
        {
            using namespace Category;
            numberOfPentatonicNotes = 0;
            numberOfDiatonicNotes = 0;
            numberOfMajorRoots = 0;
            numberOfMinorRoots = 0;

            for(int i = 0; i < FIELD_SIZE; ++i)
            {
                for(int j = 0; j < FIELD_SIZE; ++j)
                {
                    unsigned cat = mCategoryInformation[i][j];
                    if(cat & Pentatonic || cat & MajorRoot || cat & MinorRoot)
                        ++numberOfPentatonicNotes;
                    if(cat & Diatonic)
                        ++numberOfDiatonicNotes;
                    if(cat & MajorRoot)
                        ++numberOfMajorRoots;
                    if(cat & MinorRoot)
                        ++numberOfMinorRoots;

                }
            }
        }
        Vec2D                   mCategoryInformation;
        std::string             mName;
        unsigned                numberOfPentatonicNotes;
        unsigned                numberOfDiatonicNotes;
        unsigned                numberOfMajorRoots;
        unsigned                numberOfMinorRoots;
    };

private:
    SceneNode                       mSceneGraph;
    CommandQueue                    mCommandQueue;

    sf::Sprite                      mBackgroundSprite;
    sf::Texture                     mBackgroundTexture;
    sf::Font                        mFont;

    std::vector<Pattern>            mPatterns;
    Notes                           mNotePattern;
    unsigned                        mCurrentPatternIndex;
    unsigned                        mCurrentQuestionState;
    int                             mTotalErrorCount;

public:
                                    World();
    void                            update();
    CommandQueue&                   getCommandQueue();

    const sf::Font&                 getFont() const;
    std::string                     getActiveQuestionName() const;
    std::string                     getActivePatternName() const;
    int                             getTotalErrorCount() const;

private:
    void                            draw(sf::RenderTarget& target, sf::RenderStates states) const;
    //in zukunft: kein festes 6x6 feld, sondern jedes feld wird mit dem pattern erzeugt und bei einem
    //neuen pattern ersetzt
    void                            loadPatternsFromFolder(const std::string& path);
    void                            addGameField(const sf::Vector2f& rootPosition);
    void                            setNextPatternActive();
    void                            initializePatterns();
    void                            updateQuestionState();

};

#endif // WORLD_H_INCLUDED
