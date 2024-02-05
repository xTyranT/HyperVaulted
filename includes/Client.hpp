
#pragma once

#include "webserv.hpp"
#include "../includes/Request.hpp"

class   Client
{   
    public:
        int svfd;
        std::string request;
        std::string requestHeader;
        Request parsedRequest;
        bool read;
        Client ();
}; 