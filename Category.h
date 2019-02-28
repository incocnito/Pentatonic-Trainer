#ifndef CATEGORY_H_INCLUDED
#define CATEGORY_H_INCLUDED

namespace Category
{
    enum
    {
        None                = 1 << 0,
        Pentatonic          = 1 << 1,
        Diatonic            = 1 << 2,
        MajorRoot           = 1 << 3,
        MinorRoot           = 1 << 4,
        All                 = None | Pentatonic | Diatonic | MajorRoot | MinorRoot,
        Communicator        = 1 << 5,
        End                 = 1 << 6
    };
};

#endif // CATEGORY_H_INCLUDED
