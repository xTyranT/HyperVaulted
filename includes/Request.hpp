#pragma once

#include "webserv.hpp"

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

        Request(void);
        std::string getRequest(void);
        int requestParser(std::string& request);
        int valueChecker(void);
        bool pathURIChecker(std::string& URI);
        void printRequestComponents(void);
        ~Request(void);
};