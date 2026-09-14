#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];
    char message[] = "Hello from parent!\n";

    pipe(pipefd);

    pid_t pid = fork();

    if (pid == 0) {
        // Child：只讀，不寫
        close(pipefd[1]);

        char buffer[64];
        ssize_t bytes_read;

        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("Child read: %s", buffer);
        }

        printf("Child saw EOF: no writers remain.\n");
        close(pipefd[0]);
    } else {
        // Parent：只寫，不讀
        close(pipefd[0]);

        write(pipefd[1], message, strlen(message));
        printf("Parent wrote the message.\n");

        close(pipefd[1]);
        waitpid(pid, NULL, 0);
        printf("Parent: child has finished.\n");
    }

    return 0;
}
