
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

std::string gnExtencion( std::string contentType )
{
    std::string s = contentType.substr(contentType.find("/") + 1);
    return s;
}

void    Post( Client & Clients , char *buff , int rd )
{
    if (!Clients.flag)
    {
        std::string buffer(buff,rd);
        std::string fname =  "upload." + gnExtencion( Clients.parsedRequest.httpHeaders["Content-Type"]);
        Clients.postFile.open(fname.c_str(), std::ios::app | std::ios::binary);
        // std::cout << "TRRRRUUUUEEE" << std::endl;
        // Clients.first = true;
        // file.open(fname.c_str() , std::ios::app);
        // if (!file.is_open())
        //     std::cout << "here " << std::endl;
        //  Clients.filename = fname;
        Clients.postFile.write(Clients.request.c_str(), Clients.request.size());
        Clients.flag = true;
        std::cout << atoi(Clients.parsedRequest.httpHeaders["Content-Length"].c_str()) << std::endl;
    }
    else
    {
        Clients.postFile.write(buff, rd);
    }
    if (Clients.sread == atoi(Clients.parsedRequest.httpHeaders["Content-Length"].c_str()))
    { 
        std::cout << "rere" << std::endl;
        Clients.enf = true;
        Clients.postFile.close();
    }
}
