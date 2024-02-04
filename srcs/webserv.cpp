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
        
        multiplexing( sv );

        //for(std::vector<Server>::iterator i = sv.begin(); i != sv.end(); i++)
          //  i->printServerAttributes();
        // for(std::vector<Server>::iterator i = sv.begin(); i != sv.end(); i++)
        //     i->printServerAttributes();

        Request request;
        std::string  req;

        req = request.getRequest();
        request.requestParser(req);
        request.printRequestComponents();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}