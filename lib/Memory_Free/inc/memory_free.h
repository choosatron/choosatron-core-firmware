#ifndef MEMORY_FREE_H
#define MEMORY_FREE_H

// MemoryFree library based on code posted here:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15
//
// Extended by Matthew Murdoch to include walking of the free list.

//#ifdef __cplusplus
   //extern "C" {
//#endif

class MemoryFree
{
   public:
   int freeMemory();

   private:
   int freeListSize();
};



//#ifdef __cplusplus
   //}
//#endif

#endif