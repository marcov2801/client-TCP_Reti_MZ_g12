
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link della libreria per le socket

#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    const char *messagePrefix = "messaggio iniziale per il server: ";
    const char *serverName;
    struct hostent *serverHost;
    int port;

    // Inizializzazione Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Errore: WSAStartup fallito.\n");
        return 1;
    }

    // 1. Il client legge da tastiera il nome dell'host e il numero di porta del server
    printf("Inserire il nome dell'host: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
    serverName = buffer;
	
	// 1. Il client legge da tastiera il numero di porta del server
    printf("Inserire la porta (es. 53477): ");
    scanf("%d", &port);
    getchar();

    // Risoluzione dell'indirizzo IP del server con gethostbyname
    serverHost = gethostbyname(serverName);
    if (serverHost == NULL) {
        printf("Errore nella risoluzione dell'host %s\n", serverName);
        WSACleanup();
        return 1;
    }

    // Creazione della socket TCP
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Errore: socket() fallito.\n");
        WSACleanup();
        return 1;
    }

    // Configurazione dell'indirizzo del server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr = *((struct in_addr *)serverHost->h_addr_list[0]);

    // 3. Il client si connette al server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Errore: connect() fallito.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 3. Il client invia un messaggio di benvenuto
    snprintf(buffer, sizeof(buffer), "%s%s", messagePrefix, serverHost->h_name);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Ricezione del messaggio di benvenuto dal server
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        printf("Messaggio ricevuto dal server: %s\n", buffer);
    }

    while (true) {
        // 7. Il client legge una stringa di caratteri alfanumerici e la invia al server
        printf("Inserisci una stringa da inviare al server: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(clientSocket, buffer, strlen(buffer), 0);

        // 9. Il client riceve l'intero dal server
        int numeroVocali;
        bytesReceived = recv(clientSocket, (char *)&numeroVocali, sizeof(int), 0);
        if (bytesReceived > 0) {
            printf("Il server ha trovato %d vocali nella stringa.\n", numeroVocali);
            if (numeroVocali % 2 == 0) {
                printf("%d: numero di vocali pari. Fine esecuzione.\n", numeroVocali);
                system("pause");
                break;
            } else {
                printf("%d: numero di vocali dispari. Continua l'inserimento.\n", numeroVocali);
            }
        }
    }	

    // Chiusura della connessione
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

