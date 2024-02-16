#pragma once

#include "Server.hpp"

class RequestLine
{
    public:
        std::string method;
        std::string path;
        std::string httpVersion;

        RequestLine(void);
        ~RequestLine(void);
};

class  Request
{
    public:
        RequestLine Component;
        std::map<std::string, std::string>  httpHeaders;
        int returnCode;
        int sFd;

        Request(void);
        std::string getRequest(void);
        void requestParser(std::string& request, std::vector<Server>& srv);
        int valueChecker(std::vector<Server>& srv);
        std::pair<int, std::string> generateCorrespondingErrorPage(void);
        std::string errorPageMessage(void);
        bool pathURIChecker(std::string& URI);
        void printRequestComponents(void);
        Location& matchURIWithLocation(std::vector<Server>& srv);
        ~Request(void);
};