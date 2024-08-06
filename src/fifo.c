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


char fifo_send_path[] = "./fifo_send";
char fifo_recv_path[] = "./fifo_recv";

int named_pipe_exist(char *pathname){
    return F_OK == access(pathname, R_OK | W_OK | (!X_OK));
}

int legal_pipe(char *pathname){
    struct stat *statbuf = (struct stat*)zcalloc(sizeof(struct stat));
    int ret = stat(pathname, statbuf);
    
    if(statbuf->st_mode != S_IFIFO){
        fprintf(stderr, "Wrong FIFO type, re-create FIFO\n");
        return 0;
    }

    zfree(statbuf);
    return 1;

}

int createNamedPipe(FIFO_info *fifo_info){
    int fd;
    char *path = fifo_info->named_pipe_path;

    if(!named_pipe_exist(path) || !legal_pipe(path))
        mkfifo(path, 0x1b6);

    fd = open(path, O_RDONLY | O_NONBLOCK, 0x1b6);
    if(fd < 0){
        fprintf(stderr, "Failed to create Named Pipe: %s\n", strerror(errno));
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

    processInputBuffer(cl);

    zfree(buf);
    return;
}

void setFIFOEventLoop(struct redisServer *server, FIFO_info *fifo_info){
    int fd = createNewFIFO(fifo_info);
    if(fd < 0){
        fprintf(stderr, "Failed to create new FIFO\n");
        return;
    }

    if(fifo_info->ft == NAMED_PIPE){
        client *cl = createClient(NULL);
        if(aeCreateFileEvent(server->el, fd, AE_READABLE, readFromFIFO, cl) == AE_ERR){
            fprintf(stderr, "Failed to create event for FIFO\n");
            close(fd);
            exit(1);
        }
    }else if(fifo_info->ft == COHORT_QUEUE){
        //register cohort queue as file(or other types if necessary) event in server->el
    }else{
        //should not be here
    }

    return;
}

/* void clearNamedPipe(); */

/* void clearCohortQueue(); */

/* void unsetFIFOEventLoop(struct redisServer *server, FIFO_Type ft){ */
/*     switch (ft){ */
/*         case NAMED_PIPE: */
/*             clearNamedPipe(); */
/*             break; */
/*         case COHORT_QUEUE: */
/*             clearCohortQueue(); */
/*             break; */

/*         default: */
/*             fprintf(stderr, "Failed to unset FIFO Event loop: Should not be here :<\n"); */
/*     } */

/*     return; */
/* } */
