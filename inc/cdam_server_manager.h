#ifndef CDAM_SERVER_MANAGER_H
#define CDAM_SERVER_MANAGER_H

#include "spark_utilities.h"

namespace cdam
{

class ServerManager
{
    public:
        /* Public Methods */
        static ServerManager& getInstance()
        {
            // The only instance
            // Guaranteed to be lazy initialized
            // Guaranteed that it will be destroyed correctly
            static ServerManager instance;
            return instance;
        }

        void initialize();

        /* Public Variables */
        char lastCommand[64];

    private:
        /* Private Methods */
        ServerManager();
        // Stop the compiler generating methods of copy the object
        ServerManager(ServerManager const& copy);            // Not Implemented
        ServerManager& operator=(ServerManager const& copy); // Not Implemented

        static int serverCommand(String aCommand);

        /* Private Variables */
};

}

#endif