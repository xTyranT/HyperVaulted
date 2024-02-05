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
        void requestParser(std::string& request, std::vector<Server>& vec);
        int valueChecker(std::vector<Server>& vec);
        void generateCorrespondingErrorPage(void);
        std::string errorPageMessage(void);
        bool pathURIChecker(std::string& URI);
        void printRequestComponents(void);
        ~Request(void);
};