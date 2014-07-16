#ifndef CDAM_SERVER_MANAGER_H
#define CDAM_SERVER_MANAGER_H

#include "spark_utilities.h"
#include "spark_wiring_tcpclient.h"
#include "elapsed_time.h"

namespace cdam
{

class ServerManager
{
    public:
        /* Public Methods */
        ServerManager();
        void initialize();
        void handlePendingActions();

        /* Public Variables */
        bool pendingAction;
        char lastCommand[64];
        byte serverIp[4];
        uint16_t serverPort;

        uint8_t newStoryIndex;
        uint32_t newStorySize;

    private:
        /* Private Methods */
        static int serverCommand(String aCommand);
        //TCPClient* connectToServer(byte aServer[4], uint16_t aPort);
        bool getStoryData(TCPClient *aClient, uint32_t aStorySize);

        /* Private Variables */
        ElapsedMillis _actionsElapsed;
};

}

#endif