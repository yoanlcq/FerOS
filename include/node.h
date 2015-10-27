#ifndef NODE_H
#define NODE_H

typedef int node;

node nopen(const uint8_t *url, uint32_t flags);
int nclose(node n);
int nflush(node n);
int ngetc(node n, uint8_t *c);
int ngets(node n, uint8_t *res, uint32_t length);
int nprintf(node n, const uint8_t *fmt, ...);
int nscanf(node n, const uint8_t *fmt, ...);
int nputc(node n, uint8_t c);
int nputs(node n, const uint8_t *str);
int64_t nread(node n, uint8_t *str, int64_t nbytes);
int64_t nwrite(node n, uint8_t *str, int64_t nbytes);
int nseek(node n, int64_t offset, uint32_t whence);
/* err can be NULL. */
uint64_t ntell(node n, int *err);
int ndelete(const uint8_t *url);
int nctl(node n, uint64_t op, ...);

#endif /* NODE_H */
