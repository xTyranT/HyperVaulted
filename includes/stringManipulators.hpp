#pragma once

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
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
#include <errno.h>
#include <netinet/in.h>
#include <dirent.h>
#include <iomanip>
#include <locale>
#include <fcntl.h>
#include <filesystem>
#include <sys/types.h>
#include <fstream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/time.h>


std::string to_string(int value);
void strtrim(std::string& s, std::string set);
bool skippableCharacterString(std::string line);
std::vector<std::string> split(std::string& line);