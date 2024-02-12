#pragma once 

#include "Request.hpp"

class Response : public Request
{
    public:
        std::string responseBuffer;
        
        Response();
        Response(const Response& other);
        void formTheResponse(void);
        void sendResponse(void);
        std::string appropiateFileLength(void);
        std::string appropiateFileBody(void);
        void permanentRedirecting(void);
        void listDirFiles(std::string path);
        void getMethod(Location& req);
        ~Response();
};