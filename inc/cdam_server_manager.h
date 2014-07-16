#ifndef CDAM_SERVER_MANAGER_H
#define CDAM_SERVER_MANAGER_H

#include "spark_utilities.h"
#include "spark_wiring_tcpclient.h"

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

        TCPClient* connectToServer(byte server[4], uint16_t port);

        bool getStoryData(TCPClient *aClient, uint32_t aByteCount);
        /* Private Variables */
};

}

#endif