#include "stdio.h"

#ifdef RAC5_NTSCJ
#define GAME_VSYNC_COUNT 0x01ef74d4
#else
#define GAME_VSYNC_COUNT 0x01ef57b4
#endif

int main(void)
{
    printf("Size Matters printf test: vblank=%u\n",
           *(volatile unsigned int *)GAME_VSYNC_COUNT);
    return 0;
}
