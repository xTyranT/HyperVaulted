#pragma once

#include "webserv.hpp"

class Location
{
    public:
        std::string path;
        std::vector<std::string> methods;
        std::string root;
        std::vector<std::string> indexes;
        bool autoIndex;
        bool upload;
        bool cgi;
        bool dirListing;
        std::string uploadPath;
        std::vector<std::string> cgiPaths;
        std::map<int, std::string> ret;
        Location(void);
        void checkAndStoreLocationAttributes(std::vector<std::string> attr);
        ~Location(void);

};

class Server
{
    public:
        struct sockaddr_in sockett;
        int port;
        std::string host;
        std::string root;
        std::vector<std::string> indexes;
        std::vector<std::string> srvNames;
        std::map<int, std::string> errPages;
        long maxBodySize;
        std::vector<Location> location;
        std::vector<Location>::iterator l_i;
        Server(void);
        void defaultErrorPages(void);
        void checkNecessaryAttributes(void);
        void printServerAttributes(void);
        void checkAndStoreServerAttributes(std::vector<std::string> attr, std::ifstream& file);
        void serverBlock(std::ifstream& file);
        ~Server(void);
};

std::vector<Server> getAvailableServers(std::ifstream& file);
void    multiplexing( std::vector<Server> & sv );