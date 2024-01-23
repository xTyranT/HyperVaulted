
#include "../includes/Server.hpp"
#include "../includes/stringManipulators.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    try
    {
        if (argc != 2)
            throw std::invalid_argument("./HyperVaulted <config_file>");
        Server server;
        std::ifstream file(argv[1]);
        if (!file.is_open())
            throw std::invalid_argument("no such file or permission denied");
        server.serverBlock(file);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}