#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iostream>
#include<cassert>
#include<cstdlib>
#pragma comment(lib,"ws2_32.lib")
constexpr int N = 1024;
char buf[N];
int main(int argc,char* argv[]){
  if(argc < 3) exit(EXIT_FAILURE);
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0) exit(EXIT_FAILURE);

  SOCKET clntSocket;
  clntSocket = socket(AF_INET,SOCK_STREAM,0);
  assert(clntSocket != INVALID_SOCKET);
  sockaddr_in servAddr = {0};
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));

  if(inet_pton(AF_INET,argv[1],&servAddr.sin_addr) <= 0)
    exit(EXIT_FAILURE);

  if(connect(clntSocket,(sockaddr*)&servAddr,sizeof(sockaddr))
  == INVALID_SOCKET) exit(EXIT_FAILURE);

  int len =recv(clntSocket,buf,N,0);
  if(len == SOCKET_ERROR) exit(EXIT_FAILURE);
  buf[len] = 0;
  std::cout<<buf<<std::endl;

}