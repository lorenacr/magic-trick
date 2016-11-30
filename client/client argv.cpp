#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int __cdecl main(int argc, char **argv){
   	int err,iResult;
   	
    WSADATA wsadata;
	SOCKET con_skt=INVALID_SOCKET; 
	
	struct addrinfo *result=NULL;
	struct addrinfo	*ptr=NULL;
	struct addrinfo	hints;
	
	char *sendmsg="msg";
	char rcvmsg[300]; 
	
//Validacao de parametros
	if(argc!=2){
		printf("usage: %s server-name\n",argv[0]);
		return 1;
	}
    
   //inicia dll de conexao com a net
    err=WSAStartup(MAKEWORD(2,0),&wsadata);
    if(err!=0){
    	printf("WSAStartup falhou.\n");
		return 1;
	}
   printf("DLL iniciada com sucesso.\n");
   
   //dados pra inicializacao da sockaddr
   ZeroMemory(&hints,sizeof(hints));
   hints.ai_family=AF_UNSPEC;
   hints.ai_socktype=SOCK_STREAM;
   hints.ai_protocol=IPPROTO_TCP;
      
   //server address and port
   if(getaddrinfo(argv[1],"27015", &hints, &result)!=0){
   	printf("getaddrinfo falhou com erro.\n ");
   	WSACleanup();
   	return 1;
   }
	
	//tenta conectar a um endereco ate que um deles funcione
	for(ptr=result;ptr!=NULL;ptr=ptr->ai_next){
	 	//cria socket pra conectar ao servidor
   		con_skt=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);		
		
		//avalia erro na inicializacao do socket
		if(con_skt==INVALID_SOCKET){
			printf("Nao foi possivel inicializar o socket. Erro: %d \n", WSAGetLastError());
			closesocket(con_skt);
			WSACleanup();
			return 1;
		}
	
		//conecta ao servidor
		if(connect(con_skt,ptr->ai_addr,(int)ptr->ai_addrlen)==SOCKET_ERROR){
			printf("%s",ptr->ai_addr->sa_data);
			printf("%s",ptr->ai_addr->sa_family);
			printf("nao foi possivel conectar. Erro: %d \n",WSAGetLastError());
			closesocket(con_skt);
			WSACleanup();
			return 1;
		}
		break;
	}
	
	//freeaddrinfo(result); //limpa o addrinfo
	
	//envia mensagem
	iResult=send(con_skt,sendmsg,strlen(sendmsg),0);
	if(iResult==SOCKET_ERROR){
		printf("Falha no envio. Codigo do erro: %d \n",WSAGetLastError());
		closesocket(con_skt);
		WSACleanup();
		return 1;
	}
	printf("bytes enviados: %d \n",iResult);
	
	//desabilita envio de mensagem
	if(shutdown(con_skt,SD_SEND)==SOCKET_ERROR){	
		printf("Falha ao fechar. Erro: %d\n",WSAGetLastError());
		closesocket(con_skt);
		WSACleanup();
		return 1;	
	}
	
	//continua recebendo ate que a conexao seja fechada
	do{	
		iResult=recv(con_skt,rcvmsg,strlen(rcvmsg),0);
		if(iResult>0)
			printf("Recebido: %d bytes.\n",iResult);
		else if(iResult==0)
			printf("Fim da conexao.\n");
		else
			printf("Falha no recebimento. Erro: %d\n", WSAGetLastError());
	}while(iResult>0);
	
	
	closesocket(con_skt);
	WSACleanup();
	
	return 0;
}
