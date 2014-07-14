#ifndef CDAM_SERVER_MANAGER_H
#define CDAM_SERVER_MANAGER_H

#include "spark_utilities.h"

namespace cdam
{

class ServerManager
{
    public:
        /* Public Methods */
        ServerManager();

        void initialize();

        /* Public Variables */
        char lastCommand[64];

    private:
        /* Private Methods */

        static int serverCommand(String aCommand);

        //static bool downloadStoryData(String aServerAddressAndPort);

        /* Private Variables */
};

}

#endif