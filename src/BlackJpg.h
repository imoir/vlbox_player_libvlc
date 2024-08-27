#ifndef VLBOX_PLAYER3_BLACK_JPG_H
#define VLBOX_PLAYER3_BLACK_JPG_H

#include <cstdint>
#include <sys/types.h>

class BlackJpg {
public:
    static int open(void *opaque, void **datap, uint64_t *sizep);
    static ssize_t read(void *opaque, unsigned char *buf, size_t len);
    static void close(void *opaque);
};

#endif
