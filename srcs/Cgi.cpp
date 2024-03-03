#include "../includes/Cgi.hpp"

Cgi::Cgi()
{

}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

const Cgi& Cgi::operator=(const Cgi& other)
{
    (void)other;
    return *this;
}

Cgi::~Cgi()
{

}

void Cgi::cgiCaller(Server& srv, Location& req, Response& res)
{
    std::cout << "CGI CALLED" << std::endl;
    pid_t pid;
    int status;
    char **env = new char*[7];
    std::string tmp[6] = { "REQUEST_METHOD", "QUERY_STRING", "REDIRECT_STATUS=200", "SCRIPT_FILENAME", "CONTENT_TYPE", "HTTP_COOKIE"};
    env[0] = strdup((tmp[0] + "=" + res.Component.method).c_str());
    env[1] = strdup((tmp[1] + "=" + res.Component.path.substr(res.Component.path.find("?") + 1)).c_str());
    env[2] = strdup(tmp[2].c_str());
    env[3] = strdup((tmp[3] + "=" + srv.root + res.Component.path).c_str());
    std::map<std::string, std::string>::iterator it = res.httpHeaders.find("Content-Type");
    if (it != res.httpHeaders.end())
        env[4] = strdup((tmp[4] + "=" + it->second).c_str());
    else
        env[4] = strdup((tmp[4] + "=").c_str());
    std::map<std::string, std::string>::iterator it2 = res.httpHeaders.find("Cookie");
    if (it2 != res.httpHeaders.end())
        env[5] = strdup((tmp[5] + "=" + res.httpHeaders.find("Cookie")->second).c_str());
    else
        env[5] = strdup((tmp[5] + "=").c_str());
    env[6] = NULL;
    int fd = open("cgi", O_CREAT | O_RDWR, 0666);
    char **argv = new char*[3];
    std::vector<std::pair<std::string, std::string> >::iterator it3 = req.cgiPaths.begin();
    std::string absPath = realpath((srv.root + res.Component.path).c_str(), NULL);
    std::cout << "absPath: " << absPath + res.Component.path << std::endl;
    for(; it3 != req.cgiPaths.end(); it3++)
    {
        if (absPath + res.Component.path == it3->first)
            break;
    }
    if (it3 == req.cgiPaths.end())
    {
        std::cerr << "No CGI path found" << std::endl;
        return;
    }
    argv[0] = strdup(it->second.c_str());
    argv[1] = strdup((srv.root + res.Component.path).c_str());
    argv[2] = NULL;
    std::cout << argv[0] << " " << argv[1] << std::endl;
    pid = fork();
    if (pid < 0)
    {
        std::cerr << "fork failed" << std::endl;
    }
    else if (pid == 0)
    {
        dup2(fd, 1);
        
        execve(argv[0], argv, env);
    }
    waitpid(pid, &status, 0);
    close(fd);
}