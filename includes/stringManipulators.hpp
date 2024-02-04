#pragma once

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <typeinfo> 
#include <stack>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <fcntl.h>
#include <limits.h>

void strtrim(std::string& s, std::string set);
bool skippableCharacterString(std::string line);
std::vector<std::string> split(std::string& line);