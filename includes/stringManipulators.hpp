#pragma once

#include "webserv.hpp"

void strtrim(std::string& s);
bool skippableCharacterString(std::string line);
std::vector<std::string> split(std::string& line);