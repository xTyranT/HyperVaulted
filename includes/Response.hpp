#pragma once 

#include "Request.hpp"

class Response : public Request
{
    public:
        std::string postCgiFile;
        
        Response();
        Response(const Response& other);
        const Response& operator=(const Response& other);
        ~Response();

        void formTheResponse(Server& srv, Location& req);
        void listDirFiles(std::string path);
        void formChunkedResponse(Location& req, Server& srv);
        void getMethod(Location& req, Server& srv);
        void openErrorPage(Server& srv);
        void deleteMethod(Location& req, Server& srv);
        void deleteDirectory(std::string path, Location& req, Server& srv);
};

std::string determineFileExtension(std::string fileName);