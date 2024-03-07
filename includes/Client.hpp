
#pragma once

#include "webserv.hpp"
#include "../includes/Response.hpp"

class   Client
{   
    public:
        int sfd;
        int fd;
        long long chunksize;
        long long contentlength;
        long long sread;
        clock_t start;
        clock_t end;
        std::string postpath;
        std::string chunkedbuff;
        std::string request;
        std::string requestHeader;
        std::string filename;
        std::ofstream postFile;
        std::ifstream resFile;
        Response reqRes;

        bool read;
        bool flag;
        bool requestclosed;
        bool enf;
        bool resred;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
        ~Client ();
};
