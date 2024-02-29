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

void Cgi::cgiCaller(Server& srv, Location& req)
{
    pid_t pid;
    int status;

    (void)srv;
    (void)req;
    pid = fork();
    if (pid == 0)
    {
        // execve(std::find(req.cgiPaths), NULL, NULL);
    }
    else if (pid < 0)
    {
        std::cerr << "fork failed" << std::endl;
    }
    else
    {
        waitpid(pid, &status, 0);
    }
}