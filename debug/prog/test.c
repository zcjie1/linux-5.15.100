#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <linux/fs.h>

int main() {
    int fd;
    char buffer[100];
    ssize_t bytesRead;

    // Open /proc/mounts in read-only mode
    if ((fd = open("/proc/mounts", O_RDONLY)) == -1) {
        perror("Error opening file");
        return 1;
    }

    // Seek to the fifth byte
    if (lseek(fd, 4L, SEEK_SET) == -1) {
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

    sleep(5);

    mkdir("dir", 0755);
    syscall(__NR_mount, "testproc", "dir", "proc", MS_NODEV | MS_NOEXEC | MS_NOSUID, NULL);

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
