#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#define PORT 8922

SOCKET clientSockets[100];  // Array para armazenar os sockets dos clientes
int numClients = 0;  // Contador de clientes conectados
typedef struct {
    SOCKET clientSocket;
    int id;
    // Outros argumentos que você precisa passar para a função de thread
} ThreadArgs;

// Função para envio de mensagens a todos os clientes
void BroadcastMessage(char* message, int id) {
    for (int i = 0; i < numClients; i++) {
        if (i != id) {
            if (send(clientSockets[i], message, strlen(message), 0) < 0) {
                printf("Erro ao enviar mensagem para o cliente\n");
            }
        }
    }
}


int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddress, clientAddress;
    int addrlen = sizeof(clientAddress);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erro ao inicializar o Winsock\n");
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket\n");
        WSACleanup();
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Erro na associação do socket\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erro na espera por conexões\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Servidor aguardando conexões...\n");

    while (1) {
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Erro na aceitação da conexão\n");
            continue;
        }

    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}