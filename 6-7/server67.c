#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Используйте: %s <IP> <PORT> <AREA_COUNT> <TREASURE_AREA>\n", argv[0]);
        exit(1);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int AREA_COUNT = atoi(argv[3]);
    int TREASURE_AREA = atoi(argv[4]);

    if (TREASURE_AREA < 1 || TREASURE_AREA > AREA_COUNT) {
        printf("TREASURE_AREA должен находиться в диапазоне от 1 до AREA_COUNT\n");
        exit(1);
    }

    int server_sock, client_sock, next_area = 1;
    socklen_t client_length;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        error("Не удалось открыть сокет");
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("Ошибка привязки");
    }

    listen(server_sock, 5);
    client_length = sizeof(client_addr);

    FILE *logFile = NULL;

    while (1) {
        logFile = fopen("server.log", "a");
        if (logFile == NULL) {
            error("Ошибка открытия файла лога");
        }
        fprintf(logFile, "Ожидание подключения...\n");
        fclose(logFile);

        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_length);
        if (client_sock < 0) {
            error("Ошибка при приеме");
        }

        logFile = fopen("server.log", "a");
        if (logFile == NULL) {
            error("Ошибка открытия файла лога");
        }
        fprintf(logFile, "Подключена группа.\n");
        fclose(logFile);

        bzero(buffer, BUFFER_SIZE);
        sprintf(buffer, "%d", next_area);
        int n = write(client_sock, buffer, strlen(buffer));
        if (n < 0) {
            error("Ошибка записи в сокет");
        }

        bzero(buffer, BUFFER_SIZE);
        n = read(client_sock, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            error("Ошибка чтения из сокета");
        }

        logFile = fopen("server.log", "a");
        if (logFile == NULL) {
            error("Ошибка открытия файла лога");
        }
        fprintf(logFile, "Группа сообщила: %s\n", buffer);
        fclose(logFile);

        if (next_area == TREASURE_AREA) {
            logFile = fopen("server.log", "a");
            if (logFile == NULL) {
                error("Ошибка открытия файла лога");
            }
            fprintf(logFile, "Клад найден!\n");
            fclose(logFile);
            break;
        }

        next_area++;
        if (next_area > AREA_COUNT) {
            next_area = 1;
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
