#include "../includes/stringManipulators.hpp"

void strtrim(std::string& s, std::string set)
{
    for(std::string::iterator i = set.begin(); i != set.end(); i++)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::bind1st(std::not_equal_to<char>(), *i)));
        s.erase(std::find_if(s.rbegin(), s.rend(), std::bind1st(std::not_equal_to<char>(), *i)).base(), s.end());
    }
}

bool skippableCharacterString(std::string line)
{
    for(std::string::iterator i = line.begin(); i != line.end(); i++)
    {
        if(*i != ' ' && *i != '\n')
            return false;
    }
    return true;
}

std::vector<std::string> split(std::string& line)
{
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    std:: vector<std::string> vector;

    while ((pos = line.find(delimiter)) != std::string::npos) 
    {
        token = line.substr(0, pos);
        if(!token.empty())
            vector.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    if(!line.empty())
        vector.push_back(line);
    return vector;
}

std::string to_string(int value)
{
    std::stringstream temp;
    temp << value;
    return temp.str();
}
