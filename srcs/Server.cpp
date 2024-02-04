#include "../includes/Server.hpp"

std::vector<Server> getAvailableServers(std::ifstream& file)
{
    std::vector<Server> srvs;
    std::string line;

    while(true)
    {
        Server sv;
        sv.serverBlock(file);
        if (sv.empty())
            return srvs;
        srvs.push_back(sv);
    }
    return srvs;
}

Location::Location(void)
{
    path = std::string();
    methods = std::vector<std::string>();
    root = std::string();
    indexes = std::vector<std::string>();
    autoIndex = false;
    upload = false;
    cgi = false;
    uploadPath = std::string();
    cgiPaths = std::vector<std::string>();
    ret.insert(std::pair<int, std::string>(301, "http://localhost:8080"));
}

void Location::checkAndStoreLocationAttributes(std::vector<std::string> attr)
{
    std::vector<std::string>::iterator i = attr.begin();
    if (*i == METHODS)
    {
        if (attr.size() < 2)
            throw std::invalid_argument("methods must have a value");
        attr.erase(i);
        methods = attr;
        for (i = methods.begin(); i != methods.end(); i++)
        {
            if (*i != "GET" && *i != "POST" && *i != "DELETE")
                throw std::invalid_argument("methods given not known");
        }
        for (i = methods.begin() ; i != methods.end(); i++)
        {
            if (*i != "GET" && *i != "POST" && *i != "DELETE")
                throw std::invalid_argument("methods given not known");
            std::vector<std::string>::iterator j = i + 1;
            for(j = i + 1; j != methods.end(); j++)
            {
                if (*i == *j)
                    throw std::invalid_argument("methods shall not be duplicated");
            }
        }
    }
    else if (*i == ROOT)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("root must be one path");
        root = *(i + 1);
    }
    else if (*i == INDEX)
    {
        if (attr.size() < 2)
            throw std::invalid_argument("index should at least have one value");
        i++;
        for (i = i; i != attr.end(); i++)
            indexes.push_back(*i);
    }
    else if (*i == AUTOINDEX)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("autoindex must be on or off");
        if (*(i + 1) == "on")
            autoIndex = true;
        else if (*(i + 1) == "off")
            autoIndex = false;
        else
            throw std::invalid_argument("autoindex can only be on or off");
    }
    else if (*i == UPLOAD)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("upload must be on or off");
        if (*(i + 1) == "on")
            upload = true;
        else if (*(i + 1) == "off")
            upload = false;
        else
            throw std::invalid_argument("upload can only be on or off");
    }
    else if (*i == CGI)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("cgi must be on or off");
        if (*(i + 1) == "on")
            cgi = true;
        else if (*(i + 1) == "off")
            cgi = false;
        else
            throw std::invalid_argument("cgi can only be on or off");
    }
    else if (*i == UPLOADPATH)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("upload_path must be one valid path");
        uploadPath = *(i + 1);
    }
    else if (*i == CGIPATH)
    {
        if (attr.size() < 2)
            throw std::invalid_argument("cgi_path must file extension + one valid path");
        i++;
        for (i = i; i != attr.end(); i++)
            cgiPaths.push_back(*i);
    }
    else if (*i == RETURN_)
    {
        if (attr.size() != 3)
            throw std::invalid_argument("return must code + url");
        std::pair<int, std::string> tmp;
        tmp.first = std::atoi((i + 1)->c_str());
        if (tmp.first != 301)
            throw std::invalid_argument("return should be only 301");
        tmp.second = *(i + 2);
        ret[301] = tmp.second;
    }
    else
        throw std::invalid_argument(*(attr.begin()) + " unknown location variable");
}

void Location::checkNecessaryAttributes(void)
{
    if (methods.size() == 0)
        throw std::invalid_argument("methods must be specified with one of the following:   GET POST DELETE");
    if (root.empty())
        throw std::invalid_argument("location root must be specified");
    if (upload == true && uploadPath.empty())
        throw std::invalid_argument("location upload_path must be specified");
    if(upload == false && !uploadPath.empty())
        throw std::invalid_argument("location upload_path must be specified only if upload is on");
    if (cgi == true && cgiPaths.empty())
        throw std::invalid_argument("location cgi_path must be specified");
    if(cgi == false && !cgiPaths.empty())
        throw std::invalid_argument("location cgi_path must be specified only if cgi is on");
}

Location::~Location(void)
{

}

Server::Server(void)
{
    port = int();
    host = std::string();
    root = std::string();
    indexes = std::vector<std::string>();
    srvNames = std::vector<std::string>();
    errPages = std::map<int, std::string>();
    generateErrorPages();
    maxBodySize = 1024;
    location = std::vector<Location>();
    l_i = location.begin();
}

bool Server::empty(void)
{
    bool var = true;
    if (port || !host.empty() || !root.empty() || !indexes.empty() || !srvNames.empty() || !location.empty())
        var = false;
    if(maxBodySize != 1024)
        var = false;
    return var;
}

void Server::generateErrorPages(void)
{
    errPages.insert(std::pair<int, std::string>(400, "./errorPages/400.html"));
    errPages.insert(std::pair<int, std::string>(403, "./errorPages/403.html"));
    errPages.insert(std::pair<int, std::string>(404, "./errorPages/404.html"));
    errPages.insert(std::pair<int, std::string>(405, "./errorPages/405.html"));
    errPages.insert(std::pair<int, std::string>(500, "./errorPages/500.html"));
    errPages.insert(std::pair<int, std::string>(501, "./errorPages/501.html"));
}

void Server::printServerAttributes(void)
{
    std::cout << "-------------SERVER VAR-------------\n";
    std::cout << "port: " << port << std::endl;
    std::cout << "host: " << host << std::endl;
    std::cout << "root: "<< root << std::endl;
    std::cout << "index: ";
    for (size_t i = 0; i < indexes.size(); i++)
    {
        std::cout << indexes[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "server_name: ";
    for (size_t i = 0; i < srvNames.size(); i++)
        std::cout << srvNames[i] << " ";
    std::cout << std::endl;
    std::cout << "---error_pages---\n";
    for (std::map<int, std::string>::iterator i = errPages.begin(); i != errPages.end(); i++)
        std::cout << i->first << " | " << i->second << std::endl;
    std::cout << maxBodySize << std::endl;
    for (std::vector<Location>::iterator i = location.begin(); i != location.end(); i++)
    {
        std::cout << "-------------LOCATION VAR-------------\n";
        std::cout << "path: " << i->path << std::endl;
        std::cout << "methods: ";
        for (size_t j = 0; j < i->methods.size(); j++)
            std::cout << i->methods[j] << " ";
        std::cout << std::endl;
        std::cout << "root: ";
        std::cout <<  i->root << std::endl;
        for (size_t j = 0; j < i->indexes.size(); j++)
            std::cout << i->indexes[j] << " ";
        std::cout << std::endl;
        std::string line;
        (i->autoIndex) ? line = "autoindex is on" : line = "autoindex is off";
        std::cout << line << std::endl;
        (i->upload) ? line = "upload is on" : line = "upload is off";
        std::cout << line<< std::endl;
        (i->cgi) ? line = "cgi is on" : line = "cgi is off";
        std::cout << line << std::endl;
        std::cout << "upload_path: " << i->uploadPath << std::endl;
        std::cout << "cgi_paths: ";
        for (size_t j = 0; j < i->cgiPaths.size(); j++)
            std::cout << i->cgiPaths[j] << " ";
        std::cout << std::endl;
        std::cout << "redirecting link: ";
        for (std::map<int, std::string>::iterator j = i->ret.begin(); j != i->ret.end(); j++)
            std::cout << j->first << " | " << j->second << std::endl;
    }
}

void Server::checkAndStoreServerAttributes(std::vector<std::string> attr, std::ifstream& file)
{
    std::vector<std::string>::iterator i = attr.begin();
    if (*i == PORT)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("listen only on one port");
        port = std::atof((i + 1)->c_str());
        if (port < 1)
            throw std::invalid_argument("port cannot have a negative number");
    }
    else if (*i == HOST)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("host must have one value");
        host = *(i + 1);
    }
    else if (*i == ROOT)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("root must be one path");
        root = *(i + 1);
    }
    else if (*i == SRVNAMES)
    {
        if (attr.size() < 2)
            throw std::invalid_argument("server_names should at least have one value");
        i++;
        for (i = i; i != attr.end(); i++)
            srvNames.push_back(*i);
    }
    else if (*i == INDEX)
    {
        if (attr.size() < 2)
            throw std::invalid_argument("index should at least have one value");
        i++;
        for (i = i; i != attr.end(); i++)
            indexes.push_back(*i);
    }
    else if (*i == MAXBDSIZE)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("max_body_size must be one positive value");
        maxBodySize = std::atof((i + 1)->c_str());
        if (maxBodySize < 1)
            throw std::invalid_argument("max_body_size cannot have a negative number");
    }
    else if (*i == ERRPAGES)
    {
        i++;
        int err = std::atoi(i->c_str());
        switch (err)
        {
            case 400:
                errPages.insert(std::pair<int, std::string>(400, *(i + 1)));
                break;
            case 403:
                errPages.insert(std::pair<int, std::string>(403, *(i + 1)));
                break;
            case 404:
                errPages.insert(std::pair<int, std::string>(403, *(i + 1)));
                break;
            case 405:
                errPages.insert(std::pair<int, std::string>(405, *(i + 1)));
                break;
            case 500:
                errPages.insert(std::pair<int, std::string>(500, *(i + 1)));
                break;
            case 501:
                errPages.insert(std::pair<int, std::string>(501, *(i + 1)));
                break;
            default:
                throw std::invalid_argument("error_page is not valid");
        }
    }
    else if ( *i == LOCATION)
    {
        if (attr.size() != 2)
            throw std::invalid_argument("location must have one path");
        Location tmp;
        tmp.path = *(i + 1);
        std::string line;
        std::getline(file, line);
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (line != "{")
            throw std::invalid_argument("location block: \nlocation <path>\n{\n  ...\n}");
        while(line != "}")
        {
            std::getline(file, line);
            strtrim(line, "\n ");
            if (line.empty() || line.at(0) == '#')
                continue;
            std::string temp = line;
            std::string checker = line;
            checker.erase(remove(checker.begin(), checker.end(), ' '), checker.end());
            if (checker == "}")
                break;
            std::vector<std::string> vec = split(line);
            tmp.checkAndStoreLocationAttributes(vec);
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
        }
        location.push_back(tmp);
    }
    else
        throw std::invalid_argument(*(attr.begin()) + " unknown server variable");
}

void Server::checkNecessaryAttributes(void)
{
    if (port < 1)
        throw std::invalid_argument("a listening port is necessary");
    if (root.size() == 0)
        throw std::invalid_argument("a root is necessary");
    if (location.size() > 0)
    {
        for (std::vector<Location>::iterator i = location.begin(); i != location.end(); i++)
            i->checkNecessaryAttributes();
    }
}

void Server::serverBlock(std::ifstream& file)
{
    std::string line;
    std::stack<std::string> brackets;
    std::string tmp;

    std::getline(file, line);
    while (line.empty())
    {
        std::getline(file, line);
        if (file.eof())
            return;
        strtrim(line, "\n ");
    }
    strtrim(line, "\n ");
    if (line != SERVER)
        throw std::invalid_argument("config file requires a server block: \nserver\n{\n  ...\n}");
    while(std::getline(file, line))
    {
        strtrim(line, "\n ");
        if (line.empty() || line.at(0) == '#')
            continue;
        else if (line == "{")
            brackets.push(line);
        else if (line == "}")
        {
            if (brackets.empty())
                throw std::invalid_argument("closing none opened bracket");
            brackets.pop();
            break;
        }
        else
        {
            std::vector<std::string> vec = split(line);
            checkAndStoreServerAttributes(vec, file);
        }
    }
    if (!brackets.empty())
        throw std::invalid_argument("opened bracket not closed");
    checkNecessaryAttributes();
}

Server::~Server(void)
{

}