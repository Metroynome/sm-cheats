#include "stdio.h"
#include "stdlib.h"
#include "string.h"
/* 0=not run, 1=passed, 3=allocation failure, 4=bad output.
 * Invoke once from a valid game thread, outside loading/shutdown transitions. */
volatile unsigned int libcDemoStatus __attribute__((section(".data.demo"))) = 0;
int main(void)
{
    char *buffer;
    if (libcDemoStatus) return 0;
    buffer=malloc(128);
    if (!buffer) { libcDemoStatus=3; return 0; }
    memset(buffer,0,128);
    strcpy(buffer,"librac5");
    if (strcmp(buffer,"librac5") || strlen(buffer)!=7) { free(buffer); libcDemoStatus=4; return 0; }
    snprintf(buffer,128,"librac5 libc: %s", "ready");
    printf("%s\n",buffer);
    free(buffer);
    libcDemoStatus=1;
    return 0;
}
