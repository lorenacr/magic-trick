#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int main (void) 
{
	int functionResult;
   	
    WSADATA wsadata;
	SOCKET clientSocket = INVALID_SOCKET; 
	SOCKET listenSocket = INVALID_SOCKET; 
	
	struct addrinfo *result = NULL;
	struct addrinfo	hints;
	
	char messageReceived[300]; 
	char messageSent[300];
       
   	// inicia dll de conexao com a net
    functionResult = WSAStartup (MAKEWORD(2,0), &wsadata);
    if (functionResult != 0) 
	{
    	printf("WSAStartup falhou.\n");
		return 1;
	}
	
	// dados pra inicializacao da sockaddr
   	ZeroMemory (&hints, sizeof (hints));
   	hints.ai_family = AF_INET;
   	hints.ai_socktype = SOCK_STREAM;
   	hints.ai_protocol = IPPROTO_TCP;
   	hints.ai_flags = AI_PASSIVE;
      
   	// server address and port
   	functionResult = getaddrinfo ("127.0.0.1", "27015", &hints, &result);
   	if (functionResult != 0) 
	{
   		printf ("getaddrinfo falhou com erro: %d.\n", WSAGetLastError());
		WSACleanup ();
   		return 1;
   	}
   
   	// cria socket pra conectar no servidor
    listenSocket = socket (result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) 
	{
    	printf("socket falhou com erro: %d\n", WSAGetLastError());
        freeaddrinfo (result);
        WSACleanup ();
        return 1;
	}
   	else
   		printf ("Socket criado com sucesso.\n");
   
   	// seta um socket tcp que fica escutando
   	functionResult = bind (listenSocket, result->ai_addr, result->ai_addrlen);
   	if (functionResult == SOCKET_ERROR)
	{
   		printf ("Bind falhou. Codigo do erro: %d \n", WSAGetLastError());
   		freeaddrinfo (result);
   		closesocket (listenSocket);
   		WSACleanup ();
   		return 1;
   	}
	printf ("Bind realizado com sucesso.\n");
	
   	freeaddrinfo (result);
   
  	// fica aberto a novas conexoes
   	functionResult = listen(listenSocket, 10);
   	if(functionResult == SOCKET_ERROR)
	{
   		printf("Listen falhou. Codigo do erro: %d \n", WSAGetLastError());
   		closesocket (listenSocket);
   		WSACleanup ();
   		return 1;
   	}
   	printf ("Listen realizado com sucesso.\n");
   
   	// aceita conexao de um socket cliente
   	clientSocket = accept (listenSocket, NULL, NULL);
   	if(clientSocket == INVALID_SOCKET)
	{
   		printf ("Accept falhou. Codigo do erro: %d \n", WSAGetLastError());
   		closesocket (listenSocket);
   		WSACleanup ();
   		return 1;	
   }
   
   	// nao precisa mais do server socket
   	closesocket (listenSocket);
    
		
	while (1)
	{
		// recebe mensagem primeiro
	 	functionResult = recv (clientSocket, messageReceived, sizeof(messageReceived),0);
        if (functionResult > 0) { printf("Amigo: %s", messageReceived); }
        else if (functionResult == 0) { printf ("Fim da conexao.\n"); }	
		else 
		{
			printf ("Falha no recebimento. Erro: %d\n", WSAGetLastError ());
			closesocket (clientSocket);
            WSACleanup ();
			return 0;
		}
            
	   // envia mensagem
        printf ("Voce: ");
        fgets (messageSent, 300, stdin);
		if (strlen (messageSent) > 300)
		{
        	printf ("Digite uma mensagem com menos de 300 caracteres.\n");
        	continue;
       	}

		functionResult = send (clientSocket, messageSent, strlen(messageSent), 0);
		if (functionResult == SOCKET_ERROR)
		{
			printf ("Falha no envio. Codigo do erro: %d \n", WSAGetLastError ());
			closesocket (clientSocket);
			WSACleanup ();
			return 0;
		}	
	}
			
    // shutdown no envio de mensagens
    functionResult = shutdown (clientSocket, SD_SEND);
    if (functionResult == SOCKET_ERROR) 
	{
        printf ("Shutdown falhou com erro: %d\n", WSAGetLastError());
        closesocket (clientSocket);
        WSACleanup ();
        return 0;
    }

    // limpeza
    closesocket (clientSocket);
    WSACleanup ();

	return 0;
}

	
