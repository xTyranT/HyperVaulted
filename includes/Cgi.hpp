#pragma once

#include "Response.hpp"

class Cgi
{
    public:
        Cgi();
        Cgi(const Cgi &ref);
        const Cgi& operator=(const Cgi& ref);
        ~Cgi();

        void cgiCaller(Server& srv, Location& req, Response& res);
        void formCgiResponse(Server& srv, Location& req, Response& res);
        char** getEnv(Server& srv, Response& res);
        char** getArgv(Server& srv, Response& res, Location& req);
};