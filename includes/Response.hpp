#pragma once 

#include "Request.hpp"

class Response : public Request
{
    public:
        std::string responseBuffer;
        std::ifstream file;
        Response();
        Response(const Response& other);
        void formTheResponse(Server& srv);
        void permanentRedirecting(void);
        void listDirFiles(std::string path);
        void formChunkedResponse(Location& req, Server& srv);
        void getMethod(Location& req, Server& srv);
        ~Response();
};