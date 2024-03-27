#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

void    accept_connection( int efd , int fd, std::map<int , class Client> & Clients )
{
    Client cl;
    struct epoll_event event;
    struct sockaddr_in newcon;

    int len = sizeof( struct sockaddr_in);

    int cfd = accept( fd , (struct sockaddr *)&(newcon) , (socklen_t*)&(len));
    if ( cfd == -1 )
    {
        std::cout << strerror(errno) << std::endl;
        return;
    }
    cl.fd = cfd;
    cl.reqRes.sFd = fd;
    cl.start = clock();
    Clients[cfd] = cl;
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
    {
        std::cout << "epoll_creat " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0 ; i < sv.size() ; i++)
    {
        int sfd = socket(AF_INET, SOCK_STREAM, 0);
        if ( sfd == -1 )
        {
            std::cout << "socket " << strerror(errno) << std::endl;
            sv.erase(sv.begin() + i);
            continue;
        }
        sv[i].fd = sfd;
        sfds.push_back( sfd );
        int host_adlen = sizeof(sv[i].sockett);
        sv[i].sockett.sin_family = AF_INET;
        sv[i].sockett.sin_port = htons(sv[i].port);
        sv[i].sockett.sin_addr.s_addr = htonl(INADDR_ANY);
        int opt = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(int)) == -1)
            std::cout << "setsockopt " << strerror(errno) << std::endl;
        if ( bind(sfd, (struct sockaddr *)&(sv[i].sockett ), host_adlen) == -1)
            std::cout << "bind " << strerror(errno) << std::endl;
        if ( listen(sfd, SOMAXCONN) != 0)
            std::cout << "listen " << strerror(errno) << std::endl;
        ev.data.fd = sfd;
        ev.events = EPOLLIN;
        if ( epoll_ctl(efd, EPOLL_CTL_ADD , sfd , &ev ) == -1)
            std::cout << "epoll_ctl " << strerror(errno) << std::endl;
    }

    signal(SIGPIPE, SIG_IGN);

    while( 1 ){
        int wp = epoll_wait(efd , events , MAX_EVENTS , -1);
        if (wp == -1)
            std::cout << "epoll_wait " << strerror(errno) << std::endl;
        for ( int i = 0  ; i < wp ; i++ ) 
        {
            int fd = events[i].data.fd;
            std::vector<int>::iterator it = std::find(sfds.begin() , sfds.end(), fd);
            if ( it != sfds.end() ){
                accept_connection( efd , fd , Clients);
                continue;
            }
            else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP))
            {
                epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                Clients.erase(fd);
                continue;
            }
            if ( events[i].events & EPOLLIN && !Clients[fd].enf )
            {
                Clients[fd].start = clock();
                memset(buff, 0 , 1024);
                int rd = recv(fd, buff, 1023, 0);
                Clients[fd].sread += rd;
                if ( rd == -1  || rd == 0) {
                    if ( Clients[fd].reqRes.cgiProcessing) {
                        kill(Clients[fd].cgi.pid, SIGKILL);
                        waitpid(Clients[fd].cgi.pid, NULL, 0);
                    }
                    epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                    Clients[fd].postFile.close();
                    close(fd);
                    Clients.erase(fd);
                    continue;

                }
                else if ( !Clients[fd].read ){
                    Clients[fd].request.append(buff, rd);
                    size_t find = Clients[fd].request.find("\r\n\r\n");
                    if ( find != std::string::npos && !Clients[fd].read )
                    {
                        Clients[fd].read = true;
                        Clients[fd].requestHeader = Clients[fd].request.substr(0 , find + 4);
                        Clients[fd].reqRes.requestParser(Clients[fd].requestHeader, sv, Clients[fd].enf);
                        Clients[fd].request = Clients[fd].request.erase(0, find + 4);
                        Clients[fd].sread -= find + 4;
                    }
                }
                if ( Clients.find(fd) != Clients.end() && Clients[fd].reqRes.Component.method == "POST" && !Clients[fd].enf)
                {
                    if ( Clients[fd].reqRes.returnCode != 200)
                        Clients[fd].enf = true;
                    else
                        Post( Clients[fd] , buff , rd, sv[Clients[fd].reqRes.sindx], Clients[fd].cgi, Clients[fd].enf);
                }
                else if ( Clients.find(fd) != Clients.end() && Clients[fd].reqRes.Component.method != "POST" && Clients[fd].read )
                {
                    if ( Clients[fd].reqRes.returnCode != 200)
                        Clients[fd].enf = true;
                    else
                    {
                        if (Clients[fd].reqRes.Component.method == "GET")
                            Get(Clients[fd].reqRes.matchedLocation, sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes, Clients[fd].cgi, Clients[fd].enf);
                        else if (Clients[fd].reqRes.Component.method == "DELETE")
                            Delete(Clients[fd].reqRes.matchedLocation, sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes, Clients[fd].enf);
                        if (!Clients[fd].reqRes.matchedLocation.cgi)
                            Clients[fd].enf = true;
                    }
                }
            }
            else if ( events[i].events & EPOLLOUT && Clients[fd].enf && !Clients[fd].reqRes.cgiProcessing) {
                Clients[fd].start = clock();
                if( !Clients[fd].resred ) {
                    Clients[fd].resred = true;
                    Clients[fd].resFile.open(Clients[fd].reqRes.file.c_str());
                    int wr = write(fd, Clients[fd].reqRes.responseBuffer.c_str() , Clients[fd].reqRes.responseBuffer.size());
                    if ( wr == -1 || wr == 0 ) {
                        epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                        Clients[fd].resFile.close();
                        close(fd);
                        Clients.erase(fd);
                        continue;
                    }
                }
                else {

                    memset(buff, 0 , 1024);
                    Clients[fd].resFile.read(buff , 1023);
                    int wr = write(fd, buff, Clients[fd].resFile.gcount());
                    if ( wr == -1 ) {
                        epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                        Clients[fd].resFile.close();
                        close(fd);
                        Clients.erase(fd);
                        continue;
                    }
                    if ( wr < Clients[fd].resFile.gcount() )
                        Clients[fd].resFile.seekg( wr - Clients[fd].resFile.gcount() , std::ios_base::cur );
                }
                if ((Clients[fd].resFile.eof() || Clients[fd].reqRes.returnCode == 301 ||  Clients[fd].reqRes.returnCode == 204)) {
                    if ( Clients[fd].reqRes.returnCode == 504 ) {
                        kill(Clients[fd].cgi.pid, SIGKILL);
                        waitpid(Clients[fd].cgi.pid, NULL, 0);
                    }
                    if (Clients[fd].reqRes.cgi)
                        remove(Clients[fd].reqRes.file.c_str());
                    epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                    Clients[fd].resFile.close();
                    Clients.erase(fd);
                    close(fd);
                    continue;
                }
            }
            else if (Clients[fd].reqRes.cgiProcessing ) {
                Clients[fd].cgi.cgiCaller(sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes.matchedLocation, Clients[fd].reqRes, Clients[fd].enf);
                if (Clients[fd].cgi.pid == Clients[fd].cgi.cgiPid)
                    Clients[fd].cgi.formCgiResponse(sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes.matchedLocation, Clients[fd].reqRes);
                if (Clients[fd].reqRes.Component.method == "POST" && WEXITSTATUS(Clients[fd].cgi.status) == EXIT_NO_CGI)
                {
                    Clients[fd].reqRes.returnCode = 201;
                    Clients[fd].reqRes.openErrorPage(sv[Clients[fd].reqRes.sindx]);
                    Clients[fd].reqRes.formTheResponse(sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes.matchedLocation);
                }
                
            }
            if ( Clients.find(fd) != Clients.end() &&  (float)( (clock() / CLOCKS_PER_SEC) - (Clients[fd].start / CLOCKS_PER_SEC))  > 30) {
                Clients[fd].enf = true;
                Clients[fd].reqRes.returnCode = 408;
                for(std::vector<Server>::iterator i = sv.begin(); i != sv.end(); i++) {
                    if (Clients[fd].reqRes.sFd == i->fd) {
                        Clients[fd].reqRes.sindx = i - sv.begin();
                        break;
                    }
                }
                Clients[fd].reqRes.openErrorPage(sv[Clients[fd].reqRes.sindx]);
                Clients[fd].reqRes.formTheResponse(sv[Clients[fd].reqRes.sindx], Clients[fd].reqRes.matchedLocation);
            }
        }
    }
}
