#include "../includes/Server.hpp"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>


int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    try
    {
        if (argc != 2)
            throw std::invalid_argument("./HyperVaulted <config_file>");
        std::ifstream file(argv[1]);
        if (!file.is_open())
            throw std::invalid_argument("no such file or permission denied");
        std::vector<Server> sv = getAvailableServers(file);
        for(std::vector<Server>::iterator i = sv.begin(); i != sv.end(); i++)
            i->printServerAttributes();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}