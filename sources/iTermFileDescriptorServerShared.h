//
//  iTermFileDescriptorServerShared.h
//  iTerm2
//
//  Created by George Nachman on 11/26/19.
//

#ifndef iTermFileDescriptorServerShared_h
#define iTermFileDescriptorServerShared_h

#include <stdio.h>
#include <sys/socket.h>

typedef union {
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(int))];
} iTermFileDescriptorControlMessage;

// Because xcode is hot garbage, syslog(LOG_DEBUG) goes to its console so we turn that off for debug builds.
#if DEBUG
#define FDLog(level, format, ...) do { \
    if (level < LOG_DEBUG) { \
        syslog(level, "iTermServer-Client(%d) " format, getpid(), ##__VA_ARGS__); \
    } \
} while (0)
#else
#if ITERM_SERVER
#define FDLog(level, format, ...) syslog(level, "iTermServer(pid=%d) " format, getpid(), ##__VA_ARGS__)
#else
#define FDLog(level, format, ...) syslog(level, "iTermServer-Client(%d) " format, getpid(), ##__VA_ARGS__)
#endif  // ITERM_SERVER
#endif  // DEBUG

void iTermFileDescriptorServerLog(char *format, ...);
int iTermFileDescriptorServerAcceptAndClose(int socketFd);
int iTermFileDescriptorServerAccept(int socketFd);
void SetRunningServer(void);

ssize_t iTermFileDescriptorServerSendMessageAndFileDescriptor(int connectionFd,
                                                              void *buffer,
                                                              size_t bufferSize,
                                                              int fdToSend);

// Socket only (e.g., unix-domain socket, not pipe)
ssize_t iTermFileDescriptorServerSendMessage(int connectionFd,
                                             void *buffer,
                                             size_t bufferSize,
                                             int *errorOut);

// For use on a pipe or other non-socket
ssize_t iTermFileDescriptorClientWrite(int fd, void *buffer, size_t bufferSize);

// Takes an array of file descriptors and its length as input. `results` should be an array of
// equal length. On return, the readable FDs will have the corresponding value in `results` set to
// true. Takes care of EINTR. Return value is number of readable FDs.
int iTermSelect(int *fds, int count, int *results, int wantErrors);

// Create a socket and listen on it. Returns the socket's file descriptor.
// This is used for connecting a client and server prior to fork.
// Follow it with a call to iTermFileDescriptorServerAcceptAndClose().
int iTermFileDescriptorServerSocketBindListen(const char *path);

#endif /* iTermFileDescriptorServerShared_h */