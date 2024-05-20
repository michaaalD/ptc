Serwer (server.c):

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int listen_fd, conn_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Tworzenie gniazda
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Konfiguracja adresu serwera
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    // Bindowanie gniazda
    bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Nasłuchiwanie na połączenia
    listen(listen_fd, 10);

    // Otwarcie pliku do zapisu
    FILE* fp = fopen("odbierany_plik.txt", "wb");
    if (fp == NULL) {
        perror("Błąd otwarcia pliku");
        return 1;
    }

    // Akceptowanie połączeń i odbieranie danych
    while (1) {
        conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
        int bytes_received = recv(conn_fd, buffer, sizeof(buffer), 0);
        while (bytes_received > 0) {
            fwrite(buffer, 1, bytes_received, fp);
            bytes_received = recv(conn_fd, buffer, sizeof(buffer), 0);
        }
        close(conn_fd);
    }

    fclose(fp);
    return 0;
}

Klient (client.c):

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int socket_fd;
    struct sockaddr_in serv_addr;
    char send_buffer[1024];

    // Tworzenie gniazda
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Konfiguracja adresu serwera
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adres IP serwera

    // Nawiązywanie połączenia
    connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Otwarcie pliku do odczytu
    FILE* fp = fopen("wysylany_plik.txt", "rb");
    if (fp == NULL) {
        perror("Błąd otwarcia pliku");
        return 1;
    }

    // Wysyłanie danych do serwera
    int bytes_read = fread(send_buffer, 1, sizeof(send_buffer), fp);
    while (!feof(fp)) {
        send(socket_fd, send_buffer, bytes_read, 0);
        bytes_read = fread(send_buffer, 1, sizeof(send_buffer), fp);
    }

    fclose(fp);
    close(socket_fd);
    return 0;
}
