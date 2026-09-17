#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    char buffer[100];
    int fd = open("fd_vfs_input.txt", O_RDONLY);
    printf("open() returned FD: %d\n", fd);

    int n = read(fd, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    printf("read: %s", buffer);

    close(fd);
    return 0;
}
