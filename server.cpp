#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

// Definindo quantidade de clientes, threads e ações
#define NOOFCLIENTS 5
#define NOOFTHREADS 5
#define NOOFSTOCKOPTIONS 5

// Instanciando a lista de threads
pthread_t serverThread[NOOFTHREADS + 1];

// Instanciando a lista de clientes
int clientSocket[NOOFCLIENTS];
int clientCount = 0;

// Definindo a lista de possíveis ações a se listar
const char* listStockOption[30] = {"PETR4", "VALE3", "ITUB4", "BBDC4", "ABEV3", "BBAS3", "BBSE3", "BRFS3", "BRKM5", "BRML3", "CCRO3", "CIEL3", "CMIG4", "CPFE3", "CSAN3", "CSNA3", "CYRE3", "ECOR3", "EGIE3", "ELET3", "ELET6", "EMBR3", "ENBR3", "EQTL3", "ESTC3", "GGBR4", "GOAU4", "GOLL4", "HYPE3", "IGTA3"};

// Estruturando e instanciando a lista de ações
struct stockOption { string name; float price = 0.0; };
struct stockOption stockOptions[NOOFSTOCKOPTIONS];

// Variável de controle de atualização
int updatedTime = 0;

// Função para gerar um número aleatório
int randomNumber(int min, int max) {
    return min + (std::rand() % (max - min + 1));
}

// Procedimento para inicializar a lista de ações
void initializeStockOptionsValues() {
    for (int i = 0; i < NOOFSTOCKOPTIONS; i++)
        stockOptions[i].name = listStockOption[i];
}

// Função para concatenar a lista de ações
string getStockOptions() {
    string stockOptionsString = "| ";

    for (int i = 0; i < NOOFSTOCKOPTIONS; i++)
        stockOptionsString += stockOptions[i].name + " " + to_string(stockOptions[i].price) + " | ";

    return stockOptionsString;
}

// Procedimento para inicializar a lista de ações e atualizar seus respectivos valores
void *refreshStockOptions(void *arg) {
    initializeStockOptionsValues();

    while (true)
    {
        int numeroAcoesAtualizar = randomNumber(3, NOOFSTOCKOPTIONS);

        for (int i = 0; i < numeroAcoesAtualizar; i++)
        {
            int indexAcao = randomNumber(0, NOOFSTOCKOPTIONS);
            int price = randomNumber(0, 100);

            stockOptions[indexAcao].price = price;
        }

        updatedTime = time(NULL);

        sleep(3);
    }
}

// Procedimento para enviar a lista de ações para os clientes
void *sendDataToClient(void *ClientSocket) {
    int *clientSocket = (int *)ClientSocket;
    int lastUpdatedTime = 0;

    while (true) {
        if (lastUpdatedTime != updatedTime) {
            lastUpdatedTime = updatedTime;

            string price = getStockOptions();
            int sendRes = send(*clientSocket, price.c_str(), price.size() + 1, 0);

            if (sendRes == -1) {
                cout << "Error sending to client" << endl;
                continue;
            }
        }
    }
}

int main() {
    int port;
    int newSocket;
    int addrlen;
    int opt = true;

    cout << "Enter port number: ";
    cin >> port;

    for (int i = 0; i < NOOFCLIENTS; i++)
        clientSocket[i] = 0;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        cout << "Error creating socket" << endl;
        return -1;
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        cerr << "setsockopt" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

    if (bind(serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0) {
        cout << "Error binding to IP/port" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 3) == 0)
        cout << "Listening" << endl;

    addrlen = sizeof(addr);

    pthread_create(&serverThread[0], NULL, refreshStockOptions, NULL);

    while (true) {
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
            cout << "Accept" << endl;

        cout << "New connection, socket fd is " << newSocket << ", ip is " << inet_ntoa(addr.sin_addr) << ", port is " << ntohs(addr.sin_port) << endl;

        clientSocket[clientCount] = newSocket;

        pthread_create(&serverThread[clientCount + 1], NULL, sendDataToClient, (void *)&clientSocket[clientCount]);

        clientCount++;
    }
}