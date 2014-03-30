/*
 * DataManager
 * cdam_data_manager.cpp
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
 * Choosatron Deluxe Adventure Matrix
 * Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
 * All rights reserved.
 */


class DataManager
{
    private:
        // Private Constructor
        DataManager();
        // Stop the compiler generating methods of copy the object
        DataManager(DataManager const& copy);            // Not Implemented
        DataManager& operator=(DataManager const& copy); // Not Implemented

    public:
        static DataManager& getInstance()
        {
            // The only instance
            // Guaranteed to be lazy initialized
            // Guaranteed that it will be destroyed correctly
            static DataManager instance;
            return instance;
        }
};