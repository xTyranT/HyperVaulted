#pragma once

#include "Response.hpp"

class Cgi
{
    public:
        Cgi();
        Cgi(const Cgi &ref);
        const Cgi& operator=(const Cgi& ref);
        ~Cgi();

        void cgiCaller(Server& srv, Location& req);
};