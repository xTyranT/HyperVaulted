#pragma once

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <map>

typedef struct l_config
{
    std::map<std::string, std::vector<std::string> > methods;
    std::map<std::string, std::string> root;
    std::map<std::string, std::vector<std::string> > indexes;
    std::map<std::string, bool> autoIndex;
    std::map<std::string, bool> upload;
    std::map<std::string, bool> cgi;
    std::map<std::string, std::string> uploadPath;
    std::map<std::string, std::map<std::string, std::string> > cgiPaths;
} l_config;

typedef struct s_config
{
    std::map<std::string, int> port;
    std::map<std::string, std::string> host;
    std::map<std::string, std::string> root;
    std::map<std::string, std::vector<std::string> > indexes;
    std::map<std::string, std::vector<std::string> > srvNames;
    std::map<std::string, std::map<int, std::string> > errPages;
    std::map<std::string ,long> maxBodySize;
    std::map<std::string, l_config> location;
} s_config;