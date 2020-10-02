#include <string>
#include <iostream>

#include "log.h"

void DebugLog::WriteLine(std::string pMessage)
{
    std::cout << pMessage << '\n' << std::endl;
}