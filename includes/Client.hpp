
#pragma once

#include "webserv.hpp"
#include "../includes/Response.hpp"

class   Client
{   
    public:
        int svfd;
        std::string request;
        std::string requestHeader;
        Response reqRes;

        bool read;
        Client ();
}; 