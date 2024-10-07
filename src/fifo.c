#define FIFO_IMPL
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include "fifo.h"
#include "server.h"
#include "ae.h"

extern FIFO_info fifo_write;

int named_pipe_exist(char *pathname){
    return (access(pathname, R_OK | W_OK) == 0);
}

int legal_pipe(char *pathname){
    struct stat *statbuf = (struct stat*)zcalloc(sizeof(struct stat));
    int ret = stat(pathname, statbuf);
    if(ret == -1){
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if(S_ISFIFO(statbuf->st_mode)){
        fprintf(stderr, "Wrong FIFO type, re-create FIFO\n");
        return 0;
    }

    zfree(statbuf);
    return 1;

}

int createNamedPipe(FIFO_info *fifo_info){
    int fd, ret = 0;
    char *path = fifo_info->named_pipe_path;
    
    if(!named_pipe_exist(path)){
        ret = mkfifo(path, 0666);
        if(ret <= -1){
            perror("mkfio");
            exit(EXIT_FAILURE);
        }
        return 1;
    }

    if(!legal_pipe(path)){
        ret = unlink(path);
        if(ret){
            perror("remove");
            exit(EXIT_FAILURE);
        }

        ret = mkfifo(path, 0666);
        if(ret <= -1){
            perror("mkfio");
            exit(EXIT_FAILURE);
        }
    }


    return 1;
    
}

int openNamedPipe(FIFO_info *fifo_info){
    int fd;
    char *path = fifo_info->named_pipe_path;

    fd = open(path, fifo_info->mode);
    if(fd < 0){
        fprintf(stderr, "Failed to open Named Pipe: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}

int createCohortQueue(){
    printf("Cohort queue haven't impl. yet\n");
    return 0;
}

int createNewFIFO(FIFO_info *fifo_info){
    int ret;
    switch (fifo_info->ft){
        case(NAMED_PIPE):
            ret = createNamedPipe(fifo_info);
            break;

        case(COHORT_QUEUE):
            createCohortQueue(fifo_info);
            break;

        default:
            fprintf(stderr, "Failed to initialize new FIFO\n");
            ret = -1;
            break;
    }

    return ret;
}

void readFromFIFO(struct aeEventLoop *el, int fd, void *private_data, int mask){
    client *cl = (client*)private_data;
    if(!cl)
        return;

    char *buf = zcalloc(FIFO_READ_SIZE);
    ssize_t nread = read(fd, buf, sizeof(buf));

    if(nread == -1){
        zfree(buf);
        perror("read");
        return;
    }else if(nread == 0){
        zfree(buf);
        return;
    }

    cl->querybuf = sdscatlen(cl->querybuf, buf, nread);
    putClientInPendingWriteQueue(cl);
    processInputBuffer(cl);

    zfree(buf);
    return;
}

void setFIFOEventLoop(struct redisServer *server, FIFO_info *fifo_info_read, FIFO_info *fifo_info_write){
    int ret;

    createNamedPipe(fifo_info_read);
    createNamedPipe(fifo_info_write);
    
    int fd_read = openNamedPipe(fifo_info_read);
    if(fd_read < 0){
        fprintf(stderr, "Failed to create new read FIFO\n");
        exit(EXIT_FAILURE);
    }

    if(fifo_info_read->ft == NAMED_PIPE){
        client *cl = createClient(NULL);
        /*
         * TODO: 
         * We are going to use a new write function to write result to FIFO "connection"(which is a named pipe or Cohort queue)
         * cl->conn should not been considered as a real connection when isFIFO == 1
         * A more elegant way to deal with this is a new connection type(existed connections def&impl exists in connection.*)
         * However, this one still works, and working on new connection type may cost a lot of time
         * I'll do this in a new branch, for this branch lets just stick with this ugly design :(
         */
        cl->isFIFO = 1;
        cl->conn = (connection *)&fifo_write;
        if(aeCreateFileEvent(server->el, fd_read, AE_READABLE, readFromFIFO, cl) == AE_ERR){
            fprintf(stderr, "Failed to create event for read FIFO\n");
            close(fd_read);
            exit(1);
        }
    }else if(fifo_info_read->ft == COHORT_QUEUE){
        //register cohort queue as file(or other types if necessary) event in server->el
    }else{
        //should not be here
    }

    return;
}

