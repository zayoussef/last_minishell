#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

void execute_command_with_redirection(char* command, char* filename) 
{
    int fd;
    pid_t pid;

    // Step 1: Fork a process
    pid = fork();

    if (pid == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
    {
        // Step 2: Child process handles the redirection
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) 
        {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to the file
        if (dup2(fd, STDOUT_FILENO) == -1) 
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(fd);
        // Step 3: Execute the command
        execlp(command, command, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else 
    {
        // Parent process waits for the child to complete
        wait(NULL);
    }
}

int main(int argc, char* argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <command> <operator> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[2], ">") == 0) 
    {
        execute_command_with_redirection(argv[1], argv[3]);
    }
    else 
    {
        fprintf(stderr, "Unsupported operator: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}