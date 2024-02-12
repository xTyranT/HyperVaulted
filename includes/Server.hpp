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
        std::string uploadPath;
        std::vector<std::string> cgiPaths;
        std::map<int, std::string> ret;
        Location(void);
        void checkAndStoreLocationAttributes(std::vector<std::string> attr);
        void checkNecessaryAttributes(void);
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
        int fd;
        Server(void);
        void generateErrorPages(void);
        void checkNecessaryAttributes(void);
        void printServerAttributes(void);
        void checkAndStoreServerAttributes(std::vector<std::string> attr, std::ifstream& file);
        void serverBlock(std::ifstream& file);
        bool empty(void);
        ~Server(void);
};

std::vector<Server> getAvailableServers(std::ifstream& file);
void    multiplexing( std::vector<Server> & sv );
void    Post( class Client & Clients , char *buff , int rd );
