#include "../../module.h"

static const ModuleApi *game;
static unsigned int frames;

int moduleInit(const ModuleApi *api)
{
    if (api->version != SM_MODULE_VERSION)
        return -1;
    game = api;
    game->print("hello: module started, region %u\n", game->region);
    return 0;
}

void moduleUpdate(void)
{
    if (++frames % 300 == 0)
        game->print("hello: %u updates\n", frames);
}
