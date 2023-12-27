#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

using namespace std;

const int PORT = 6666;       // IRC server port number
const int MAX_CLIENTS = 4096;  // Maximum number of clients to support

class User {
public:
    User(int fd) : fd_(fd), buffer_("\0"), buffer_len_(0) {}
    ~User() {}

    int fd() const { return fd_; }
    void set_fd(int fd) { fd_ = fd; }
    // int getFd() const {return fd_;};

    char* buffer() { return buffer_; }
    size_t buffer_len() const { return buffer_len_; }
    void set_buffer_len(size_t len) { buffer_len_ = len; }

    void clear_buffer() {
        memset(buffer_, 0, sizeof(buffer_));
        buffer_len_ = 0;
    }

private:
    int fd_;
    char buffer_[4096];
    size_t buffer_len_;
};

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Failed to create socket." << endl;
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void bind_socket(int sockfd) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Failed to bind socket." << endl;
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int sockfd) {
    if (listen(sockfd, MAX_CLIENTS) == -1)
    {
        cerr << "Failed to listen on socket." << endl;
        exit(EXIT_FAILURE);
    }
}

int accept_connection(int sockfd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        cerr << "Failed to accept connection." << endl;
        exit(EXIT_FAILURE);
    }
    return client_fd;
}

struct FindByFD
{
    int fd;

    FindByFD(int fd) : fd(fd) { }

    bool operator()(const User& user) const
    {
        return user.fd() == fd;
    }
};
//remove the user from the users vector
void remove_user(vector<User>& users, int fd) {
    std::vector<User>::iterator it = std::find_if(users.begin(), users.end(), FindByFD(fd));
    if (it != users.end()) { //If no match is found, it returns users.end().
        close(it->fd());  
        users.erase(it);
    }
}


void run() {
    int sockfd = create_socket();
    bind_socket(sockfd);
    listen_socket(sockfd);

    vector<User> users;

    vector<pollfd> fds;
    pollfd tmp = {sockfd, POLLIN, 0};
    fds.push_back(tmp);

    while (true)
    {
        int num_fds = poll(fds.data(), fds.size(), -1);
        if (num_fds == -1) {
            cerr << "Failed to poll file descriptors." << endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == sockfd) {
                    // New client connection
                    int client_fd = accept_connection(sockfd);
                    pollfd tmp2 = {client_fd, POLLIN, 0};
                    fds.push_back(tmp2);
                    users.push_back(User(client_fd));
                    cout << "New client connected. FD: " << client_fd << endl;
                }
                else {
                    // Client message received
                    char buffer[4096];
                    int bytes_read = read(fds[i].fd, buffer, sizeof(buffer));
                    if (bytes_read <= 0) {
                        // Client disconnected
                        cout << "Client disconnected. FD: " << fds[i].fd << endl;
                        remove_user(users, fds[i].fd);
                        fds.erase(fds.begin() + i);// remove the specified fd
                        i--;
                    }
                    else {
                        // Process client message
                        std::vector<User>::iterator it = std::find_if(users.begin(), users.end(), FindByFD(fds[i].fd));
                        
                        if (it != users.end()) {
                            // Append message to user's buffer
                            memcpy(it->buffer() + it->buffer_len(), buffer, bytes_read);
                            it->set_buffer_len(it->buffer_len() + bytes_read);

                                                
                        // Process messages in user's buffer
                        char* message_start = it->buffer();
                        char* message_end = nullptr;
                        while ((message_end = strchr(message_start, '\n')) != nullptr) {
                            *message_end = '\0';
                            cout << "Received message from client " << fds[i].fd << ": " << message_start << endl;
                            message_start = message_end + 1;
                        }
                        it->set_buffer_len(strlen(message_start));
                         message_start = it->buffer();
                        }
                    }
                }
            }
        }
    }
}

int main() {
    run();
    return 0;
}