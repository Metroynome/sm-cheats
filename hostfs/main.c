#include "stdio.h"
#include "file.h"

#define HOSTFS_STATE (*(volatile unsigned int *)0x000ff000)
#define HOSTFS_BUFFER ((char *)0x000ff040)
#define HOSTFS_MAGIC 0x48534653

int main(void)
{
    const char *path = "host0:sm/test.txt";
    int fd, count, result;

    if (HOSTFS_STATE == HOSTFS_MAGIC)
        return 0;
    HOSTFS_STATE = HOSTFS_MAGIC;

    fd = fileOpen(path, 1, 0);
    printf("hostfs: open %s = %d\n", path, fd);
    if (fd < 0) {
        path = "host0:sm/test";
        fd = fileOpen(path, 1, 0);
        printf("hostfs: open %s = %d\n", path, fd);
    }
    if (fd < 0)
        return 0;

    count = fileRead(fd, HOSTFS_BUFFER, 1023);
    if (count >= 0 && count <= 1023) {
        HOSTFS_BUFFER[count] = 0;
        printf("hostfs: read %d bytes\n%s\n", count, HOSTFS_BUFFER);
        if (count == 1023)
            printf("hostfs: output limited to 1023 bytes\n");
    } else {
        printf("hostfs: read failed (%d)\n", count);
    }
    result = fileClose(fd);
    printf("hostfs: close = %d\n", result);
    return 0;
}
