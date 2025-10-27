#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        return 1; // ERR_USAGE
    }

    const char* source_path = argv[1];
    const char* dest_path = argv[2];

    if (strcmp(source_path, dest_path) == 0) {
        fprintf(stderr, "Error: Source and destination are the same file\n");
        return 1; // ERR_USAGE
    }

    struct stat source_stat;
    if (stat(source_path, &source_stat) == -1) {
        fprintf(stderr, "Error: Cannot stat source file - %s\n", strerror(errno));
        return 9; // ERR_STAT_SOURCE
    }

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        fprintf(stderr, "Error: Cannot open source file - %s\n", strerror(errno));
        return 2; // ERR_OPEN_SOURCE
    }

    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        close(source_fd);
        fprintf(stderr, "Error: Cannot open destination file - %s\n", strerror(errno));
        return 3; // ERR_OPEN_DEST
    }

    // Copy data using read/write
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            close(source_fd);
            close(dest_fd);
            unlink(dest_path);
            fprintf(stderr, "Error: Failed to write to destination file - %s\n", strerror(errno));
            return 5; // ERR_WRITE_DEST
        }
    }

    if (bytes_read == -1) {
        close(source_fd);
        close(dest_fd);
        unlink(dest_path);
        fprintf(stderr, "Error: Failed to read source file - %s\n", strerror(errno));
        return 4; // ERR_READ_SOURCE
    }

    if (close(source_fd) == -1) {
        close(dest_fd);
        unlink(dest_path);
        fprintf(stderr, "Error: Failed to close source file - %s\n", strerror(errno));
        return 6; // ERR_CLOSE_FILES
    }

    if (close(dest_fd) == -1) {
        unlink(dest_path);
        fprintf(stderr, "Error: Failed to close destination file - %s\n", strerror(errno));
        return 6; // ERR_CLOSE_FILES
    }

    if (unlink(source_path) == -1) {
        fprintf(stderr, "Error: Failed to remove source file - %s\n", strerror(errno));
        return 7; // ERR_REMOVE_SOURCE
    }
    
    return 0;
}

