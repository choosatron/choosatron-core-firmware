// structures exported from grammar Choosatron Story Binary-WORKING
/*
   Grammar for the Choosatron Deluxe Adventure Matrix binary story file.
*/

    typedef struct _Story
    {

        struct _StoryHeader
        {
            /*
            Start of Heading byte.
            */
            unsigned char StartHeader[1];

            struct _BinaryVersion
            {
                unsigned int Major;
                unsigned int Minor;
            } BinaryVersion;


            struct _Flags
            {
                unsigned int Features Used;
                unsigned int Toggles;
                unsigned int Flags3;
                unsigned int Flags4;
            } Flags;


            struct _Variables
            {
                unsigned int Bits;
                /*
                The number of small variables to allocate space for (8 bit).
                */
                unsigned int Small;
                /*
                The number of big variables to allocate space for (16 bit).
                */
                unsigned int Big;
            } Variables;

            /*
            The story size in bytes.
            */
            unsigned int StorySize;

            struct _StoryVersion
            {
                unsigned int Major;
                unsigned int Minor;
                unsigned int Revision;
            } StoryVersion;

            char LanguageCode[3];
            char Title[0];
            char Subtitle[0];
            char Author[0];
            char Credits[0];
            char Contact[0];
            char Website[0];

            struct _ReleaseDate
            {
                char Year[4];
                char Month[2];
                char Day[2];
            } ReleaseDate;

        } StoryHeader;


        struct _StoryBody
        {
            /*
            Start of Text byte.
            */
            unsigned char StartBody[1];
            unsigned int PassageCount;

            struct _Passage
            {
                unsigned int Attributes;
                unsigned int UpdateCount;

                struct _ValueUpdate
                {
                    unsigned int VariableTypes;
                    unsigned int ValueOrIndex1;
                    unsigned int ValueOrIndex2;
                } ValueUpdate;

                unsigned int PassageLength;
Other
                unsigned int ChoiceCount;

                struct _Choice
                {
                    unsigned int Attributes;
                    unsigned int ConditionCount;

                    struct _DisplayCondition
                    {
                        unsigned int ValueTypes;
                        unsigned int ValueOrIndex1;
                        unsigned int ValueOrIndex2;
                    } DisplayCondition;

                    unsigned int ChoiceLength;
Other
                    unsigned int PassageOffset;
                    unsigned int UpdateCount;

                    struct _ValueUpdate
                    {
                        unsigned int VariableTypes;
                        unsigned int ValueOrIndex1;
                        unsigned int ValueOrIndex2;
                    } ValueUpdate;

                } Choice;

            } Passage;

            unsigned char EndBody[1];
        } StoryBody;


        struct _Dictionary
        {
            char Word[0];
        } Dictionary;

    } Story;

    typedef struct _ValueUpdate
    {
        unsigned int VariableTypes;
        unsigned int Value/Index1;
        unsigned int Value/Index2;
    } ValueUpdate;

