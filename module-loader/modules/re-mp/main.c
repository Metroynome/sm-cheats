#include "../../module.h"
#include <librac5/stdio.h>
#include <librac5/utils.h>
#include <librac5/types.h>
#include <librac5/ui.h>

#define uiCreatePRofileOption_Default ((u32*)0x01090da8)
#define uiDeletePRofileOption_Default ((u32*)0x01090dac)
#define uiSelectPRofileOption_Default ((u32*)0x01090db0)

static const ModuleApi *game;
bool init = false;

void patchSelectPRofile(void)
{
    if (!*uiCreatePRofileOption_Default || !*uiSelectPRofileOption_Default) {
        *uiSelectPRofileOption_Default = 1;
        *uiCreatePRofileOption_Default = 1;
    }
}

void start(void)
{
    // hook to end of uiCreateProfileOption_Update
    HOOK_J(0x01064174, &patchSelectPRofile);

    // set default multiplayer menu to select profile
    u8 *pushUI_a0 = (u8 *)0x00f898b8;
    if (*pushUI_a0 == 0x33)
        *pushUI_a0 = 0x27;

    init = true;
}

void moduleUpdate(void)
{
    if (!init)
        start();
}

int moduleInit(const ModuleApi *api)
{
    if (api->version != SM_MODULE_VERSION)
        return -1;
    game = api;
    MDPRINTF(game, "re-mp: module started, region %u\n", game->region);
    return 0;
}
