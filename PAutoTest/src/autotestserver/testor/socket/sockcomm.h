#ifndef _SOCKCOMM_H_
#define _SOCKCOMM_H_

#include <sys/socket.h>
#include <sys/types.h>

#define SOCK_PORT 7000
#define BUF_LEN  4096

#define CHECK(expr) do { \
    err = expr; \
    if (err < 0) { \
        printf("Error:%d line:%d\n", err, __LINE__); \
        return err; \
    }} while (0)


ssize_t readn(int fd, void *vptr, size_t n) {
    ssize_t nread;
    char *ptr = (char*)vptr;
    size_t nleft = n;
    while(nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR)
                nread = 0;
            else
                return -1; 
        }
        else if(nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }   
    return (n-nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n) {
    ssize_t nwritten;
    char* ptr = (char*)vptr;
    size_t nleft = n;
    while(nleft > 0) {
        if((nwritten = write(fd, ptr, nleft)) <= 0) {
            if(nwritten <0 && errno == EINTR)
                nwritten = 0;
            else
                return -1; 
        }
        nleft -= nwritten;
        ptr += nwritten;
    }   
    return n;
}

ssize_t readp(int fd, char *ptr, size_t n) {
    uint32_t len = 0;
    ssize_t nr = readn(fd, &len, sizeof(len));
    if (nr <= 0) return nr; 
    
    return readn(fd, ptr, len);
}

ssize_t writep(int fd, const char *ptr, size_t n) {
    uint32_t len = n;
    ssize_t nw = writen(fd, &len, sizeof(len));
    if (nw <= 0) return nw; 
    return writen(fd, ptr, n);  
}

#endif // _SOCKCOMM_H_
