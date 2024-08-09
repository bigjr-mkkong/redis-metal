#ifndef __GEN_FIFO_H__
#define __GEN_FIFO_H__

typedef enum FIFO_Type_t{
    UNDEF,
    NAMED_PIPE,
    COHORT_QUEUE,
}FIFO_Type;

typedef struct FIFO_info{
    char named_pipe_path[128];
    int  mode;
    FIFO_Type ft;
} FIFO_info;

#define FIFO_READ_SIZE   2048
#define READ_NP_PATH     "/tmp/redis-pipe0"
#define WRITE_NP_PATH     "/tmp/redis-pipe1"


#ifndef FIFO_IMPL
void setFIFOEventLoop(struct redisServer *server, FIFO_info *fifo_info_read, FIFO_info *fifo_info_write);
int openNamedPipe(FIFO_info *fifo_info);
void readFromFIFO(struct aeEventLoop *el, int fd, void *private_data, int mask);
void write2FIFO(struct aeEventLoop *el, int fd, void *private_data, int mask);
#endif


#endif
