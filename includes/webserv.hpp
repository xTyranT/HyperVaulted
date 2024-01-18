#pragma once

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <typeinfo> 
#include <stack>
#include <vector>
#include <map>

#define PORT "port"
#define HOST "host"
#define ROOT "root"
#define INDEX "index"
#define SRVNAMES "server_name"
#define ERRPAGES "error_pages"
#define MAXBDSIZE "max_body_size"
#define LOCATION "location"

enum typeCode
{
    _int,
    _string,
    _vector,
    _map,
    _long,
    _l_config,
    _none
};

typedef struct l_config
{
    std::map<std::string, void*> methods;     // std::vector<std::string>
    std::map<std::string, void*> root;         // std::string
    std::map<std::string, void*> indexes;     // std::vector<std::string>
    std::map<std::string, void*> autoIndex;    // bool
    std::map<std::string, void*> upload;       // bool
    std::map<std::string, void*> cgi;          // bool
    std::map<std::string, void*> uploadPath;   // std::string
    std::map<std::string, void*> cgiPaths;    // std::map<std::string, std::string>
} l_config;

typedef struct s_config
{
    std::string server;
    std::map<std::string, void*> port;         // int
    std::map<std::string, void*> host;         // std::string
    std::map<std::string, void*> root;         // std::string
    std::map<std::string, void*> indexes;     // std::vector<std::string>
    std::map<std::string, void*> srvNames;    // std::vector<std::string>
    std::map<std::string, void*> errPages;    // std::map<int, std::string>
    std::map<std::string, void*> maxBodySize;  // long
    std::map<std::string, void*> location;     // l_config
} s_config;