#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SYSFS_PATH_LEN (128)
#define ID_STR_LEN (32)
#define UIO_DEVICE_PATH_LEN (32)
#define NUM_UIO_DEVICES (32)

const char uio_id_str[] = "apb_test";
const char sysfs_fmt[] = "/sys/class/uio/uio%d/%s";

int get_uio_device(const char *id)
{
    FILE *fp;
    int i;
    int len;
    char file_id[ID_STR_LEN];
    char sysfs_path[SYSFS_PATH_LEN];

    for (i = 0; i < NUM_UIO_DEVICES; i++)
    {
        snprintf(sysfs_path, SYSFS_PATH_LEN, sysfs_fmt, i, "/name");
        fp = fopen(sysfs_path, "r");
        if (fp == NULL)
            break;

        fscanf(fp, "%32s", file_id);

        len = strlen(id);
        if (len > ID_STR_LEN - 1)
        {
            len = ID_STR_LEN - 1;
        }

        if (strncmp(file_id, id, len) == 0)
        {
            return i;
        }
    }

    return -1;
}

uint32_t get_memory_size(char *sysfs_path, char *uio_device)
{
    FILE *fp;
    uint32_t sz;

    fp = fopen(sysfs_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Unable to determine size for %s\n", uio_device);
        exit(0);
    }

    fscanf(fp, "0x%016X", &sz);

    fclose(fp);

    return sz;
}

int main(int argc, char *argv[])
{
    int fd;
    int index = 0;
    char uio_device[UIO_DEVICE_PATH_LEN];
    char sysfs_path[SYSFS_PATH_LEN];
    volatile uint32_t *mem_ptr;
    uint32_t map_size;

    printf("Searching device for %s\n", uio_id_str);

    index = get_uio_device(uio_id_str);
    if (index < 0)
    {
        fprintf(stderr, "Unable locate device for %s\n", uio_id_str);
        return -1;
    }

    snprintf(uio_device, UIO_DEVICE_PATH_LEN, "/dev/uio%d", index);
    printf("Found %s\n", uio_device);

    fd = open(uio_device, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Cannot open %s: %s\n", uio_device, strerror(errno));
        return -1;
    }

    printf("Opened %s (r,w)\n", uio_device);

    snprintf(sysfs_path, SYSFS_PATH_LEN, sysfs_fmt, index, "maps/map0/size");
    map_size = get_memory_size(sysfs_path, uio_device);
    if (map_size == 0)
    {
        fprintf(stderr, "Invalid memory size for %s\n", uio_device);
        return -1;
    }

    mem_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem_ptr == MAP_FAILED)
    {
        fprintf(stderr, "Cannot mmap: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    printf("Size of memory is 0x%x\n", map_size);

    mem_ptr[0x10u / 4u] = 0x00000000u;

    printf("CONTROL_0: 0x%x\n", mem_ptr[0x00u / 4u]);
    printf("CONTROL_1: 0x%x\n", mem_ptr[0x10u / 4u]);
    printf("STATUS: 0x%x\n", mem_ptr[0x20u / 4u]);

    sleep(2);

    mem_ptr[0x10u / 4u] = 0x0F0F0F0Fu;

    printf("CONTROL_0: 0x%x\n", mem_ptr[0x00u / 4u]);
    printf("CONTROL_1: 0x%x\n", mem_ptr[0x10u / 4u]);
    printf("STATUS: 0x%x\n", mem_ptr[0x20u / 4u]);

    sleep(2);

    mem_ptr[0x10u / 4u] = 0x0F0F0F70u;

    printf("CONTROL_0: 0x%x\n", mem_ptr[0x00u / 4u]);
    printf("CONTROL_1: 0x%x\n", mem_ptr[0x10u / 4u]);
    printf("STATUS: 0x%x\n", mem_ptr[0x20u / 4u]);

    sleep(2);

    mem_ptr[0x10u / 4u] = 0x00000000u;

    printf("CONTROL_0: 0x%x\n", mem_ptr[0x00u / 4u]);
    printf("CONTROL_1: 0x%x\n", mem_ptr[0x10u / 4u]);
    printf("STATUS: 0x%x\n", mem_ptr[0x20u / 4u]);

    munmap((void *)mem_ptr, map_size);

    close(fd);

    printf("Closed %s\n", uio_device);

    return 0;
}
