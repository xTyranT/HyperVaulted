
#pragma once

#include "webserv.hpp"
#include "../includes/Response.hpp"

class   Client
{   
    public:
        int chunksize;
        std::string chunkedbuff;
        std::string request;
        std::string requestHeader;
        std::string filename;
        std::ofstream postFile;
        int contentlength;
        int sread;
        int svfd;
        Response reqRes;

        bool read;
        bool flag;
        bool enf;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
};