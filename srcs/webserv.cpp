#include "../includes/webserv.hpp"

void attribute_initializer(s_config& server)
{
    /* server */
    server.server = "server";
    server.port.insert(std::pair<std::string, void*>("port", NULL));
    server.host.insert(std::pair<std::string, void*>("host", NULL));
    server.root.insert(std::pair<std::string, void*>("root", NULL));
    server.indexes.insert(std::pair<std::string, void*>("index", NULL));
    server.srvNames.insert(std::pair<std::string, void*>("server_names", NULL));
    server.errPages.insert(std::pair<std::string, void*>("error_pages", NULL));
    server.maxBodySize.insert(std::pair<std::string, void*>("max_body_size", NULL));
    server.location.insert(std::pair<std::string, void*>("location", NULL));
}

std::string config_reader(char** argv)
{
    std::ifstream file(argv[1]);
    std::stringstream buffer;
    if (!file.is_open())
        throw std::invalid_argument("no such file or permission denied");
    buffer << file.rdbuf();
    return buffer.str();
}

typeCode get_the_actual_type(std::string& attr)
{
    if (attr == PORT)
        return _int;
    if(attr == HOST || attr == ROOT)
        return _string;
    if(attr == INDEX || attr == SRVNAMES)
        return _vector;
    if(attr == ERRPAGES)
       return _map;
    if(attr == MAXBDSIZE)
        return _long;
    if(attr == LOCATION)
        return _l_config;
    return _none;
}

void* allocate_the_appropriate_type(std::string& attr)
{
    void* final_type = NULL;
    typeCode code = get_the_actual_type(attr);
    switch (code)
    {
        case _int:
            int* integer = new (int);
            final_type = integer;
            break;
        case _string:
            std::string* string = new (std::string);
            final_type = string;
            break;
        case _vector:
            std::vector<std::string>* vector = new (std::vector<std::string>);
            final_type = vector;
            break;
        case _map:
            std::map<int, std::string>* map = new (std::map<int, std::string>);
            final_type = vector;
            break;
        case _long:
            long* lng = new (long);
            final_type = lng;
            break;
        case _l_config:
            l_config* location = new (l_config);
            final_type = location;
            break;
        case _none:
            throw std::invalid_argument(attr + " unknown variable");
    }
    return final_type;
}

void give_attr_value(s_config& server,std::string value)
{
    std::vector<std::map<std::string, void*> > srvVars;
    std::vector<std::map<std::string, void*> >::iterator i;
    std::map<std::string, void*>::iterator it;

    srvVars.push_back(server.port);
    srvVars.push_back(server.host);
    srvVars.push_back(server.root);
    srvVars.push_back(server.indexes);
    srvVars.push_back(server.srvNames);
    srvVars.push_back(server.errPages);
    srvVars.push_back(server.maxBodySize);
    srvVars.push_back(server.location);

    for(i = srvVars.begin(); i != srvVars.end(); i++)
    {
        it = (*i).begin();
        if (*(it->second) = )
        {
            it->second = allocate_the_appropriate_type(var.first);
            break;
        }
    }
}


void store_variable_if_valid(s_config& server, std::pair<std::string, std::string>& var)
{
    std::vector<std::map<std::string, void*> > srvVars;
    std::vector<std::map<std::string, void*> >::iterator i;
    std::map<std::string, void*>::iterator it;

    srvVars.push_back(server.port);
    srvVars.push_back(server.host);
    srvVars.push_back(server.root);
    srvVars.push_back(server.indexes);
    srvVars.push_back(server.srvNames);
    srvVars.push_back(server.errPages);
    srvVars.push_back(server.maxBodySize);
    srvVars.push_back(server.location);

    for(i = srvVars.begin(); i != srvVars.end(); i++)
    {
        it = (*i).begin();
        if (it->first == var.first)
        {
            it->second = allocate_the_appropriate_type(var.first);
            break;
        }
    }
    if (var.second.empty())
    {
        delete it->second;
        throw std::invalid_argument(var.first + ": no given value");
    }
    give_attr_value(server, var.second);
}

void server_block(std::string& file)
{
    s_config server;
    attribute_initializer(server); // filling the server structure
    size_t i = -1;
    while(++i < file.size() && std::isspace(file.at(i)));
    file = file.substr(i, file.size() - i); // trimming whitespaces at the beginning 
    std::string attr;
    std::string value;
    std::vector<char> brackets;
    i = 0;
    while(i < file.size() && file.at(i) != ' ' && file.at(i) != '{' && file.at(i) != '\n')
    {
        attr.push_back(file.at(i));
        i++;
    }
    if (attr != server.server)
        throw std::invalid_argument("config file requires a server block: \nserver\n{\n  ...\n}");
    i--;
    while(++i < file.size() && file.at(i) == '\n');
    file = file.substr(i, file.size() - i);
    attr = "";
    value = "";
    i = 0;
    size_t j = 0;
    while(i < file.length())
    {
        if (file.at(i) == '{')
            brackets.push_back(file.at(i));
        else if (file.at(i) == '}')
        {
            if (brackets.empty())
                throw std::invalid_argument("no opening bracket to be close");
            brackets.pop_back();
        }
        else if (file.at(i) != ' ' && file.at(i) != '\n')
        {
            while(file.at(i) != ' ' && file.at(i) != '\n')
            {
                attr.push_back(file.at(i));
                i++;
            }
            while(file.at(i) == ' ')
                i++;
            j = i;
            while(file.at(i++) != '\n');
            value = file.substr(j, i - j - 1);
            std::pair<std::string, std::string> var(attr, value);
            store_variable_if_valid(server, var);
            attr = "";
            value = "";
        }
        i++;
    }
    if (!brackets.empty())
        throw std::invalid_argument("opened bracket no closed");
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
        std::string buffer = config_reader(argv);
        server_block(buffer);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}