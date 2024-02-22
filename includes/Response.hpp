#pragma once 

#include "Request.hpp"

class Response : public Request
{
    public:        
        Response();
        Response(const Response& other);
        const Response& operator=(const Response& other);
        ~Response();

        void formTheResponse(Server& srv);
        void listDirFiles(std::string path);
        void formChunkedResponse(Location& req, Server& srv);
        void getMethod(Location& req, Server& srv);
};

std::string determineFileExtension(std::string fileName);