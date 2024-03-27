#pragma once

#include "Server.hpp"

class RequestLine
{
    public:
        std::string method;
        std::string path;
        std::string httpVersion;
        std::string realPath;
        std::string qstr;

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
        int sindx;
        std::string file;
        std::string responseBuffer;
        Location matchedLocation;

        Request(void);
        Request(const Request& other);
        const Request& operator=(const Request& other);
        ~Request(void);

        void requestParser(std::string& request, std::vector<Server>& srv, bool& enf);
        int valueChecker(Server& srv);
        std::pair<int, std::string> generateCorrespondingErrorPage(void);
        Server& matchHostWithServer(std::vector<Server>& srv, std::string request, int srvPort);
        void matchServerWithConfig(std::vector<Server>& srv);
        std::string errorPageMessage(void);
        bool pathURIChecker(std::string& URI);
        void matchLocation(Server& srv, bool& enf);
        void checkAllowdUriCharacters(std::string uri);
        void printRequestComponents(void);
        Location& matchURIWithLocation(Server& srv, std::string path, bool& enf);
};