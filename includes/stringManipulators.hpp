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
#include <deque>
#include <list>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

void strtrim(std::string& s);
bool skippableCharacterString(std::string line);
std::vector<std::string> split(std::string& line);