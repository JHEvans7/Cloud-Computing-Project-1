#include <iostream>
#include <string>
#include <cstring>
#include <time.h>
#include <vector>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")                                       //Initialization of Winsock Version 32

using namespace std;

int findPrimeInVector(vector<int>&);									// Function Declarations
int randomNumberGenerator(int);
vector<int> vectorOfPrimes(int);
bool isPrime(int p);
void returnSend(int returnInt, const SOCKET);
void debugfunction(string, const SOCKET);


void main()
{

									//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);


	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0)
	{
		cerr << "Cant intialize winsock! Quitting now..." << endl;
		return;
	}

										//Create a socket   (Thread would be created here - Each thread creates it's own listening socket - creating thread pool)

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting now..." << endl;
		return;
	}
	
								//Bind the ip address and port to a socket (Conversely a thread would have an IP address bound to it, when selected from pool)

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

								// Tell Winsock the socket is for listening

	listen(listening, SOMAXCONN);


								// Wait for a connection (now each thread would be listning for a connection)
	sockaddr_in client;
	int clientSize = sizeof(client);;

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Client Socket invalid! Quitting now..." << endl;
		return;
	}

	char host[NI_MAXHOST];					//Client's remote name
	char service[NI_MAXHOST];				// Service (i.e. port) the client is connected on


	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);
											//Alternative Code: memset(host, 0, NI_MAXHOST); (For Python)

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}


												// Close listening socket

	closesocket(listening);

								// While loop: accept and echo message back to client


	char buf[4096];
	string debugString;

	while (true)
	{
		ZeroMemory(buf, 4096);

		
		// Wait for client to send data

		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting now..." << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}
		
										//echo message recieved by server
		cout << string(buf, 0, bytesReceived) << endl;


									// String Variable for debugging purposes
		debugString = string(buf, 0, bytesReceived);

									//Main Function Call: Sending socket and string for operations

		debugfunction(debugString, clientSocket);

		// Echo message back to client
		//send(clientSocket, buf, bytesReceived + 1, 0);

	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();

}


int findPrimeInVector(vector<int>& processedVector)							// Find a random entry within a vector and return it
{
	int randomValue;
	
	int random = rand() % processedVector.size();							// size() makes sure that it never crosses the bounds of the Vector

	randomValue = processedVector[random];

	return randomValue;
}

int randomNumberGenerator(int returnINT)									// Finds the numerical value within the vector of all prime numbers and returns it
{
	int a = returnINT;
	int randomPrime;
	vector<int> processedVector;
	
	processedVector = vectorOfPrimes(a);

	randomPrime = findPrimeInVector(processedVector);

	return randomPrime;

}


vector<int> vectorOfPrimes(int intToBePrimed)								// Builds the vector of all primes, using user input as bounds 
{
	vector<int> primeVector;
	
	for (int i = 2; i <= intToBePrimed; i++)
	{
		if (isPrime(i))
		{
			primeVector.push_back(i);
		}
	}

	return primeVector;
}



void returnSend(int returnInt, const SOCKET clientsocket)					// Response function. Converts INT type to C String type for communication over TCP
{

		int valueToConvert = returnInt; 
		string returnString = to_string(valueToConvert);


		send(clientsocket, returnString.c_str(), returnString.size() + 1, 0);

}

bool isPrime(int stringToPrime)												// Simple boolean is number prime or not.
{
	if (stringToPrime <= 1)													// Edge case.
		return false;

	for (int i = 2; i < stringToPrime; i++)
		if (stringToPrime % i == 0)
			return false;

	return true;
}

void debugfunction(string x, const SOCKET clientsocket)						// Somewhat redundant function. exists for debugging purposes between process and response.
{
	int stringtoPrime = stoi(x);
	int randomPrimetoSend = randomNumberGenerator(stringtoPrime);
	SOCKET funcSocket = clientsocket;
	
	//Commented out for debugging
	//cout << endl << "In String Function, Variable is: " << x << endl;
	//cout << endl << "In String Function, Randomized Variable is: " << stringtoPrime << endl << endl;
	//cout << endl << "Now attempting a return send... " << endl;
	//Commented out for debugging

	returnSend(randomPrimetoSend, funcSocket);

	return;
}


