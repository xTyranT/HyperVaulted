
#pragma once
#include "stringManipulators.hpp"

class   Client
{   
    public:
        int svfd;
        std::string request;
        std::string requestHeader;
        bool read;
        Client ();
}; 