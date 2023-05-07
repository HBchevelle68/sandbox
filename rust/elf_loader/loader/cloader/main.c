#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <elf.h>

#define RESULT_ONE_THEN_DONE \
    do                       \
    {                        \
        result = 1;          \
        goto done;           \
    } while (0)

int main(int argc, char **argv)
{
    struct stat stat_buf = {0};
    uint8_t *file_data = NULL;
    int fd = -1;
    int result = 0;
    ssize_t bytes_read = 0;

    // Basic setup

    if (2 != argc)
    {
        printf("usage: cloader FILE\n");
        RESULT_ONE_THEN_DONE;
    }

    // Open targeted elf
    fd = open(argv[1], O_RDONLY);
    if (-1 == fd)
    {
        printf("[-] Error opening file. errno: %d\n", errno);
        RESULT_ONE_THEN_DONE;
    }
    // Pull file stats to get size
    result = fstat(fd, &stat_buf);
    if (-1 == result)
    {
        printf("[-] Error file stat. errno: %d\n", errno);
        RESULT_ONE_THEN_DONE;
    }
    file_data = calloc(stat_buf.st_size, sizeof(uint8_t));
    if (NULL == file_data)
    {
        printf("[-] Error allocating buffer. errno: %d\n", errno);
        RESULT_ONE_THEN_DONE;
    }
    while (bytes_read != stat_buf.st_size)
    {
        bytes_read = read(fd, (uint8_t *)file_data + bytes_read, stat_buf.st_size - bytes_read);
        if (bytes_read != stat_buf.st_size)
        {
            printf("[-] Error reading file. errno: %d\n", errno);
            RESULT_ONE_THEN_DONE;
        }
        printf("[*] Read: %zd bytes\n", bytes_read);
    }

    // End setup

done:
    // Clean up time
    if (NULL != file_data)
    {
        free(file_data);
    }
    if (0 < fd)
    {
        close(fd);
        fd = -1;
    }

    return result;
}