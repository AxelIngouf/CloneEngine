#include "Test.h"

#include <iostream>

#include <fmod.hpp>
#include <fmod_errors.h>

namespace sound
{
    void Test::HelloWorld(std::string indentation)
    {
        std::cout << indentation << "Sound say : [Hello] to [World] !" << std::endl;

        FMOD_RESULT result;
        FMOD::System* system = NULL;

        result = FMOD::System_Create(&system);      // Create the main system object.
        if (result != FMOD_OK)
        {
            printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            exit(-1);
        }

        result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
        if (result != FMOD_OK)
        {
            printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            exit(-1);
        }

        result = system->release();    // Shut Down FMOD.
        if (result != FMOD_OK)
        {
            printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
            exit(-1);
        }
    }
}