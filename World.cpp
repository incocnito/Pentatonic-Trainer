#include "World.h"
#include "Category.h"

World::World() :
    mCurrentPatternIndex(0),
    mCurrentQuestionState(Category::Pentatonic),
    mTotalErrorCount(0),
    mNotePattern
    {
        { "X0", "X1", "X2", "X3", "X4", "X5" },
        { "B0", "B1", "B2", "B3", "B4", "B5" },
        { "G0", "G1", "G2", "G3", "G4", "G5" },
        { "D0", "D1", "D2", "D3", "D4", "D5" },
        { "A0", "A1", "A2", "A3", "A4", "A5" },
        { "E0", "E1", "E2", "E3", "E4", "E5" }
    },
    mBassStateActive(false)
{

	mFont.loadFromFile("resources/arial.ttf");
    mBackgroundTexture.loadFromFile("resources/Griffbrett.png");
    mBackgroundSprite.setTexture(mBackgroundTexture);

    initializePatterns();

    addGameField();
}

void World::addGameField()
{
    mSceneGraph.clearTree();
    sf::Vector2f rootPosition(250, 175);
    if(mBassStateActive)
        rootPosition.y = 243;
    sf::Vector2f currentPosition = rootPosition;

    for(int i = mIterationLimits.yMin; i < mIterationLimits.yMax; ++i)
    {
        for(int j = mIterationLimits.xMin; j < mIterationLimits.xMax; ++j)
        {
            std::unique_ptr<CircleNode> square(new CircleNode(mFont, currentPosition, Category::None, mNotePattern[i][j]));
            mSceneGraph.attachChild(std::move(std::unique_ptr<CircleNode>(std::move(square))));
            if(j == 0)
                currentPosition.x = 282;
            else
                currentPosition.x += 37;
        }
        currentPosition.y += 34;
        currentPosition.x = rootPosition.x;
    }
    std::unique_ptr<CommunicatorNode> node(new CommunicatorNode);
    node->setCategory(Category::Communicator);
    mCommunicator = node.get();
    mSceneGraph.attachChild(std::move(std::unique_ptr<CommunicatorNode>(std::move(node))));
    copyCurrentPatternToNodes(true);
}

CommandQueue& World::getCommandQueue()
{
    return mCommandQueue;
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mBackgroundSprite, states);
	target.draw(mSceneGraph);
}

void World::update()
{
    checkCommunicator();
    updateQuestionState();
	while(!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop());
    mSceneGraph.update();
}

void World::initializePatterns()
{
    using namespace Category;
    unsigned N = None, D = Diatonic, P = Pentatonic, C = MajorRoot, A = MinorRoot, E = End;
    Vec2D pattern1
    {
        {A,N,D,C,E,E},
        {P,D,N,P,E,E},
        {C,N,P,N,E,E},
        {P,N,A,N,E,E},
        {P,N,P,D,E,E},
        {A,N,D,C,E,E}
    };
    Vec2D pattern2
    {
        {D,C,N,P,E,E},
        {N,P,N,A,E,E},
        {P,N,P,D,E,E},
        {A,N,D,C,E,E},
        {P,D,N,P,E,E},
        {D,C,N,P,E,E}
    };
    Vec2D pattern3
    {
        {N,P,N,P,D,E},
        {N,A,N,D,C,E},
        {P,D,N,P,N,E},
        {D,C,N,P,N,E},
        {N,P,N,A,N,E},
        {N,P,N,P,D,E}
    };
    Vec2D pattern4
    {
        {P,D,N,P,E,E},
        {D,C,N,P,E,E},
        {P,N,A,N,E,E},
        {P,N,P,D,E,E},
        {A,N,D,C,E,E},
        {P,D,N,P,E,E}
    };
    Vec2D pattern5
    {
        {N,P,N,A,E,E},
        {N,P,N,P,E,E},
        {A,N,D,C,E,E},
        {P,D,N,P,E,E},
        {D,C,N,P,E,E},
        {N,P,N,A,E,E}
    };
    mPatterns.push_back(Pattern(pattern1, "Pentatonic: Pattern 1", mIterationLimits));
    mPatterns.push_back(Pattern(pattern2, "Pentatonic: Pattern 2", mIterationLimits));
    mPatterns.push_back(Pattern(pattern3, "Pentatonic: Pattern 3", mIterationLimits));
    mPatterns.push_back(Pattern(pattern4, "Pentatonic: Pattern 4", mIterationLimits));
    mPatterns.push_back(Pattern(pattern5, "Pentatonic: Pattern 5", mIterationLimits));
}

const sf::Font& World::getFont() const
{
    return mFont;
}

std::string World::getActiveQuestionName() const
{
    if(mCurrentQuestionState == Category::Pentatonic)
        return "pentatonic";
    else if(mCurrentQuestionState == Category::Diatonic)
        return "diatonic";
    else if(mCurrentQuestionState == Category::MajorRoot)
        return "major root";
    else if(mCurrentQuestionState == Category::MinorRoot)
        return "minor root";
    return "None";
}

std::string World::getActivePatternName() const
{
    return mPatterns[mCurrentPatternIndex].mName;
}

void World::updateQuestionState()
{
    unsigned pentatonicGuessed = 0, diatonicGuessed = 0,
             majorRootGuessed = 0, minorRootGuessed = 0;
    int errorCount = 0;
    for(const auto& i : mSceneGraph.getTree())
    {
        CircleNode* node = dynamic_cast<CircleNode*>(i.get());
        if(node != nullptr)
        {
            if(node->isShowingSolution())
                return;
            errorCount += node->getErrorCount();
            if(node->guessed())
            {
                switch(node->getCategory())
                {
                case Category::Pentatonic:
                case Category::MajorRoot:
                case Category::MinorRoot:
                    ++pentatonicGuessed;
                    break;
                case Category::Diatonic:
                    ++diatonicGuessed;
                    break;
                }
            }
            if(node->rootGuessed())
            {
                switch(node->getCategory())
                {
                case Category::MajorRoot:
                    ++majorRootGuessed;
                    break;
                case Category::MinorRoot:
                    ++minorRootGuessed;
                    break;
                }
            }
        }
    }
    mTotalErrorCount = errorCount;

    if(mCurrentQuestionState == Category::Pentatonic && pentatonicGuessed == mPatterns[mCurrentPatternIndex].numberOfPentatonicNotes)
        mCurrentQuestionState = Category::MajorRoot;
    if(mCurrentQuestionState == Category::MajorRoot && majorRootGuessed == mPatterns[mCurrentPatternIndex].numberOfMajorRoots)
        mCurrentQuestionState = Category::MinorRoot;
    if(mCurrentQuestionState == Category::MinorRoot && minorRootGuessed == mPatterns[mCurrentPatternIndex].numberOfMinorRoots)
        mCurrentQuestionState = Category::Diatonic;
    if(mCurrentQuestionState == Category::Diatonic && diatonicGuessed == mPatterns[mCurrentPatternIndex].numberOfDiatonicNotes)
        mCurrentQuestionState = Category::None;

    for(const auto& i : mSceneGraph.getTree())
    {
        CircleNode* node = dynamic_cast<CircleNode*>(i.get());
        if(node != nullptr)
            node->setQuestionState(mCurrentQuestionState);
    }
}

void World::copyCurrentPatternToNodes(bool reset)
{
    mCurrentQuestionState = Category::Pentatonic;
    int i = mIterationLimits.yMin, j = mIterationLimits.xMin;
    for(const auto& node : mSceneGraph.getTree())
    {
        CircleNode* circleNode = dynamic_cast<CircleNode*>(node.get());
        if(circleNode != nullptr)
        {
            circleNode->setCategory(mPatterns[mCurrentPatternIndex].mCategoryInformation[i][j]);
            if(circleNode->isShowingSolution() && !reset)
            {
                circleNode->resetStates();
                circleNode->enableSolutionState();
            }
            else
                circleNode->resetStates();

            if(++j == mIterationLimits.xMax)
            {
                j = 0;
                ++i;
            }
        }
    }
}

int World::getTotalErrorCount() const
{
    return mTotalErrorCount;
}

void World::showNextPattern()
{
    if(++mCurrentPatternIndex == mPatterns.size())
        mCurrentPatternIndex = 0;
    copyCurrentPatternToNodes(false);
}

void World::showPreviousPattern()
{
    if(mCurrentPatternIndex == 0)
        mCurrentPatternIndex = mPatterns.size() - 1;
    else
        --mCurrentPatternIndex;
    copyCurrentPatternToNodes(false);
}

void World::checkCommunicator()
{
    if(mCommunicator->mShowNextPatternState)
    {
        mCommunicator->mShowNextPatternState = false;
        showNextPattern();
    }
    if(mCommunicator->mShowPreviousPatternState)
    {
        mCommunicator->mShowPreviousPatternState = false;
        showPreviousPattern();
    }
    if(mCommunicator->mResetCurrentPatternState)
    {
        mCommunicator->mResetCurrentPatternState = false;
        copyCurrentPatternToNodes(true);
    }
    if(mCommunicator->mToggleBassState)
    {
        mCommunicator->mToggleBassState = false;
        toggleBassState();
    }
}

void World::toggleBassState()
{
    mBassStateActive = !mBassStateActive;
    if(mBassStateActive)
    {
        mIterationLimits.setBassLimits();
        mBackgroundTexture.loadFromFile("resources/GriffbrettBass.png");
    }
    else
    {
        mIterationLimits.setGuitarLimits();
        mBackgroundTexture.loadFromFile("resources/Griffbrett.png");
    }
    for(Pattern& i : mPatterns)
        i.countNotes(mIterationLimits);

    addGameField();
    copyCurrentPatternToNodes(true);
    mBackgroundSprite.setTexture(mBackgroundTexture);
}

bool World::bassStateActive() const
{
    return mBassStateActive;
}
