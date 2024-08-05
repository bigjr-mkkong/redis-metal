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


int createNamedPipe(char *path){
    int fd;

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

int createNewFIFO(FIFO_Type ft, char *path){
    int ret;
    switch (ft){
        case(NAMED_PIPE):
            ret = createNamedPipe(path);
            break;

        case(COHORT_QUEUE):
            createCohortQueue();
            break;

        default:
            fprintf(stderr, "Failed to initialize new FIFO\n");
            ret = -1;
            break;
    }

    return ret;
}

void readFromFIFO(struct aeEventLoop *el, int fd, void *private_data, int mask){
    fprintf(stdout, "Read something");
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

void setFIFOEventLoop(struct redisServer *server, FIFO_Type ft, char *fifo_path){
    int fd = createNewFIFO(ft, fifo_path);
    if(fd < 0){
        fprintf(stderr, "Failed to create new FIFO\n");
        return;
    }

    if(ft == NAMED_PIPE){
        client *cl = createClient(NULL);
        if(aeCreateFileEvent(server->el, fd, AE_READABLE, readFromFIFO, cl) == AE_ERR){
            fprintf(stderr, "Failed to create event for FIFO\n");
            close(fd);
            exit(1);
        }
    }else if(ft == COHORT_QUEUE){
        //register cohort queue as file(or other types if necessary) event in server->el
    }else{
        //should not be here
    }

    return;
}

