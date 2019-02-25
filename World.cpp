#include "World.h"
#include "Category.h"

#include <fstream>

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
    }
{
    std::srand(std::time(0));
	mFont.loadFromFile("resources/arial.ttf");
    mBackgroundTexture.loadFromFile("resources/Griffbrett.png");
    mBackgroundSprite.setTexture(mBackgroundTexture);

    initializePatterns();

    addGameField(sf::Vector2f(282, 175));
}

void World::addGameField(const sf::Vector2f& rootPosition)
{
    //282, 320, 356, 393, 430, -> 37er-schritte horizontal
    //175, 209, 242, 275, 310, -> 34er-schritte vertikal
    sf::Vector2f currentPosition = rootPosition;
    for(int i = 0; i < FIELD_SIZE; ++i)
    {
        for(int j = 0; j < FIELD_SIZE; ++j)
        {
            std::unique_ptr<CircleNode> square(new CircleNode(mFont, currentPosition, mPatterns[0].mCategoryInformation[i][j], mNotePattern[i][j]));
            mSceneGraph.attachChild(std::move(std::unique_ptr<CircleNode>(std::move(square))));
            currentPosition.x += 37;
        }
        currentPosition.y += 34;
        currentPosition.x = rootPosition.x;
    }
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
    updateQuestionState();
	while(!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop());
    mSceneGraph.update();
}

void World::initializePatterns()
{
    using namespace Category;
    unsigned N = None, D = Diatonic, P = Pentatonic, C = MajorRoot, A = MinorRoot;
    Vec2D pattern1
    {
        {N,A,N,D,C,N},
        {N,P,D,N,P,N},
        {N,C,N,P,N,N},
        {N,P,N,A,N,D},
        {N,P,N,P,D,N},
        {N,A,N,D,C,N}
    };
    Vec2D pattern2
    {
        {N,D,C,N,P,N},
        {N,N,P,N,A,N},
        {N,P,N,P,D,N},
        {N,A,N,D,C,N},
        {N,P,D,N,P,N},
        {N,D,C,N,P,N}
    };
    Vec2D pattern3
    {
        {N,P,N,P,D,N},
        {N,A,N,D,C,N},
        {P,D,N,P,N,N},
        {N,C,N,P,N,D},
        {N,P,N,A,N,D},
        {N,P,N,P,D,N}
    };
    Vec2D pattern4
    {
        {N,P,D,N,P,N},
        {N,D,C,N,P,N},
        {N,P,N,A,N,N},
        {N,P,N,P,D,N},
        {N,A,N,D,C,N},
        {N,P,D,N,P,N}
    };
    Vec2D pattern5
    {
        {D,N,P,N,A,N},
        {N,N,P,N,P,N},
        {N,A,N,D,C,N},
        {N,P,D,N,P,N},
        {N,D,C,N,P,N},
        {D,N,P,N,A,N}
    };
    mPatterns.push_back(Pattern(pattern1, "Pentatonic: Pattern 1"));
    mPatterns.push_back(Pattern(pattern2, "Pentatonic: Pattern 2"));
    mPatterns.push_back(Pattern(pattern3, "Pentatonic: Pattern 3"));
    mPatterns.push_back(Pattern(pattern4, "Pentatonic: Pattern 4"));
    mPatterns.push_back(Pattern(pattern5, "Pentatonic: Pattern 5"));
    std::random_shuffle(mPatterns.begin(), mPatterns.end(), [] (int i) { return std::rand() % i; } );
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
    return "minor root";
}

std::string World::getActivePatternName() const
{
    return mPatterns[mCurrentPatternIndex].mName;
}

void World::updateQuestionState()
{
    unsigned pentatonicGuessed = 0, diatonicGuessed = 0, majorRootGuessed = 0, minorRootGuessed = 0;
    int errorCount = 0;
    for(const auto& i : mSceneGraph.getTree())
    {
        CircleNode* node = dynamic_cast<CircleNode*>(i.get());
        if(node != nullptr)
        {
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
        mCurrentQuestionState = Category::Diatonic;
    else if(mCurrentQuestionState == Category::Diatonic && diatonicGuessed == mPatterns[mCurrentPatternIndex].numberOfDiatonicNotes)
        mCurrentQuestionState = Category::MajorRoot;
    else if(mCurrentQuestionState == Category::MajorRoot && majorRootGuessed == mPatterns[mCurrentPatternIndex].numberOfMajorRoots)
        mCurrentQuestionState = Category::MinorRoot;
    else if(mCurrentQuestionState == Category::MinorRoot && minorRootGuessed == mPatterns[mCurrentPatternIndex].numberOfMinorRoots)
        mCurrentQuestionState = Category::None;

    if(mCurrentQuestionState == Category::None)
        setNextPatternActive();

    for(const auto& i : mSceneGraph.getTree())
    {
        CircleNode* node = dynamic_cast<CircleNode*>(i.get());
        if(node != nullptr)
            node->setQuestionState(mCurrentQuestionState);
    }
}

void World::setNextPatternActive()
{
    if(++mCurrentPatternIndex == mPatterns.size() - 1)
        mCurrentPatternIndex = 0;
    mCurrentQuestionState = Category::Pentatonic;
    int i = 0, j = 0;
    for(const auto& node : mSceneGraph.getTree())
    {
        CircleNode* squareNode = dynamic_cast<CircleNode*>(node.get());
        if(squareNode != nullptr)
            squareNode->resetStates();
        node.get()->setCategory(mPatterns[mCurrentPatternIndex].mCategoryInformation[i][j]);
        if(++j == FIELD_SIZE)
        {
            j = 0;
            ++i;
        }
    }
    sf::sleep(sf::seconds(1.5));
}

int World::getTotalErrorCount() const
{
    return mTotalErrorCount;
}
