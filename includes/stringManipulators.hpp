#pragma once

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <typeinfo> 
#include <stack>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

// void strtrim(std::string& s);
#include <errno.h>
#include <netinet/in.h>
#include <iomanip>
#include <locale>
#include <fcntl.h>
#include <limits.h>

void strtrim(std::string& s, std::string set);
bool skippableCharacterString(std::string line);
std::vector<std::string> split(std::string& line);