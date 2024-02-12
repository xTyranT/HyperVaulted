
#pragma once
#include "Request.hpp"

class   Client
{   
    public:
        int sread;
        int rd;
        int svfd;
        bool first;
        std::string request;
        std::string requestHeader;
        Request parsedRequest;
        std::string filename;
        std::ofstream postFile;
        bool read;
        bool flag;
        bool enf;
        Client & operator= ( const Client & other );
        Client (const Client & other );
        Client ();
}; 