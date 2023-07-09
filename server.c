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
        if (i != id && clientSockets[i] != INVALID_SOCKET) {
            if (send(clientSockets[i], message, strlen(message), 0) < 0) {
                printf("Erro ao enviar mensagem para o cliente\n");
            }
        }
    }
}

// Função para tratamento de um cliente individual
void ClientHandler(void* Args) {
    ThreadArgs* threadArgs = (ThreadArgs*)Args;
    SOCKET clientSocket = threadArgs->clientSocket;
    int id = threadArgs->id;
    char clientMessage[256];

    while (1) {
        memset(clientMessage, 0, sizeof(clientMessage));
        if (recv(clientSocket, clientMessage, sizeof(clientMessage), 0) <= 0) {
            printf("Cliente desconectado: %d\n", id);
            break;
        }
        char result[200];
        sprintf(result, "Cliente[%d]: %s\n", id, clientMessage);
        printf(result);
        BroadcastMessage(result, id);
    }

    closesocket(clientSocket);
    clientSockets[id] = INVALID_SOCKET;
    free(Args);
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

        // Adiciona o novo cliente ao array
        clientSockets[numClients] = clientSocket;
        numClients++;
        ThreadArgs* threadArgs = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        threadArgs->clientSocket = clientSocket;
        threadArgs->id = numClients - 1;

        printf("Novo cliente conectado\n");

        // Cria uma nova thread para tratar o cliente
        _beginthread(ClientHandler, 0, (void*)threadArgs);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
