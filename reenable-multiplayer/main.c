#include <librac5/stdio.h>
#include <librac5/utils.h>
#include <librac5/types.h>


#define uiCreatePRofileOption_Default ((u32*)0x01090da8)
#define uiDeletePRofileOption_Default ((u32*)0x01090dac)
#define uiSelectPRofileOption_Default ((u32*)0x01090db0)

bool init = false;

void patchSelectPRofile(void)
{
 // stopped here because I found out there's no network functions.
    return;
}

void start(void)
{
    // hook to end of uiCreateProfileOption_Update
    HOOK_J(0x01064174, &patchSelectPRofile);

    // set default multiplayer menu to select profile
    u8 *pushUI_a0 = 0x00f898b8;
    if (*pushUI_a0 == 0x33)
        *pushUI_a0 = 0x27;

    init = true;
}

int main(void)
{

    if (!init)
        start();

    return 0;
}
