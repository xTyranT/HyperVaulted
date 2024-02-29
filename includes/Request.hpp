#pragma once

#include "Server.hpp"

class RequestLine
{
    public:
        std::string method;
        std::string path;
        std::string httpVersion;

        RequestLine(void);
        RequestLine(const RequestLine& other);
        const RequestLine& operator=(const RequestLine& other);
        ~RequestLine(void);
};

class  Request
{
    public:
        RequestLine Component;
        std::map<std::string, std::string>  httpHeaders;
        int returnCode;
        int sFd;
        std::string file;
        std::string responseBuffer;
        Location matchedLocation;

        Request(void);
        Request(const Request& other);
        const Request& operator=(const Request& other);
        ~Request(void);

        std::string getRequest(void);
        void requestParser(std::string& request, std::vector<Server>& srv);
        int valueChecker(std::vector<Server>& srv);
        std::pair<int, std::string> generateCorrespondingErrorPage(void);
        std::string errorPageMessage(void);
        bool pathURIChecker(std::string& URI);
        void matchLocation(std::vector<Server>& srv, int whichServer);
        void printRequestComponents(void);
        Location& matchURIWithLocation(std::vector<Server>& srv);
};