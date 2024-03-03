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
    // char* env[24] = {
    //     "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
    //     "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
    //     "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
    //     "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
    //     "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
    //     "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
    //     "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",      
    //     "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
    //     "SERVER_SIGNATURE","SERVER_SOFTWARE" };

    (void)srv;
    (void)req;
    // (void)env;
    pid = fork();
    if (pid == 0)
    {
        // execve( NULL, NULL);
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