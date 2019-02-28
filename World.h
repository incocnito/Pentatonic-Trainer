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
#include "CommunicatorNode.h"

#include <algorithm>
#include <ctime>
#include <cstdlib>

typedef std::vector<std::vector<unsigned>> Vec2D;
typedef std::vector<std::vector<std::string>> Notes;

class World : public sf::Drawable, private sf::NonCopyable
{
public:
    struct IterationLimits
    {
        IterationLimits()
        {
            setGuitarLimits();
        }

        unsigned xMin;
        unsigned xMax;
        unsigned yMin;
        unsigned yMax;
        void setGuitarLimits()
        {
            xMin = 0;
            xMax = 6;
            yMin = 0;
            yMax = 6;
        }
        void setBassLimits()
        {
            xMin = 0;
            xMax = 6;
            yMin = 2;
            yMax = 6;
        }
    };

    struct Pattern
    {
        Pattern(const Vec2D& categoryInformation, const std::string& name, const IterationLimits& limits) :
            mCategoryInformation(categoryInformation),
            mName(name)
        {
            countNotes(limits);
        }
        void countNotes(const IterationLimits& limits)
        {
            using namespace Category;
            numberOfPentatonicNotes = 0;
            numberOfDiatonicNotes = 0;
            numberOfMajorRoots = 0;
            numberOfMinorRoots = 0;

            for(unsigned i = limits.yMin; i < limits.yMax; ++i)
            {
                for(unsigned j = limits.xMin; j < limits.xMax; ++j)
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
    CommunicatorNode*               mCommunicator;
    CommandQueue                    mCommandQueue;

    sf::Sprite                      mBackgroundSprite;
    sf::Texture                     mBackgroundTexture;
    sf::Font                        mFont;

    IterationLimits                 mIterationLimits;

    std::vector<Pattern>            mPatterns;
    Notes                           mNotePattern;
    unsigned                        mCurrentPatternIndex;
    unsigned                        mCurrentQuestionState;
    int                             mTotalErrorCount;
    bool                            mBassStateActive;

public:
                                    World();
    void                            update();
    CommandQueue&                   getCommandQueue();

    const sf::Font&                 getFont() const;
    std::string                     getActiveQuestionName() const;
    std::string                     getActivePatternName() const;
    int                             getTotalErrorCount() const;
    void                            toggleBassState();
    IterationLimits                 getIterationLimits() const;
    bool                            bassStateActive() const;

private:
    void                            draw(sf::RenderTarget& target, sf::RenderStates states) const;
    //in zukunft: kein festes 6x6 feld, sondern jedes feld wird mit dem pattern erzeugt und bei einem
    //neuen pattern ersetzt
    void                            loadPatternsFromFolder(const std::string& path);
    void                            addGameField();
    void                            initializePatterns();
    void                            updateQuestionState();
    void                            showNextPattern();
    void                            showPreviousPattern();
    void                            checkCommunicator();
    void                            copyCurrentPatternToNodes(bool reset);

};

#endif // WORLD_H_INCLUDED
