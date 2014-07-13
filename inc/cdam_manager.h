/**
 ******************************************************************************
 * @file    cdam_manager.h
 * @author  Jerry Belich
 * @version V1.0.0
 * @date    30-March-2014
 * @brief   Header for cdam_manager.cpp module
 ******************************************************************************
  Copyright (c) 2014 Monkey with a Mustache, LLC.  All rights reserved.
 ******************************************************************************
 *
 * Limitation: Single Threaded Design
 * See: http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
 *      For problems associated with locking in multi threaded applications
 *
 * Limitation:
 * If you use this Singleton (A) within a destructor of another Singleton (B)
 * This Singleton (A) must be fully constructed before the constructor of (B)
 * is called.
 *
 */

#ifndef CDAM_MANAGER_H
#define CDAM_MANAGER_H

#include "cdam_constants.h"
#include "cdam_data_manager.h"
#include "cdam_server_manager.h"
#include "cdam_hardware_manager.h"


namespace cdam
{


class Manager
{
    public:
        /* Public Methods */
        static Manager& getInstance()
        {
            // The only instance
            // Guaranteed to be lazy initialized
            // Guaranteed that it will be destroyed correctly
            static Manager instance;
            return instance;
        }

        void initialize();

        /* Public Variables */
        DataManager *dataManager;
        ServerManager *serverManager;
        HardwareManager *hardwareManager;

    private:
        /* Private Methods */
        Manager();
        // Stop the compiler generating methods of copy the object
        Manager(Manager const& copy);            // Not Implemented
        Manager& operator=(Manager const& copy); // Not Implemented

};

}

#endif