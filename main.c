// 2024-1 Operating Systems (ITP30002) - HW #1
// File  : main.c
// Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

// MACROS
#define BUFFER_SIZE 4096

// HEADERS
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// STRUCTURES
/**
 * Linked list node for source files
 */
struct source_node {
    char *source;
    struct source_node *next;
};

// TYPEDEFS
/**
 * Task information
 */
typedef struct {
    int operation;
    int verbose;
} tasks;

/**
 * Directories information
 */
typedef struct {
    struct source_node *head;
    char *destination;
} directories;

// GLOBAL VARIABLES
tasks task = {0, 0};             // Task information
directories dir = {NULL, NULL};  // Directories information

// FUNCTION PROTOTYPES
void argparse(int argc, char *argv[]);          // Parse command line arguments
int validate();                                 // Validate source and destination
int copy_f2f(char *origin, char *destination);  // Copy a file to another file
int copy_f2d(char *origin, char *destination);  // Copy multiple sources to a target directory
int copy_d2d(char *origin, char *destination);  // Copy all files and directories from source directory to target directory
void verbose(char *origin, char *destination);  // Display verbose message if verbose mode is enabled
void help(char *error);                         // Display help message
int parserTest(int argc, char *argv[]);         // Test parser

// MAIN FUNCTION
int main(int argc, char *argv[]) {
    argparse(argc, argv);
#ifdef DEBUG
    assert(parserTest(argc, argv) == 0);  // To test parser when DEBUG is enabled
#endif
    if (validate()) {
        help("Invalid source or destination");
    }

    switch (task.operation) {  // Select the operation
        case 1:                // Operation 1: Copy source file to target file
            if (copy_f2f(dir.head->source, dir.destination)) {
                perror("[copy_f2f] ");
                exit(1);
            }
            break;
        case 2:  // Operation 2: Copy multiple source file to target directory
            if (copy_f2d(dir.head->source, dir.destination)) {
                perror("[copy_f2d] ");
                exit(1);
            }
            break;
        case 3:  // Operation 3: Copy all files and directories from source directory to target directory
            if (copy_d2d(dir.head->source, dir.destination)) {
                perror("[copy_d2d] ");
                exit(1);
            }
            break;
        default:
            help("Invalid operation");
            break;
    }

    return 0;
}

// FUNCTION DEFINITIONS
/**
 * Parse command line arguments
 */
void argparse(int argc, char *argv[]) {
    if (argc < 2) {                      // Minimal argc is 2 (./copyfile -h)
        help("Insufficient arguments");  // Display error message
    }

    if (strlen(argv[1]) > 2) {       // Check the length of the option is longer than 2 to avoid segmentation fault
        if (argv[1][2] == 'v') {     // Check if the last character of the option is 'v'
            task.verbose = 1;        // Enable verbose mode
        } else {                     // The length of the option is longer than 2 but the last character is not 'v'
            help("Invalid option");  // Display error message
        }
    }

    // Initialize source and destination by using linked list
    dir.head = (struct source_node *)malloc(sizeof(struct source_node));  // Allocate memory for the head node
    dir.head->source = argv[2];                                           // Set source as the argument
    dir.head->next = NULL;                                                // Initialize next node as NULL. It will be used for multiple sources especially for -m option
    dir.destination = argv[argc - 1];                                     // Set destination as the last argument

    if (strcmp(argv[1], "-h") == 0) {
        help(NULL);  // Help message without any error message
        exit(0);     // Exit after displaying help message without any error
    } else if (strncmp(argv[1], "-f", 2) == 0) {
        if (argc < 4) {                                                    // Minimal argc is 4 (./copyfile -f <Source File> <Target File>)
            help(strcat("Insufficient arguments for option: ", argv[1]));  // Display error message
        } else if (argc > 4) {                                             // Maximal argc is 4 (./copyfile -f <Source File> <Target File>)
            help(strcat("Too many arguments for option: ", argv[1]));      // Display error message
        }

        task.operation = 1;  // Set operation as 1
    } else if (strncmp(argv[1], "-m", 2) == 0) {
        if (argc < 5) {                                                    // Minimal argc is 5 (./copyfile -m <Source file 1> <Source file 2> ... <Source file N> <Target directory>)
            help(strcat("Insufficient arguments for option: ", argv[1]));  // Display error message
        }

        task.operation = 2;  // Set operation as 2

        // Add multiple sources to the linked list
        struct source_node *temp = dir.head;
        for (int i = 3; i < argc - 1; i++) {                                        // Add argument from ARG_3 to ARG_N-1 to the linked list
            temp->next = (struct source_node *)malloc(sizeof(struct source_node));  // Allocate memory for the next node
            temp->next->source = argv[i];                                           // Set source as the argument
            temp->next->next = NULL;                                                // Initialize next node as NULL
            temp = temp->next;                                                      // Move to the next node
        }
    } else if (strncmp(argv[1], "-d", 2) == 0) {
        if (argc < 4) {                                                    // Minimal argc is 4 (./copyfile -d <Source Directory> <Target Directory>)
            help(strcat("Insufficient arguments for option: ", argv[1]));  // Display error message
        } else if (argc > 4) {                                             // Maximal argc is 4 (./copyfile -d <Source Directory> <Target Directory>)
            help(strcat("Too many arguments for option: ", argv[1]));      // Display error message
        }

        task.operation = 3;  // Set operation as 3
    } else {
        help("Invalid option");  // Display error message
    }
}

/**
 * Validate source and destination
 */
int validate() {
    struct stat origin_stat, destination_stat;  // Stat structure for source and destination

    if (task.operation == 1) {  // Operation 1: Copy source file to target file
        // Soruce : Regular file, Destination : Regular file
        if (stat(dir.head->source, &origin_stat) == 0 && S_ISREG(origin_stat.st_mode) && stat(dir.head->source, &destination_stat) == 0 && S_ISREG(destination_stat.st_mode)) {
            return 0;
        } else {
            return 1;
        }
    } else if (task.operation == 2) {  // Operation 2: Copy multiple source file to target directory
        // Source : Regular file, Destination : Directory
        struct source_node *temp = dir.head;  // Temporary node for linked list
        while (1) {                           // Check the source files are regular files
            if (temp == NULL) break;          // Exit condition
            if (!(stat(temp->source, &origin_stat) == 0 && S_ISREG(origin_stat.st_mode))) {
                return 1;
            }
            temp = temp->next;  // Move to the next node
        }
        if (stat(dir.destination, &destination_stat) == 0 && S_ISDIR(destination_stat.st_mode)) {  // Check the destination is a directory
            return 0;
        } else {
            return 1;
        }
    } else if (task.operation == 3) {  // Operation 3: Copy all files and directories from source directory to target directory
        // Source : Directory, Destination : Directory
        if (stat(dir.head->source, &origin_stat) == 0 && S_ISDIR(origin_stat.st_mode) && stat(dir.destination, &destination_stat) == 0 && S_ISDIR(destination_stat.st_mode)) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

/**
 * Copy a file to another file
 */
int copy_f2f(char *origin, char *destination) {
    int fd_origin, fd_destination;  // File descriptors for source and destination
    char buffer[BUFFER_SIZE];       // Buffer for reading and writing
    ssize_t read_bytes, write_bytes;

#ifdef DEBUG
    printf("[copy_f2f] origin: %s\n", origin);
    printf("[copy_f2f] destination: %s\n", destination);
#endif

    if ((fd_origin = open(origin, O_RDONLY)) == -1) {
        perror("[copy_f2f] Opening origin file");
        return 1;
    }

    if ((fd_destination = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        perror("[copy_f2f] Opening destination file");
        return 1;
    }

    while ((read_bytes = read(fd_origin, buffer, sizeof(buffer))) > 0) {
        if ((write_bytes = write(fd_destination, buffer, read_bytes)) != read_bytes) {
            perror("[copy_f2f] Write");
            return 1;
        }
    }

    if (read_bytes == -1) {
        perror("[copy_f2f] Read");
        return 1;
    }

    if (close(fd_origin) == -1) {
        perror("[copy_f2f] Closing origin file");
        return 1;
    }

    if (close(fd_destination) == -1) {
        perror("[copy_f2f] Closing destination file");
        return 1;
    }

    if (task.verbose) {
        verbose(origin, destination);
    }

    return 0;
}

/**
 * Copy multiple sources to a target directory
 */
int copy_f2d(char *origin, char *destination) {
    struct source_node *temp = dir.head;  // Temporary node for linked list
    while (1) {
        if (temp == NULL) break;  // Exit condition

        // Destination path normalization
        char *new_dest = (char *)malloc(sizeof(char) * (strlen(destination) + strlen(temp->source) + 2));
        strcpy(new_dest, destination);
        if (strcmp(&new_dest[(int)strlen(new_dest) - 1], "/") != 0) {
            strcat(new_dest, "/");
        }

        // Source path normalization
        char *new_origin = (char *)malloc(sizeof(char) * (strlen(new_dest) + strlen(temp->source) + 1));
        strcpy(new_origin, temp->source);
        if (strrchr(new_origin, '/') != NULL) {
            new_origin = strrchr(new_origin, '/') + 1;
        }

        strcat(new_dest, new_origin);

        if (copy_f2f(temp->source, new_dest)) {
            perror("[copy_f2f] ");
            return 1;
        }

        free(new_dest);    // Free memory for new_dest
        free(new_origin);  // Free memory for new_origin

        temp = temp->next;
    }

    return 0;
}

/**
 * Copy all files and directories from source directory to target directory
 */
int copy_d2d(char *origin, char *destination) {
    DIR *dir_origin, *dir_destination;
    struct dirent *entry;

    if ((dir_origin = opendir(origin)) == NULL) {
        perror("[copy_d2d] Opening origin directory");
        return 1;
    }

    if ((dir_destination = opendir(destination)) == NULL) {
        perror("[copy_d2d] Opening destination directory");
        return 1;
    }

    while ((entry = readdir(dir_origin)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Origin path normalization
        char *new_origin = (char *)malloc(sizeof(char) * (strlen(origin) + strlen(entry->d_name) + 2));
        strcpy(new_origin, origin);
        if (strcmp(&new_origin[(int)strlen(new_origin) - 1], "/") != 0) {
            strcat(new_origin, "/");
        }
        strcat(new_origin, entry->d_name);

        // Destination path normalization
        char *new_destination = (char *)malloc(sizeof(char) * (strlen(destination) + strlen(entry->d_name) + 2));
        strcpy(new_destination, destination);
        if (strcmp(&new_destination[(int)strlen(new_destination) - 1], "/") != 0) {
            strcat(new_destination, "/");
        }
        strcat(new_destination, entry->d_name);

        struct stat origin_stat;
        if (stat(new_origin, &origin_stat) == 0) {
            if (S_ISDIR(origin_stat.st_mode)) {
                if (mkdir(new_destination, origin_stat.st_mode) == -1) {
                    perror("[copy_d2d] Creating directory");
                    return 1;
                }
                if (copy_d2d(new_origin, new_destination)) {
                    perror("[copy_d2d] ");
                    return 1;
                }
            } else if (S_ISREG(origin_stat.st_mode)) {
                if (copy_f2f(new_origin, new_destination)) {
                    perror("[copy_d2d] ");
                    return 1;
                }
            }
        }

        free(new_origin);       // Free memory for new_origin
        free(new_destination);  // Free memory for new_destination
    }

    if (closedir(dir_origin) == -1) {
        perror("[copy_d2d] Closing origin directory");
        return 1;
    }

    if (closedir(dir_destination) == -1) {
        perror("[copy_d2d] Closing destination directory");
        return 1;
    }

    if (task.verbose) {
        verbose(origin, destination);
    }

    return 0;
}

/**
 * Display verbose message if verbose mode is enabled
 */
void verbose(char *origin, char *destination) {
    printf("Copy File: %s -> %s\n", origin, destination);
}

/**
 * Display help message
 */
void help(char *error) {
    if (error != NULL) {                 // If help() called with an error message
        printf("Error: %s\n\n", error);  // Display the error message
    }
    printf(
        "Usage: ./copyfile [OPTIONS <Parameters> ...]\n\n"
        "OPTIONS:\n"
        "\t-h \t- Display this help message\n"
        "\t\t ./copyfile -h\n\n"
        "\t-f \t- Copy source file to target file\n"
        "\t\t ./copyfile -f <Source File> <Target File>\n\n"
        "\t-m \t- Copy multiple source file to target directory\n"
        "\t\t ./copyfile -m <Source file 1> <Source file 2> ... <Source file N> <Target directory>\n\n"
        "\t-d \t- Copy all files and directories from source directory to target directory\n"
        "\t\t ./copyfile -d <Source Directory> <Target Directory>\n\n\n"
        "Verbose Mode: Add \"v\" to the end of the option\n"
        "\tExample: \n"
        "\t\t ./copyfile -fv <Source file> <Target file>\n"
        "\t\t ./copyfile -mv <Source File 1> <Source File 2> ... <Source File N> <Target Directory>\n"
        "\t\t ./copyfile -dv <Source Directory> <Target Directory>\n");
    if (error != NULL) {  // If help() called with an error message
        exit(1);          // Exit with error
    }
}

// FUNCTION DEFINITIONS FOR TESTING
int parserTest(int argc, char *argv[]) {
    int parsed_argc = 1;

    if (task.operation != 0) parsed_argc++;
    if (dir.destination != NULL) parsed_argc++;

    printf("[parserTest] operation: \t%d\n", task.operation);
    printf("[parserTest] verbose enabled: \t%d\n", task.verbose);
    printf("[parserTest] parsed argv: \t%s %s ", argv[0], argv[1]);

    struct source_node *temp = dir.head;
    while (1) {
        if (temp == NULL) break;
        printf("%s ", temp->source);
        parsed_argc++;
        temp = temp->next;
    }
    printf("%s ", dir.destination);
    printf("\n");

    printf("[parserTest] original argv: \t");
    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    printf("[parserTest] parsed argc: \t%d\n", parsed_argc);
    printf("[parserTest] original argc: \t%d\n", argc);

    if (parsed_argc == argc) {
        return 0;
    } else {
        return 1;
    }
}