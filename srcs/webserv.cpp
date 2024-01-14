#include "../includes/webserv.hpp"



void attribute_initializer(s_config& server)
{
    /* server */
    server.port.insert(std::pair<std::string, int>("port", -1));
    server.host.insert(std::pair<std::string, std::string>("host", ""));
    server.root.insert(std::pair<std::string, std::string>("root", ""));
    server.indexes.insert(std::pair<std::string, std::vector<std::string> >("index", std::vector<std::string>()));
    server.srvNames.insert(std::pair<std::string, std::vector<std::string> >("server_names", std::vector<std::string>()));
    server.errPages.insert(std::pair<std::string, std::map<int, std::string> >("error_pages", std::map<int, std::string>()));
    server.maxBodySize.insert(std::pair<std::string, long>("max_body_size", -1));
        /* location */
        l_config location;

        location.methods.insert(std::pair<std::string, std::vector<std::string> >("methods", std::vector<std::string>()));
        location.root.insert(std::pair<std::string, std::string>("root", ""));
        location.indexes.insert(std::pair<std::string, std::vector<std::string> >("index", std::vector<std::string>()));
        location.autoIndex.insert(std::pair<std::string, bool>("autoIndex", false));
        location.upload.insert(std::pair<std::string, bool>("upload", false));
        location.cgi.insert(std::pair<std::string, bool>("cgi", false));
        location.uploadPath.insert(std::pair<std::string, std::string>("upload_path", ""));
        location.cgiPaths.insert(std::pair<std::string, std::map<std::string, std::string> >("cgi_path", std::map<std::string, std::string>()));

    server.location.insert(std::pair<std::string, l_config>("location", location));
}

void set_attr_value(s_config& server, std::string& attr)
{
    std::vector<std::string> attrs;

    for(std::vector<std::string>::iterator i = attrs.begin(); i != attrs.end(); i++)
    {
        
    }
}

std::ifstream& config_reader(char** argv)
{
    std::ifstream* file = new std::ifstream(argv[1]);
    if (!file->is_open())
    {
        delete file;
        throw std::invalid_argument("no such file or permission denied");
    }
    return *file;
}

void server_block(std::ifstream& file)
{
    s_config server;
    attribute_initializer(server);
    std::string line;
    std::string rest;
    std::getline(file, line);
    size_t i = -1;
    while(++i < line.size() && (std::isspace(line.at(i)) || line.at(i) == '}' ));
    line = line.substr(i, line.size() - i);
    i = -1;
    while(++i < line.size() && line.at(i) != '{' && line.at(i) != '\n');
    rest = line.substr(i, line.size() - i);
    line = line.substr(0, i);
    if (line != "server")
    {
        delete &file;
        throw std::invalid_argument("config file requires a server block: \nserver\n{\n  ...\n}");
    }
    std::stack<char> brackets;
    std::string attr;
    for(size_t i = 0; i < rest.length(); i++)
    {
        if (rest.at(i) == '{')
            brackets.push(rest.at(i));
        else
        {
            if (rest.at(i) != ' ' && rest.at(i) != '}')
                attr.push_back(rest.at(i));
            else
            
        }

    }
}

void config_parser(std::ifstream& file)
{
    std::string line;
    std::getline(file, line);

}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
            throw std::invalid_argument("./HyperVaulted <config_file>");
        std::ifstream& file = config_reader(argv);
        server_block(file);
        delete &file;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}