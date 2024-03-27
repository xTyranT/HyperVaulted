
#pragma once

#include "webserv.hpp"
#include "Response.hpp"
#include "Cgi.hpp"

class   Client
{   
    public:
        int sfd;
        int fd;
        size_t chunksize;
        size_t contentlength;
        size_t sread;
        clock_t start;
        std::string chunkedbuff;
        std::string request;
        std::string requestHeader;
        std::ofstream postFile;
        std::ifstream resFile;
        Response reqRes;
        Cgi cgi;

        bool read;
        bool flag;
        bool enf;
        bool resred;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
        ~Client ();
};
