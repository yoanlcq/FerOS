#ifndef NODE_H
#define NODE_H

#include <stdint.h>

typedef void node;

typedef enum nopen_flags {
    N_READ     = 0x001,
    N_WRITE    = 0x002,
    N_RDWR     = 0x003,
    N_EXEC     = 0x004,
    N_APPEND   = 0x008,
    N_TRUNC    = 0x010,
    N_CREAT    = 0x020,
    N_EXCL     = 0x040,
    N_NOFOLLOW = 0x080,
    N_NONBLOCK = 0x100,
} nopen_flag;

typedef enum nseek_whences {
    NSEEK_SET  = 0,
    NSEEK_CUR  = 1,
    NSEEK_END  = 2,
    NSEEK_DATA = 3,
    NSEEK_HOLE = 4
} nseek_whence;

node *nopen(const uint8_t *url, uint32_t flags);
int nclose(node* n);
int nflush(node* n);
uint8_t ngetc(node* n, int *err);
uint64_t ngets(node* n, uint8_t *str, uint64_t nbytes, int *err);
int nprintf(node* n, const uint8_t *fmt, ...);
int nscanf(node* n, const uint8_t *fmt, ...);
int nputc(node* n, uint8_t c);
int nputs(node* n, const uint8_t *str);
uint64_t nread(node* n, uint8_t *str, uint64_t nbytes, int *err);
uint64_t nwrite(node* n, uint8_t *str, uint64_t nbytes, int *err);
int nseek(node* n, int64_t offset, uint32_t whence);
uint64_t ntell(node* n, int *err);
int ndelete(const uint8_t *url);
int nctl(node* n, uint64_t op, ...);

#endif /* NODE_H */
