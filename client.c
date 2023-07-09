#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8922
#define BUFFER_SIZE 1024

WSADATA wsaData;
SOCKET server_socket, new_socket;

DWORD WINAPI ReceiveThread(LPVOID lpParam) {
    SOCKET new_socket = *(SOCKET*)lpParam;
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        //Leitura da mensagem enviada pelo cliente
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            //Fechamento dos sockets
            printf("Conexão fechada pelo cliente.\n");
            closesocket(new_socket);
            closesocket(server_socket);
            WSACleanup();
            exit(0);
        }
        printf("Cliente: %s\n", buffer);

        //Limpa o buffer
        memset(buffer, 0, BUFFER_SIZE);
    }
}

DWORD WINAPI SendThread(LPVOID lpParam) {
    SOCKET new_socket = *(SOCKET*)lpParam;
    char response[100];
    printf("Conexao estabelecida. Envie uma mensagem.\n");
    while (1) {
        //Envio da resposta para o cliente
        fgets(response, sizeof(response), stdin);
        send(new_socket, response, strlen(response), 0);
        printf("Mensagem enviada.\n");
    }

    return 0;
}

int main() {
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(client_address);

    //Inicialização do Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicialização do Winsock\n");
        return 1;
    }

    //Criação do socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Falha ao criar o socket\n");
        return 1;
    }

    //Configuração do socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("192.168.122.83");
    server_address.sin_port = htons(PORT);

    //Associação do socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Falha na associação do socket\n");
        return 1;
    }

    //Espera por conexões
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Erro na espera por conexao\n");
        return 1;
    }
    printf("Servidor aguardando conexao...\n");

    //Aceitação de uma nova conexão
    if ((new_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen)) == INVALID_SOCKET) {
        printf("Erro na aceitação da conexão\n");
        return 1;
    }

    //Criação das threads
    HANDLE receiveThread = CreateThread(NULL, 0, ReceiveThread, &new_socket, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, SendThread, &new_socket, 0, NULL);

    //Aguarda o término das threads
    WaitForSingleObject(receiveThread, INFINITE);
    WaitForSingleObject(sendThread, INFINITE);
    return 0;
}