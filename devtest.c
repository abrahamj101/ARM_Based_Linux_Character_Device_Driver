#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    unsigned int result, read_i, read_j; // Variables to store results and inputs
    int fd; // File descriptor for the device
    int i, j; // Loop counters for multiplication tests

    // Buffer for I/O operations
    unsigned int* values = (unsigned int*)malloc(2 * sizeof(unsigned int));
    unsigned int* results = (unsigned int*)malloc(3 * sizeof(unsigned int));

    // Open the multiplier device for read/write operations
    fd = open("/dev/multiplier", O_RDWR);
    if (fd == -1) {
        printf("Failed to open device file!\n");
        free(values);
        free(results);
        return -1;
    }

    // User input control variable
    char command = 0;

    // Run multiplication tests until 'q' is entered
    while (command != 'q') {
        for (i = 0; i <= 16; i++) {
            for (j = 0; j <= 16; j++) {
                values[0] = i;
                values[1] = j;

                // Write multiplication inputs to the device
                write(fd, values, 2 * sizeof(unsigned int));

                // Read the result of the multiplication from the device
                read(fd, results, 3 * sizeof(unsigned int));
                read_i = results[0];
                read_j = results[1];
                result = results[2];

                // Display the result
                printf("%u * %u = %u\n", read_i, read_j, result);
                if (result == (i * j)) {
                    printf("Result Correct!\n");
                } else {
                    printf("Result Incorrect!\n");
                }

                // Check for user input to quit
                command = getchar();
                if (command == 'q') {
                    close(fd);
                    free(values);
                    free(results);
                    return 0;
                }
            }
        }
    }

    // Cleanup and close operations
    close(fd);
    free(values);
    free(results);
    return 0;
}