#pragma once

#include "Response.hpp"

class Cgi
{
    public:
        bool executed;
        pid_t pid;
        int status;
        FILE* out;
        FILE* in;
        clock_t start;
        clock_t end;
        std::string cgiExtension;
        pid_t cgiPid;

        Cgi();
        Cgi(const Cgi &ref);
        const Cgi& operator=(const Cgi& ref);
        ~Cgi();

        void cgiCaller(Server& srv, Location& req, Response& res, bool& enf);
        void formCgiResponse(Server& srv, Location& req, Response& res);
};