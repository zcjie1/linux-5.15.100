#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    int fd;
    char buffer[1024];
    ssize_t bytesRead;

    // Open /proc/mounts in read-only mode
    if ((fd = open("/proc/mounts", O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }

    // Seek to the fifth byte
    if (lseek(fd, 25L, SEEK_SET) == -1) {
        perror("Error seeking in file");
        close(fd);
        return 1;
    }

    // Read from the file starting from the fifth byte
    if ((bytesRead = read(fd, buffer, sizeof(buffer))) == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    // Null terminate the string so we can print it
    buffer[bytesRead] = '\0';

    // Print the data
    printf("Data starting from the fifth byte:\n%s\n", buffer);

    // Close the file
    if (close(fd) == -1) {
        perror("Error closing file");
        return 1;
    }

    return 0;
}
