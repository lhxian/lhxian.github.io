#include<WinSock2.h>
#include<WS2tcpip.h>
#include<cstdlib>
#include<iostream>
#include<fstream>
using namespace std;
constexpr  int N = 2048 ;

#define PORT 59036
#pragma comment(lib,"ws2_32.lib")

int main(int argc,char* argv[]){
  if(argc < 2) exit(EXIT_FAILURE);
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0) exit(EXIT_FAILURE);
  SOCKET clntSock;
  clntSock = socket(PF_INET,SOCK_STREAM,0);
  if(clntSock == -1) exit(EXIT_FAILURE);

  sockaddr_in servAddr = {0};
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(PORT);

  if(inet_pton(AF_INET,argv[1],&servAddr.sin_addr) <= 0) exit(EXIT_FAILURE);

  if(connect(clntSock,(sockaddr*)&servAddr,sizeof(sockaddr)) == INVALID_SOCKET)
    exit(EXIT_FAILURE);

  cout<<"connect the server, begin reveive file..."<<endl;
  char* pbuf = new char[N];
  int len = 0;
  ofstream ofs("send.txt",ios::out | ios::binary);
  while((len = recv(clntSock,pbuf,N,0)) > 0){
    ofs.write(pbuf,len);
  }
  ofs.close();
  delete[] pbuf;
  WSA
}
