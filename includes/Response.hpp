#pragma once 

#include "Request.hpp"

class Response
{
    public:
        std::string responseBuffer;
        Response();
        void formTheResponse(Request& request);
        void sendRespons(void);
        ~Response();
};