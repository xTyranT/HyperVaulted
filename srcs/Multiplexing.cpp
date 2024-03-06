#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#define MAX_EVENTS 10

void    accept_connection( int efd , int fd, std::map<int , class Client> & Clients )
{
    Client cl;
    struct epoll_event event;
    struct sockaddr_in newcon;

    int len = sizeof( struct sockaddr_in);

    int cfd = accept( fd , reinterpret_cast< struct sockaddr * >(&newcon) , reinterpret_cast<socklen_t*>(&len));
    if ( cfd == -1 )
        std::cout << strerror(errno) << std::endl;
    cl.fd = cfd;
    cl.reqRes.sFd = fd;
    Clients[cfd] = cl;
    // gettimeofday(&cl.stime, 0);
    fcntl(cfd, F_SETFL, O_NONBLOCK);
    event.data.fd = cfd;
    event.events = EPOLLIN | EPOLLOUT;

    if ( epoll_ctl( efd , EPOLL_CTL_ADD , cfd , &event) == -1 )
        std::cout << strerror(errno) << std::endl;

}

void    multiplexing( std::vector<Server> & sv )
{
    char buff[1024];
    struct epoll_event ev, events[MAX_EVENTS];
    std::vector<int> sfds;
    std::map<int, class Client> Clients;

    int efd = epoll_create1(0);
    if ( efd == -1 )
        std::cout << "epoll_creat " << strerror(errno) << std::endl;
    for (size_t i = 0 ; i < sv.size() ; i++)
    {
        int sfd = socket(AF_INET, SOCK_STREAM, 0);
        if ( sfd == -1 )
            std::cout << "socket " << strerror(errno) << std::endl;
        sv[i].fd = sfd;
        sfds.push_back( sfd );
        int host_adlen = sizeof(sv[i].sockett);
        sv[i].sockett.sin_family = AF_INET;
        sv[i].sockett.sin_port = htons(sv[i].port);
        sv[i].sockett.sin_addr.s_addr = htonl(INADDR_ANY);
        int opt = 1;
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int));
        if ( bind(sfd, (struct sockaddr *)&(sv[i].sockett ), host_adlen) == -1)
            std::cout << "bind " << strerror(errno) << std::endl;

        if ( listen(sfd, SOMAXCONN) != 0)
            std::cout << "listen " << strerror(errno) << std::endl;
        ev.data.fd = sfd;
        ev.events = EPOLLIN | EPOLLET;
        if ( epoll_ctl(efd, EPOLL_CTL_ADD , sfd , &ev ) == -1)
            std::cout << "epoll_ctl " << strerror(errno) << std::endl;
    }

    while( 1 ){
        int wp = epoll_wait(efd , events , MAX_EVENTS , -1);
        if (wp == -1)
            std::cout << "epoll_wait " << strerror(errno) << std::endl;

        for ( int i = 0  ; i < wp ; i++ ) 
        {
            
            int fd = events[i].data.fd;
            std::vector<int>::iterator it = std::find(sfds.begin() , sfds.end(), fd);
            if ( it != sfds.end() )
                accept_connection( efd , fd , Clients);
            else if (((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ))
                        close(events[i].data.fd);
            else if ( events[i].events & EPOLLIN )
            {
                // gettimeofday(&Clients[fd].stime, 0);
                memset(buff, 0 , 1024);
                int rd = recv(fd, buff, 1023, 0);
                if (!Clients[fd].flag)
                    Clients[fd].sread = 0;
                Clients[fd].sread += rd;
                if ( rd == -1 )
                {
                    std::cout << "recv " << strerror(errno) << std::endl;
                    exit(EXIT_FAILURE);
                }
                if ( !Clients[fd].read ){
                    Clients[fd].request.append(buff, rd);
                    size_t find = Clients[fd].request.find("\r\n\r\n");
                    if ( find != std::string::npos && !Clients[fd].read )
                    {
                        Clients[fd].read = true;
                        Clients[fd].requestHeader = Clients[fd].request.substr(0 , find + 4);
                        Clients[fd].reqRes.requestParser(Clients[fd].requestHeader, sv);
                        Clients[fd].request = Clients[fd].request.erase(0, find + 4);
                        Clients[fd].sread -= find + 4;
                    }
                }
                if ( Clients[fd].reqRes.Component.method == "POST" && !Clients[fd].enf)
                {
                    if ( Clients[fd].reqRes.returnCode != 200 && Clients[fd].reqRes.returnCode != 301 )
                        Clients[fd].enf = true;
                    else
                        Post( Clients[fd] , buff , rd , sv[Clients[fd].reqRes.sindx]);
                }
                else if ( Clients[fd].reqRes.Component.method != "POST" )
                    Clients[fd].enf = true;
            }
            else if ( events[i].events & EPOLLOUT && Clients[fd].enf)
            {
                std::cout << "epoll out \n";
                // gettimeofday(&Clients[fd].stime, 0);
                 std::cout << "fd = " << fd  << Clients[fd].resred << std::endl;
                if( !Clients[fd].resred )
                {
                    Clients[fd].resred = true;
                    Clients[fd].resFile.open(Clients[fd].reqRes.file.c_str());
                    std::cout << "res file = " << Clients[fd].reqRes.file.c_str() << std::endl;
                    if ( !Clients[fd].resFile.is_open() )
                    {
                        std::cout << "open " << strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    write(fd, Clients[fd].reqRes.responseBuffer.c_str() , Clients[fd].reqRes.responseBuffer.size());
                    
               }
               else{
                    memset(buff, 0 , 1024);
                    Clients[fd].resFile.read(buff , 1023);
                    write(fd, buff, Clients[fd].resFile.gcount());
                }
                if ( Clients[fd].resFile.eof())
                {
                    if ( !epoll_ctl( efd , EPOLL_CTL_DEL , fd , &events[i]) )
                    {
                        std::cout << "ctl del " << std::endl;
                    }
                    // Clients.
                    close(fd);
                    Clients.erase(fd);
                }
            }
            // gettimeofday(&Clients[fd].etime, 0);
            // if ( Clients[fd].etime.tv_sec - Clients[fd].stime.tv_sec >= 10 )
            // {
            //     Clients[fd].enf = true;
            //     Clients[fd].reqRes.returnCode = 408;
            //     std::cout << Clients[fd].reqRes.sindx << std::endl;
            //     Clients[fd].reqRes.openErrorPage(sv[Clients[fd].reqRes.sindx]);
            //     Clients[fd].reqRes.formTheResponse(sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes.matchedLocation);
            // }
        }
    }
}
