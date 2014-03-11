

#include <Logger.h>

// initialise the static memeber variables.

MySDFat Logger::logFile ; // set constructor parameters here as/and if required.

#define CLOSED 0
#define OPENED 1
char Logger::state = CLOSED ;

int  Logger::lineCount = 0 ;

// Ensure the single chosen file is open for logging
void Logger::open()
{
  if (state != OPENED)
  {
     // open the file :)
     state = OPENED ;
  }
}

void Logger::close()
{
  if (state == OPENED)
  {
    // close the file :)
    state = CLOSED ;
  }
}

void Logger::writeLine(char *line)
{
  open() ; // open file
  // logFile.writeLine(line) ;
  lineCount ++ ;
}