#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include "fifo.h"


char fifo_send_path[] = "./fifo_send";
char fifo_recv_path[] = "./fifo_recv";


int createNamedPipe(char *path){
    int fd;

    mkfifo(path);
    fd = open(path, O_RDONLY, 0x1b6);
    if(fd < 0){
        fprintf("Failed to create Named Pipe: %s\n", strerror(errno));
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

        case(COHORT):
            createCohortQueue();
            break;

        default:
            fprintf(stderr, "Failed to initialize new FIFO\n");
            return = -1;
            break;
    }

    return ret;
}

void setFIFOEventLoop(redisServer *server, FIFO_Type ft, const char *fifo_path){
    int fd = createNewFIFO(ft, fifo_path);
    if(fd < 0){
        fprintf(stderr, "Failed to create new FIFO\n");
        return -1;
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

void readFromFifo(aeEventLoop el, int fd, void *privdat, int mask){
    client *cl = (client*)privdat;
    if(!cl){
        zfree(buf);
        return;
    }

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


    cl->querybuf = sdscatlen(c->querybuf, buf, nread);

    processInputBuffer(c);

    zfree(buf);
    return;
}
