#ifndef CDAM_SERVER_MANAGER_H
#define CDAM_SERVER_MANAGER_H

#include "spark_utilities.h"
#include "spark_wiring_tcpclient.h"
#include "elapsed_time.h"

namespace cdam
{

class StateController;

class ServerManager
{
    public:
        /* Public Methods */
        ServerManager();
        void initialize(StateController* aStatecontroller);
        void handlePendingActions();

        /* Public Variables */
        bool pendingAction;
        //char lastCommand[64];
        char* pendingArguments;
        char* pendingCommand;

        byte serverIp[4];
        uint16_t serverPort;

        //uint8_t newStoryIndex;
        //uint32_t newStorySize;

    private:
        /* Private Methods */
        static int serverCommand(String aCommand);
        //TCPClient* connectToServer(byte aServer[4], uint16_t aPort);
        bool getStoryData(TCPClient *aClient, uint32_t aStorySize);
        void parseServerAddress(char* aAddress);

        /* Private Variables */
        DataManager* _dataManager;
        HardwareManager* _hardwareManager;
        ServerManager* _serverManager;
        StateController* _stateControl;
        ElapsedMillis _actionsElapsed;
};

}

#endif