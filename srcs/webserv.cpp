#include "../includes/Server.hpp"
#include "../includes/Request.hpp"

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
        if (sv.empty())
            sv = getDefaultServer();
        // for(size_t i = 0; i < sv.size(); i++)
        //     sv[i].printServerAttributes();
        fillMimeTypes();
        multiplexing(sv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}