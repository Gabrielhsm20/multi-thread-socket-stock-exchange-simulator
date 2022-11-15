#include <iostream>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

int main() {
    int port;
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    int bytesReceived;
    char buf[4096];

    if (clientSocket == -1) {
        cout << "Error creating socket" << endl;
        return -1;
    }

    cout << "Enter port number: ";
    cin >> port;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

    int connectRes = connect(clientSocket, (sockaddr *)&addr, sizeof(addr));

    if (connectRes == -1)
    {
        cout << "Error connecting to server" << endl;
        return -1;
    }
    else if (connectRes == 0)
    {
        cout << "Connected to server" << endl;
    }

    do {
        memset(buf, 0, 4096);
        bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "Error receiving from server" << endl;
        }
        else
        {
            cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
        }
    } while (true);
}