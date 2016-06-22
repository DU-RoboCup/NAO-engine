#include "debug.h"

int main()
{
   LOG_DEBUG << "Example debug message";
   LOG_WARNING << "Example warning message";
   LOG_ERROR << "Example error message";
   LOG_FATAL << "Example Fatal Error message";
   return 0;
}
