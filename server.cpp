#include "server.hpp"


Server::Server(int fd) : _fd(fd) , _buffer("\0") , _bufferLength(0)
{
    std::cout << GREEN << "Server Constructor Called" << RESET << std::endl;
}

Server::~Server()
{
    std::cout << RED << "Server Destructor Called" << RESET << std::endl;
}

int Server::getFd() const
{
    return this->_fd;
}

void Server::setFd(int fd)
{
    this->_fd = fd;
}

int Server::fdVal() const
{
    return (_fd);
}

char* Server::getBuffer()
{
    return this->_buffer;
}

size_t Server::getBufferLen() const
{
    return this->_bufferLength;
}

void Server::setBufferLen(size_t len) 
{
    _bufferLength = len;
}

void Server::clearBuffer()
{
    memset(_buffer, 0 , sizeof(_buffer));
    _bufferLength = 0;
}

int Server::createSocket()
{
    //int socket(int domain, int type, int protocol);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << RED "Failed to create socket" << RESET << std::endl;
        exit (EXIT_FAILURE);
    }
    std::cout << GREEN << "Socket Created" << RESET << std::endl;
    return sockfd;
}

void Server::bindSocket(int sockfd)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; //(0.0. 0.0) any address for binding
    serverAddr.sin_port = htons(PORT); // convert to network byte order.

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << RED << "Failed to bind socket." << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << GREEN << "Bind socket has been successfully bound." << RESET << std::endl;
}

void Server::listenSocket(int sockfd)
{
    if (listen(sockfd , MAX_CLIENTS) == - 1)
    {
        std::cerr << RED << "Failed to listen socket." << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << GREEN << "Successfully listen " << RESET << std::endl;
}

int Server::acceptConection(int sockfd)
{
    struct sockaddr_in clientAddr; //hold clientAddr information
    socklen_t clientLen = sizeof (clientAddr);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int clientFd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientFd == -1){
        std::cerr << RED << "Failed to accept << " << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << GREEN << "Successfully accept " << RESET << std::endl;
    return clientFd; //Return the new socket descriptor for communication with the client.
}

void Server::runServer()
{
    int sockfd = createSocket();
    bindSocket(sockfd);
    listenSocket(sockfd);

    std::vector<Server> users;
    std::vector<pollfd> fds; // move to class 

    while(1)
    {
        //int poll(representing a FD, number of FD, timeout);
        int numFds = poll(fds.data(), fds.size(), -1);
        if (numFds = -1) {
            std::cout << RED "failed to poll" << RESET << std::endl;
            exit (EXIT_FAILURE);
        }
        for (int i = 0 ; i < fds.size(); i++){
            if (fds[i].revents & POLLIN) { //data that can be read without blocking AND can safely read operation be on it
                if (fds[i].fd == sockfd) {
                    int clientFd = acceptConection(sockfd);
                    pollfd tmp2 = (clientFd, POLLIN, 0);
                    fds.push_back(tmp2); // ??
                    users.push_back(Server(clientFd));
                    std::cout << BL|UE << "new client connected FD:" << clientFd << RESET << stdendl;
                }
                else{
                    char buffer[4096];
                    int byteRead = read(fds[i].fd buffer, sizeof(buffer));
                    if (byteRead <= 0){
                        std::cout << RED << "Client disconnected FD :" << fds[i].fd << RESET << stdendl;
                    }
                }
            }
        }
    }



}
