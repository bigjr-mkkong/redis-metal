#ifndef __GEN_FIFO_H__
#define __GEN_FIFO_H__

typedef enum FIFO_Type_t{
    UNDEF,
    NAMED_PIPE,
    COHORT_QUEUE,
}FIFO_Type;

#define FIFO_READ_SIZE   2048
#define NAMED_PIPE_PATH     "/tmp/redis-pipe0"

#ifndef FIFO_IMPL
void setFIFOEventLoop(struct redisServer *server, FIFO_Type ft, char *fifo_path);
void readFromFIFO(struct aeEventLoop *el, int fd, void *private_data, int mask);
#endif

#endif
