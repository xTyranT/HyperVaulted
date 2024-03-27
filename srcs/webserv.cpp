#include "../includes/Server.hpp"
#include "../includes/Request.hpp"

void checkNeceesaryTools()
{
    std::ifstream file("./ErrorPages/error_page.html");
    if (!file.is_open())
        throw std::invalid_argument("ErrorPage file not found or permission denied");
    file.close();
    std::ifstream file1("./tools/mime.types");
    if (!file1.is_open())
        throw std::invalid_argument("MimeTypes file not found or permission denied");
    file1.close();
}

void serversChecker(std::vector<Server>& sv)
{
    for(std::vector<Server>::iterator i = sv.begin(); i != sv.end(); i++)
    {
        for(std::vector<Server>::iterator j = i + 1; j != sv.end(); j++)
        {
            if (i->port == j->port && i->host == j->host && i->srvNames == j->srvNames)
                throw std::invalid_argument("Duplicate server found");
        }
    }
}

int main(int argc, char** argv)
{
    try
    {
        checkNeceesaryTools();
        std::vector<Server> sv;
        if (argc == 1)
            sv = getDefaultServer();
        else if (argc == 2)
        {
            std::ifstream file(argv[1]);
            if (!file.is_open())
                throw std::invalid_argument("no such file or permission denied");
            sv = getAvailableServers(file);
            if (sv.empty())
                throw std::invalid_argument("no server found");
        }
        else
            throw std::invalid_argument("too many arguments");
        serversChecker(sv);
        fillMimeTypes();
        multiplexing(sv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}