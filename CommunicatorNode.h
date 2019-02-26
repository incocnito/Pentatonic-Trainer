#ifndef COMMUNICATORNODE_H_INCLUDED
#define COMMUNICATORNODE_H_INCLUDED

#include "SceneNode.h"

struct CommunicatorNode : public SceneNode
{
    CommunicatorNode();
    bool                mShowNextPatternState;
    bool                mShowPreviousPatternState;
    bool                mResetCurrentPatternState;
    bool                mToggleBassState;
};

#endif // COMMUNICATORNODE_H_INCLUDED
