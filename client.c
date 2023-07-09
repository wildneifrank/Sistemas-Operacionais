#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#define PORT 8922

SOCKET clientSocket;

// Função para envio de mensagens
void SendMessageThread() {
    while (1) {
        // Enviar dados ao servidor
        char message[256];

        fgets(message, sizeof(message), stdin);
        if (send(clientSocket, message, strlen(message), 0) < 0) {
            printf("Erro ao enviar dados ao servidor\n");
            break;
        }
    }
}

// Função para recebimento de mensagens
void ReceiveMessageThread() {
    while (1) {
        // Receber resposta do servidor
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
            printf("servidor encerrou a conexão\n");
            closesocket(clientSocket);
            WSACleanup();
            exit(0);
        }
        printf("%s", buffer);
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erro ao inicializar o Winsock\n");
        return 1;
    }

    // Criar o socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket\n");
        WSACleanup();
        return 1;
    }

    // Definir o endereço do servidor
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.0.4");  // Coloque o IP do servidor aqui

    // Conectar ao servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Erro ao conectar ao servidor\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    printf("Conexao estabelecida com sucesso!\nDigite uma mensagem.\n");

    // Criar threads para envio e recebimento de mensagens
    _beginthread(SendMessageThread, 0, NULL);
    _beginthread(ReceiveMessageThread, 0, NULL);

    // Aguardar pelo término das threads
    while (1) {
        // Espera infinitas
    }

    // Fechar o socket e finalizar o Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}