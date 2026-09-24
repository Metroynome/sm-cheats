#ifndef SM_MODULE_H
#define SM_MODULE_H

#define SM_MODULE_MAGIC 0x444f4d53
#define SM_MODULE_VERSION 1
#define SM_MODULE_NO_ENTRY 0xffffffff
#define SM_REGION_NTSCU 1
#define SM_REGION_PAL 2
#define SM_REGION_NTSCJ 3

#if defined(RAC5_PAL)
#define SM_MODULE_REGION SM_REGION_PAL
#elif defined(RAC5_NTSCJ)
#define SM_MODULE_REGION SM_REGION_NTSCJ
#else
#define SM_MODULE_REGION SM_REGION_NTSCU
#endif

typedef struct ModuleHeader {
    unsigned int magic, version, region, address;
    unsigned int imageSize, memorySize, initOffset, updateOffset;
} ModuleHeader;

typedef struct ModuleApi {
    unsigned int version, region;
    int (*print)(const char *format, ...);
    int (*open)(const char *path, int flags, unsigned short mode);
    int (*read)(int fd, void *buffer, int size);
    int (*close)(int fd);
} ModuleApi;

#ifdef DEBUG
#define MDPRINTF(api, ...) do { (api)->print(__VA_ARGS__); } while (0)
#else
#define MDPRINTF(api, ...) do { } while (0)
#endif

typedef int (*ModuleInit)(const ModuleApi *api);
typedef void (*ModuleUpdate)(void);

#endif
