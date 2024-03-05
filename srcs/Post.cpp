
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <cmath>

int x = 1;
extern std::map<std::string, std::string>  mimeTypes;

int htd( std::string str )
{
    int dec = 0;

    std::istringstream dt( str );

    dt >> std::hex >> dec;

    return dec;
}

std::string itos()
{
    std::string name = "upload";
    std::stringstream s;
    s << name << x  << '.' ;
    x++;
    return s.str();
}

std::string gnExtencion( std::string contentType ,  std::string path )
{
    std::cout << path << std::endl;
    std::map<std::string, std::string>::iterator i;
    std::string ex;
    for ( i = mimeTypes.begin() ; i != mimeTypes.end(); i++ )
    {
        if ( contentType == i->second){

            ex = i->first;
            break;
        }
    }
    std::cout << path << std::endl;
    std::string s =  path + itos() + ex;
    return s;
}

void    ChunkedPost( Client & Clients , char *buff , int rd , Server & srv)
{
    size_t find = 0;
    std::string str;
    if ( !Clients.flag )
    {
        Clients.flag = true;
        std::string fname = gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath);
        Clients.postFile.open(fname.c_str(), std::ios::app );
        find = Clients.request.find("\r\n");
        str  = Clients.request.substr(0 , find);
        Clients.chunksize = htd( str );
        Clients.request = Clients.request.erase(0, find + 2);
        if ( Clients.chunksize < (int)Clients.request.size() )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request = Clients.request.erase( 0 , Clients.chunksize  + 2);
            find = Clients.request.find("\r\n");
            str  = Clients.request.substr(0 , find);
            Clients.chunksize = htd( str );
            Clients.request = Clients.request.erase(0, find + 2);
            
        }
    }
    else
    {
        Clients.request.append( buff , rd);
        if ( Clients.chunksize < (int)Clients.request.size() && Clients.request.find("\r\n", Clients.chunksize + 2) != std::string::npos )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request = Clients.request.erase( 0 , Clients.chunksize + 2 );
            find = Clients.request.find("\r\n");
            str  = Clients.request.substr(0 , find);
            Clients.chunksize = htd( str );
            Clients.request = Clients.request.erase(0, find + 2);
        }
    }
    if ( Clients.chunksize == 0 )
    {
        std::cout << "here 1" << std::endl;
        Clients.reqRes.returnCode = 201;
        Clients.reqRes.formTheResponse(srv);
        Clients.enf = true;
        Clients.postFile.close();
    }
}

void    Post( Client & Clients , char *buff , int rd , Server & srv)
{

    std::map<std::string, std::string>::iterator it;

    if ( Clients.reqRes.httpHeaders.find("Transfer-Encoding") != Clients.reqRes.httpHeaders.end() )
            ChunkedPost( Clients , buff , rd , srv);
    else {
    if ( !Clients.flag )
    {
        std::string fname =  gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath );
        Clients.postFile.open(fname.c_str(), std::ios::app );
        Clients.postFile.write(Clients.request.c_str(), Clients.request.size());
        Clients.flag = true;
        Clients.contentlength = atoi(Clients.reqRes.httpHeaders["Content-Length"].c_str());
    }
    else
    {
        Clients.postFile.write(buff, rd);
    }
    if ( Clients.sread >= Clients.contentlength )
    {
        std::cout << "here " << std::endl;
        Clients.reqRes.returnCode = 201;
        Clients.reqRes.formTheResponse(srv);
        Clients.enf = true;
        Clients.postFile.close();
    }
    }
}
