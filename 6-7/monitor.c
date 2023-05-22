#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    FILE *logFile;
    char buffer[BUFFER_SIZE];

    while (1) {
        logFile = fopen("server.log", "r");
        if (logFile == NULL) {
            error("Ошибка открытия файла лога");
        }

        while (fgets(buffer, BUFFER_SIZE, logFile) != NULL) {
            printf("%s", buffer);
        }

        fclose(logFile);
        sleep(1);  // задержка перед следующей проверкой
    }

    return 0;
}
