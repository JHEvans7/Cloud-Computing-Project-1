#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")


using namespace std;


int valueCheck(int);

void main()
{

	string ipaddress = "127.0.0.1";
	int port = 54000;


	// Initialize winsock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);

	if (wsResult != 0)
	{
		cerr << "Can't start winsock, Err # " << wsResult << endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipaddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err# " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	// Do-while loop to send and recieve data
	char buf[4096];
	string userInput;
	int primeValue;

	do
	{
		//Prompt the user for some text
		cout << "Please enter a value so that we can find all the Prime numbers > ";
		cin >> primeValue;

		primeValue = valueCheck(primeValue);

		userInput = to_string(primeValue);

		if (userInput.size() > 0)  //Make sure user hast typed in something
		{	//send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesRecieved = recv(sock, buf, 4096, 0);

				if (bytesRecieved > 0)
				{
					//Echo response to console
					cout << "Server> " << string(buf, 0, bytesRecieved) << endl;
				}

			}
		}
		//wait for repsonse
		//echo the repspons
	} while (userInput.size() > 0);


	// Gracefully close down everything

	closesocket(sock);
	WSACleanup();

}


int valueCheck(int s)
{
	int returnValue = s;
	
	//cout << "In function testing x as: " << x << endl;  (just more debugging code here)
	
	while (returnValue > 30000)
	{
		if (returnValue > 30000)											// This part is a nested if-while loop designed to force the user to give me a nice number to display
		{																	// multithreading within a parameter of my choosing
			cout << endl << "Number is greater than 30,000!"
				<< endl << "Please enter a new number: ";
			cin >> returnValue;
		}
	}

	return returnValue;
}