#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char b[1];
    int fd = open("/dev/ttyUSB0", O_RDWR);
    int ff = open("data.txt", O_RDWR | O_CREAT);
    ssize_t size = read(fd, &b, 1);
    printf("Read byte %c\n", b);
    write(ff, b, 1);
    close(fd);
    close(ff);
    return 0;
}