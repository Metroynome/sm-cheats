#include "stdio.h"
#include "string.h"
#include "file.h"
#include "kernel.h"
#include "module.h"
#include "exception.h"

#define MODULE_START 0x000f4000U
#define MODULE_END 0x000fe000U
#define MODULE_LIMIT 8
#define LOADER_MAGIC 0x534d4c32U

struct LoadedModule {
    unsigned int address, size;
    ModuleUpdate update;
};

struct LoaderState {
    unsigned int magic, busy, count;
    struct LoadedModule modules[MODULE_LIMIT];
    char list[2048];
};

#define STATE ((struct LoaderState *)0x000fe000)

static const ModuleApi api = {
    SM_MODULE_VERSION, SM_MODULE_REGION, printf, fileOpen, fileRead, fileClose
};

static int readExact(int fd, void *buffer, unsigned int size)
{
    unsigned int done = 0;
    while (done < size) {
        int count = fileRead(fd, (char *)buffer + done, size - done);
        if (count <= 0 || (unsigned int)count > size - done)
            return 0;
        done += count;
    }
    return 1;
}

static int atEnd(int fd)
{
    char extra;
    return fileRead(fd, &extra, 1) == 0;
}

static void syncCode(void)
{
    FlushCache(0);
    FlushCache(2);
}

static void installExceptionHandler(void)
{
    unsigned char *memory = (unsigned char *)EXCEPTION_ADDRESS;
    unsigned int i, hash = 2166136261U;
    int fd, valid;

    fd = fileOpen("host0:sm/debug/exceptionhandler.bin", 1, 0);
    if (fd < 0) {
        printf("loader: optional exception handler unavailable (%d)\n", fd);
        return;
    }
    for (i = 0; i < EXCEPTION_MEMORY_SIZE; ++i) {
        if (memory[i]) {
            printf("loader: exception handler area is occupied\n");
            fileClose(fd);
            return;
        }
    }
    valid = readExact(fd, memory, EXCEPTION_FILE_SIZE) && atEnd(fd);
    if (fileClose(fd) < 0)
        valid = 0;
    for (i = 0; valid && i < EXCEPTION_FILE_SIZE; ++i)
        hash = (hash ^ memory[i]) * 16777619U;
    if (!valid || hash != EXCEPTION_HASH) {
        memset(memory, 0, EXCEPTION_MEMORY_SIZE);
        printf("loader: rejected exception handler (size/read/hash)\n");
        return;
    }
    syncCode();
    printf("loader: installing exception handler at %08x\n", EXCEPTION_ADDRESS);
    ((void (*)(void))EXCEPTION_ADDRESS)();
    syncCode();
    printf("loader: exception handler installed\n");
}

static int validPath(const char *path)
{
    const char *start = path;
    if (!*path)
        return 0;
    for (;;) {
        if (*path == ':' || *path == '\\')
            return 0;
        if (!*path || *path == '/') {
            int length = path - start;
            if (!length || (length == 1 && start[0] == '.') ||
                (length == 2 && start[0] == '.' && start[1] == '.'))
                return 0;
            if (!*path)
                return 1;
            start = path + 1;
        }
        ++path;
    }
}

static int validEntry(unsigned int offset, unsigned int size)
{
    return offset == SM_MODULE_NO_ENTRY || (!(offset & 3) && offset < size);
}

static int validHeader(const ModuleHeader *header)
{
    unsigned int i;
    if (header->magic != SM_MODULE_MAGIC || header->version != SM_MODULE_VERSION ||
        (header->region && header->region != SM_MODULE_REGION) ||
        (header->address & 15) || header->address < MODULE_START ||
        header->address >= MODULE_END || !header->imageSize ||
        (header->imageSize & 3) || header->imageSize > header->memorySize ||
        header->memorySize > MODULE_END - header->address ||
        !validEntry(header->initOffset, header->imageSize) ||
        !validEntry(header->updateOffset, header->imageSize))
        return 0;
    for (i = 0; i < STATE->count; ++i) {
        struct LoadedModule *module = &STATE->modules[i];
        if (header->address < module->address + module->size &&
            module->address < header->address + header->memorySize)
            return 0;
    }
    return 1;
}

static void loadModule(const char *name)
{
    char path[128];
    ModuleHeader header;
    struct LoadedModule *module;
    int fd, valid, result = 0;

    if (STATE->count >= MODULE_LIMIT || !validPath(name) || strlen(name) > 118) {
        printf("loader: rejected module path/count: %s\n", name);
        return;
    }
    snprintf(path, sizeof(path), "host0:sm/%s", name);
    fd = fileOpen(path, 1, 0);
    if (fd < 0) {
        printf("loader: open %s failed (%d)\n", path, fd);
        return;
    }
    if (!readExact(fd, &header, sizeof(header)) || !validHeader(&header)) {
        printf("loader: invalid header, region, range or overlap: %s\n", name);
        fileClose(fd);
        return;
    }
    memset((void *)header.address, 0, header.memorySize);
    valid = readExact(fd, (void *)header.address, header.imageSize) && atEnd(fd);
    if (fileClose(fd) < 0)
        valid = 0;
    if (!valid) {
        memset((void *)header.address, 0, header.memorySize);
        printf("loader: incomplete or oversized module: %s\n", name);
        return;
    }
    module = &STATE->modules[STATE->count++];
    module->address = header.address;
    module->size = header.memorySize;
    module->update = 0;
    syncCode();
    if (header.initOffset != SM_MODULE_NO_ENTRY)
        result = ((ModuleInit)(header.address + header.initOffset))(&api);
    if (result < 0) {
        printf("loader: init failed (%d), keeping memory reserved: %s\n", result, name);
        return;
    }
    if (header.updateOffset != SM_MODULE_NO_ENTRY)
        module->update = (ModuleUpdate)(header.address + header.updateOffset);
    syncCode();
    printf("loader: loaded %s at %08x (%u bytes)\n", name, header.address, header.memorySize);
}

static void loadModuleList(void)
{
    char *line, *next, *end;
    unsigned int size = 0;
    int fd = fileOpen("host0:sm/modules.txt", 1, 0);
    int count = 0, valid = 1;
    if (fd < 0) {
        printf("loader: modules.txt unavailable (%d)\n", fd);
        return;
    }
    while (size < sizeof(STATE->list) - 1) {
        count = fileRead(fd, STATE->list + size, sizeof(STATE->list) - 1 - size);
        if (count < 0 || (unsigned int)count > sizeof(STATE->list) - 1 - size) {
            valid = 0;
            break;
        }
        if (!count)
            break;
        size += count;
    }
    if (size == sizeof(STATE->list) - 1 && !atEnd(fd))
        valid = 0;
    if (fileClose(fd) < 0)
        valid = 0;
    if (!valid || memchr(STATE->list, 0, size)) {
        printf("loader: modules.txt read failed or exceeds 2047 bytes\n");
        return;
    }
    STATE->list[size] = 0;
    line = STATE->list;
    while (*line) {
        next = strchr(line, '\n');
        if (next)
            *next++ = 0;
        while (*line == ' ' || *line == '\t' || *line == '\r')
            ++line;
        end = line + strlen(line);
        while (end > line && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r'))
            *--end = 0;
        if (*line && *line != '#')
            loadModule(line);
        if (!next)
            break;
        line = next;
    }
}

int main(void)
{
    unsigned int i;
    if (STATE->magic != LOADER_MAGIC) {
        memset(STATE, 0, sizeof(*STATE));
        STATE->magic = LOADER_MAGIC;
        STATE->busy = 1;
        printf("loader: starting (region %u)\n", SM_MODULE_REGION);
        installExceptionHandler();
        loadModuleList();
        STATE->busy = 0;
    }
    if (STATE->busy)
        return 0;
    STATE->busy = 1;
    for (i = 0; i < STATE->count; ++i) {
        if (STATE->modules[i].update)
            STATE->modules[i].update();
    }
    STATE->busy = 0;
    return 0;
}
