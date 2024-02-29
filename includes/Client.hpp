
#pragma once

#include "webserv.hpp"
#include "../includes/Response.hpp"

class   Client
{   
    public:
    int sfd;
        int fd;
        int chunksize;
        int contentlength;
        int sread;
        std::string chunkedbuff;
        std::string request;
        std::string requestHeader;
        std::string filename;
        std::ofstream postFile;
        std::ifstream resFile;
        Response reqRes;

        bool read;
        bool flag;
        bool enf;
        bool resred;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
};
