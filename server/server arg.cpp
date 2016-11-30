#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int main(void){
	int iResult,iSendResult;
   	
    WSADATA wsadata;
	SOCKET client_skt=INVALID_SOCKET; 
	SOCKET listen_skt=INVALID_SOCKET; 
	
	struct addrinfo *result=NULL;
	struct addrinfo	hints;
	
	char rcvmsg[300]; 

       
   //inicia dll de conexao com a net
    iResult=WSAStartup(MAKEWORD(2,0),&wsadata);
    if(iResult!=0){
    	printf("WSAStartup falhou.\n");
		return 1;
	}
	
	//dados pra inicializacao da sockaddr
   ZeroMemory(&hints,sizeof(hints));
   hints.ai_family=AF_INET;
   hints.ai_socktype=SOCK_STREAM;
   hints.ai_protocol=IPPROTO_TCP;
   hints.ai_flags=AI_PASSIVE;
      
   //server address and port
   iResult=getaddrinfo("127.0.0.1","27015", &hints, &result);
   if(iResult!=0){
   	printf("getaddrinfo falhou com erro: %d.\n",WSAGetLastError());
	WSACleanup();
   	return 1;
   }
   
   // cria socket pra conectar no servidor
    listen_skt=socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_skt==INVALID_SOCKET) {
        printf("socket falhou com erro: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
	}
   else
   		printf("socket criado com sucesso.\n");
   
   //seta um socket tcp que fica escutando
   iResult=bind(listen_skt,result->ai_addr,result->ai_addrlen);
   if(iResult==SOCKET_ERROR){
   		printf("bind falhou. Codigo do erro: %d \n",WSAGetLastError());
   		freeaddrinfo(result);
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;
   }
   	printf("bind realizado com sucesso.\n");
   	freeaddrinfo(result);
   
   //fica aberto a novas conexoes
   iResult=listen(listen_skt,10);
   if(iResult==SOCKET_ERROR){
   		printf("listen falhou. Codigo do erro: %d \n",WSAGetLastError());
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;
   }
   printf("listen realizado com sucesso.\n");
   
   //aceita conexao de um socket cliente
   client_skt=accept(listen_skt,NULL,NULL);
   if(client_skt==INVALID_SOCKET){
   		printf("accept falhou. Codigo do erro: %d \n",WSAGetLastError());
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;	
   }
   
   //nao precisa mais do server socket
   closesocket(listen_skt);
    
	//recebe ate que a conexao seja fechada
   do {
        iResult=recv(client_skt,rcvmsg,sizeof(rcvmsg),0);
        if (iResult>0) {
            printf("Bytes recebidos: %d\n", iResult);

        //manda o resultado de volta pra quem enviou
            iSendResult=send(client_skt,rcvmsg,iResult,0);
            if (iSendResult==SOCKET_ERROR) {
                printf("falha no envio da mensagem. Codigo de erro: %d\n", WSAGetLastError());
                closesocket(client_skt);
                WSACleanup();
                return 1;
            }
            printf("Bytes enviados: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Fechando conexao...\n");
        else  {
            printf("falha no recebimento. Codigo do erro: %d\n", WSAGetLastError());
            closesocket(client_skt);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    //shutdown no envio de mensagens
    iResult = shutdown(client_skt, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown falhou com erro: %d\n", WSAGetLastError());
        closesocket(client_skt);
        WSACleanup();
        return 1;
    }

    //limpeza
    closesocket(client_skt);
    WSACleanup();

return 0;
}

	
