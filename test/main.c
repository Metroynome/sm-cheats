#include "game.h"
#include "message.h"
#include "moby.h"

volatile struct {
    u32 magic, status;
    int level;
    u32 msgstrAddress, mobyCreateAddress, mobyDestroyAddress;
} smProbeResult = {0x534d5052, 0, -1, 0, 0, 0};

int main(void)
{
    smProbeResult.level = gameGetCurrentLevel();
    smProbeResult.msgstrAddress = GetAddress(&vaMsgStr);
    smProbeResult.mobyCreateAddress = GetAddress(&vaMobyCreate);
    smProbeResult.mobyDestroyAddress = GetAddress(&vaMobyDestroy);
    smProbeResult.status = smProbeResult.msgstrAddress ? 1 : 2;
    return 0;
}
