# IRC
_____________________________________________________________________________________________

## Tp do the Server you have to know the following steps:

 ### non-blocking file descriptors:
     non-blocking file descriptors help your computer do multiple tasks without getting stuck waiting for each task to be done.

### How to use non-blocking file descriptors :
1) Open a File Descriptor with socket().
2) Set Non-blocking Mode with fcntl().
3) Use Non-blocking Operations with read(), write(), recv(), send().

### poll():
    It helps your computer wait for things to happen without using too much energy checking all the time.
    when you're using recv() or send() to read or write data with non-blocking file descriptor spoll() to wait until there's something to read or write.

### the equivalent of the poll() function:
    The select() function is often used in C/C++98 to achieve similar I/O multiplexing capabilities as poll().
    It helps you to listens for activity on different (fd) and tells you when there's something happening.

### TCP/IP:
    (Transmission Control Protocol/Internet Protocol) is a set of communication protocols that computers use to talk to each other over a network.
    TCP and IP work like a team to make sure your data gets from one computer to another in an accurate way.
