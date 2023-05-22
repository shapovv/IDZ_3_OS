#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256
#define DELAY 5

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Используйте: %s <hostname> <port> <group number>\n", argv[0]);
        exit(0);
    }

    char *hostname = argv[1];
    int port = atoi(argv[2]);
    int group_number = atoi(argv[3]);
    int client_sock;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Ошибка: нет такого хоста\n");
        exit(0);
    }

    while (1) {
        client_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (client_sock < 0) {
            error("Не удалось открыть сокет");
        }

        bzero((char *) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
        server_addr.sin_port = htons(port);

        if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            error("Ошибка подключения");
        }

        bzero(buffer, BUFFER_SIZE);
        int n = read(client_sock, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            error("Ошибка чтения из сокета");
        }

        if (strcmp(buffer, "TREASURE_FOUND") == 0) {
            printf("Джон Сильвер: Клад найден! Завершение работы группы %d.\n", group_number);
            close(client_sock);
            exit(0);
        }

        int area = atoi(buffer);
        printf("Ваша группа %d отправлена на участок %d.\n", group_number, area);

        bzero(buffer, BUFFER_SIZE);
        sprintf(buffer, "Группа %d проверила участок %d", group_number, area);
        n = write(client_sock, buffer, strlen(buffer));
        if (n < 0) {
            error("Ошибка записи в сокет");
        }

        close(client_sock);
        sleep(DELAY);  // задержка перед следующей попыткой
    }

    return 0;
}
