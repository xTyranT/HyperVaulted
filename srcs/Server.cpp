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
        {
            file.close();
            return srvs;
        }
        srvs.push_back(sv);
    }
    file.close();
    return srvs;
}

std::vector<Server> getDefaultServer(void)
{
    std::vector<Server> sv;
    Server tmp;

    tmp.port = 8080;
    tmp.host = "localhost";
    tmp.root = "./";
    tmp.maxBodySize = 1024;
    Location loc;
    loc.path = "/";
    loc.methods.push_back("GET");
    loc.root = "./";
    tmp.location.push_back(loc);
    sv.push_back(tmp);
    return sv;
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
    cgiPaths = std::vector<std::pair<std::string, std::string> >();
    ret = std::string();
}

Location::Location(const Location& other)
{
    *this = other;
}

const Location& Location::operator=(const Location& other)
{
    path = other.path;
    methods = other.methods;
    root = other.root;
    indexes = other.indexes;
    autoIndex = other.autoIndex;
    upload = other.upload;
    cgi = other.cgi;
    uploadPath = other.uploadPath;
    cgiPaths = other.cgiPaths;
    ret = other.ret;
    return *this;
}

void Location::checkAndStoreLocationAttributes(std::vector<std::string> attr, std::ifstream& file)
{
    (void)file;
    std::vector<std::string>::iterator i = attr.begin();
    if (*i == METHODS)
    {
        if (attr.size() < 2)
        {
            file.close();
            throw std::invalid_argument("methods must have a value");
        }
        attr.erase(i);
        methods = attr;
        for (i = methods.begin(); i != methods.end(); i++)
        {
            if (*i != "GET" && *i != "POST" && *i != "DELETE")
            {
                file.close();
                throw std::invalid_argument("methods given not known");
            }
        }
        for (i = methods.begin() ; i != methods.end(); i++)
        {
            if (*i != "GET" && *i != "POST" && *i != "DELETE")
            {
                file.close();
                throw std::invalid_argument("methods given not known");
            }
            std::vector<std::string>::iterator j = i + 1;
            for(j = i + 1; j != methods.end(); j++)
            {
                if (*i == *j)
                {
                    file.close();
                    throw std::invalid_argument("methods shall not be duplicated");
                }
            }
        }
    }
    else if (*i == ROOT)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("root must be one path");
        }
        root = *(i + 1);
    }
    else if (*i == INDEX)
    {
        if (attr.size() < 2)
        {
            file.close();
            throw std::invalid_argument("index should at least have one value");
        }
        i++;
        for (; i != attr.end(); i++)
            indexes.push_back(*i);
    }
    else if (*i == AUTOINDEX)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("autoindex must be on or off");
        }
        if (*(i + 1) == "on")
            autoIndex = true;
        else if (*(i + 1) == "off")
            autoIndex = false;
        else
        {
            file.close();
            throw std::invalid_argument("autoindex can only be on or off");
        }
    }
    else if (*i == UPLOAD)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("upload must be on or off");
        }
        if (*(i + 1) == "on")
            upload = true;
        else if (*(i + 1) == "off")
            upload = false;
        else
        {
            file.close();
            throw std::invalid_argument("upload can only be on or off");
        }
    }
    else if (*i == CGI)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("cgi must be on or off");
        }
        if (*(i + 1) == "on")
            cgi = true;
        else if (*(i + 1) == "off")
            cgi = false;
        else
        {
            file.close();
            throw std::invalid_argument("cgi can only be on or off");
        }
    }
    else if (*i == UPLOADPATH)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("upload_path must be one valid path");
        }
        uploadPath = *(i + 1);
    }
    else if (*i == CGIPATH)
    {
        if (attr.size() != 3)
        {
            file.close();
            throw std::invalid_argument("cgi_path must file extension + one valid path");
        }
        i++;
        cgiPaths.push_back(std::pair<std::string, std::string>(*i, *(i + 1)));
    }
    else if (*i == RETURN_)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("return + <url>");
        }
        ret = *(i + 1);
    }
    else
    {
        file.close();
        throw std::invalid_argument(*(attr.begin()) + " unknown location variable");
    }
}

void Location::checkNecessaryAttributes(void)
{
    if (methods.empty())
        throw std::invalid_argument("methods must be specified with one of the following:   GET POST DELETE");
    std::vector<std::string>::iterator i = std::find(methods.begin(), methods.end(), "POST");
    if (i != methods.end() && upload == false)
        throw std::invalid_argument("location upload must be on if POST method is on");
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
    maxBodySize = long();
    location = std::vector<Location>();
    l_i = location.end();
}

Server::Server(const Server& other)
{
    *this = other;
}

const Server& Server::operator=(const Server& other)
{
    sockett = other.sockett;
    port = other.port;
    host = other.host;
    root = other.root;
    indexes = other.indexes;
    srvNames = other.srvNames;
    errPages = other.errPages;
    maxBodySize = other.maxBodySize;
    location = other.location;
    l_i = other.l_i;
    fd = other.fd;
    return *this;
}

bool Server::empty(void)
{
    bool var = true;
    if (maxBodySize || port || !host.empty()|| !root.empty() || !indexes.empty() 
        || !srvNames.empty() || !errPages.empty() || !location.empty())
        var = false;
    return var;
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
    std::cout << "max_body_size " << maxBodySize << std::endl;
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
        std::cout << "index: ";
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
            std::cout << i->cgiPaths[j].first << " | " << i->cgiPaths[j].second << "\n";
        std::cout << "redirecting link: ";
        std::cout << i->ret << std::endl;
    }
}

void Server::checkAndStoreServerAttributes(std::vector<std::string> attr, std::ifstream& file)
{
    std::vector<std::string>::iterator i = attr.begin();
    if (*i == PORT)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("listen only on one port");
        }
        port = std::atof((i + 1)->c_str());
        if (port < 1)
        {
            file.close();
            throw std::invalid_argument("port cannot have a negative number");
        }
    }
    else if (*i == HOST)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("host must have one value");
        }
        host = *(i + 1);
    }
    else if (*i == ROOT)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("root must be one path");
        }
        root = *(i + 1);
    }
    else if (*i == SRVNAMES)
    {
        if (attr.size() < 2)
        {
            file.close();
            throw std::invalid_argument("server_names should at least have one value");
        }
        i++;
        for (; i != attr.end(); i++)
            srvNames.push_back(*i);
    }
    else if (*i == INDEX)
    {
        if (attr.size() < 2)
        {
            file.close();
            throw std::invalid_argument("index should at least have one value");
        }
        i++;
        for (; i != attr.end(); i++)
            indexes.push_back(*i);
    }
    else if (*i == MAXBDSIZE)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("max_body_size must be one positive value");
        }
        if ((i + 1)->at(0) == '-')
        {
            file.close();
            throw std::invalid_argument("max_body_size cannot be zero or negative number");
        }
        unsigned long tmp = strtoul ((i + 1)->c_str(), NULL, 10);
        if ( std::numeric_limits<unsigned long>::max() < tmp)
            throw std::out_of_range( "max_body_size cannot exceed the unsigned long limits");
        maxBodySize = tmp;
        if (maxBodySize < 1)
        {
            file.close();
            throw std::invalid_argument("max_body_size cannot be zero or negative number");
        }
    }
    else if (*i == ERRPAGES)
    {
        if (attr.size() != 3)
        {
            file.close();
            throw std::invalid_argument("error_page syntax: error_page <error_code> <error_page_path>");
        }
        i++;
        int err = std::atoi(i->c_str());
        if (err < 1)
        {
            file.close();
            throw std::invalid_argument("error code should not be negative");
        }
        if ((i + 1)->substr(0, 2) != "./")
        {
            file.close();
            throw std::invalid_argument("error_page_path must be a relative path");
        }
        errPages.insert(std::pair<int, std::string>(err, *(i + 1)));
    }
    else if ( *i == LOCATION)
    {
        if (attr.size() != 2)
        {
            file.close();
            throw std::invalid_argument("location must have one path");
        }
        Location tmp;
        tmp.path = *(i + 1);
        std::string line;
        std::getline(file, line);
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (line != "{")
        {
            file.close();
            throw std::invalid_argument("location block: \nlocation <path>\n{\n  ...\n}");
        }
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
            tmp.checkAndStoreLocationAttributes(vec, file);
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
        }
        location.push_back(tmp);
    }
    else
    {
        file.close();
        throw std::invalid_argument(*(attr.begin()) + " unknown server variable");
    }
}

void Server::checkAndSetNecessaryAttributes(void)
{
    if (port < 0)
        throw std::invalid_argument("a listening port should bbe positive");
    if (root.empty())
        throw std::invalid_argument("a root is necessary");
    if (!location.empty())
    {
        for (std::vector<Location>::iterator i = location.begin(); i != location.end(); i++)
        {
            for(std::vector<Location>::iterator j = i + 1; j != location.end(); j++)
                if (i->path == j->path)
                    throw std::invalid_argument("duplicate location");
        }
        for (std::vector<Location>::iterator i = location.begin(); i != location.end(); i++)
        {
            i->checkNecessaryAttributes();
            if (i->root.empty())
                i->root = root;
            if (i->indexes.empty())
                i->indexes = indexes;
        }
    }
    if (!port)
        port = 80;
    if (!maxBodySize)
        maxBodySize = 1024;
    if (host.empty())
        host = "localhost";
    if (indexes.empty())
        indexes.push_back("index.html");
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
    {
        file.close();
        throw std::invalid_argument("config file requires a server block: \nserver\n{\n  ...\n}");
    }
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
            {
                file.close();
                throw std::invalid_argument("closing none opened bracket");
            }
            brackets.pop();
            break;
        }
        else if (!brackets.empty())
        {
            std::vector<std::string> vec = split(line);
            checkAndStoreServerAttributes(vec, file);
        }
        else
        {
            file.close();
            throw std::invalid_argument("config file requires a server block: \nserver\n{\n  ...\n}");
        }
    }
    if (!brackets.empty())
    {
        file.close();
        throw std::invalid_argument("opened bracket not closed");
    }
    if (location.empty())
    {
        file.close();
        throw std::invalid_argument("server block must have at least one location block");
    }
    checkAndSetNecessaryAttributes();
}

Server::~Server(void)
{

}