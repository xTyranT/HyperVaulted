
#pragma once
#include "Request.hpp"

class   Client
{   
    public:
        int chunksize;
        std::string chunkedbuff;
        std::string request;
        std::string requestHeader;
        Request parsedRequest;
        std::string filename;
        std::ofstream postFile;
        int contentlength;
        int sread;
        int svfd;
        bool read;
        bool flag;
        bool enf;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
};