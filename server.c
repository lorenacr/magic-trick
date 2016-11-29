#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int main(void){
	int err,iResult,iSendResult;
   	
    WSADATA wsadata;
	SOCKET client_skt=INVALID_SOCKET; 
	SOCKET listen_skt=INVALID_SOCKET; 
	
	struct addrinfo *result=NULL;
	struct addrinfo	hints;
	
	char rcvmsg[300]; 

       
   //inicia dll de conexao com a net
    err=WSAStartup(MAKEWORD(2,0),&wsadata);
    if(err!=0){
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
   if(getaddrinfo(NULL,"27015", &hints, &result)!=0){
   	printf("getaddrinfo falhou.\n");
   	WSACleanup();
   	return 1;
   }
   
   // cria socket pra conectar no servidor
    listen_skt=socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_skt==INVALID_SOCKET) {
        printf("socket falhou com erro: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
	}
   
   if(bind(listen_skt,result->ai_addr,result->ai_addrlen)==SOCKET_ERROR){
   		printf("bind falhou. Codigo do erro: %ld \n",WSAGetLastError());
   		freeaddrinfo(result);
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;
   }
   
   if(listen(listen_skt,10)==SOCKET_ERROR){
   		printf("listen falhou. Codigo do erro: %ld \n",WSAGetLastError());
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;
   }
   
   client_skt=accept(listen_skt,NULL,NULL);
   if(client_skt==INVALID_SOCKET){
   		printf("accept falhou. Codigo do erro: %ld \n",WSAGetLastError());
   		closesocket(listen_skt);
   		WSACleanup();
   		return 1;	
   }
   //nao precisa mais do server socket
   closesocket(listen_skt);
   
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

	
