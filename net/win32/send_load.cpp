#include<WinSock2.h>
#include<cassert>
#include<cstring>
#include<WS2tcpip.h>
#include<cstdio>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")
#define MAX_BUF_SIZE 1024
char input[128];
void LoadF(SOCKET servSock,const char* filename);
void SendF(SOCKET servSock,const char* filename);
int main(int argc,char* argv[]){
  if(argc < 3)  return 1;
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2,2),&wsaData);
  SOCKET servSock=socket(PF_INET,SOCK_STREAM,0);
  sockaddr_in servAddr = {0};
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));

  if(inet_pton(AF_INET,argv[1],&servAddr.sin_addr) <= 0) return 2;
  if(connect(servSock,(sockaddr*)&servAddr,sizeof(sockaddr)) == INVALID_SOCKET){
    return 3;
  }
  std::cout<<"connect the server\n";
  std::cin>>input;
  int len = strlen(input);
  input[len] = '\n';
  send(servSock,input,len+1,0);
  input[len] = 0;
  char holder[4];
  std::cout<<"input "<<input<<std::endl;
  switch(input[0]){
    case 'S':{
      int len= recv(servSock,holder,1,0);
      assert(len > 0);
      std::cout<<"get char: "<<holder[0]<<std::endl;
      SendF(servSock,input+1);
      len = recv(servSock,holder,1,0);
      assert(len > 0);
      std::cout<<"get last char: "<<holder[0]<<std::endl;
    }
    break;
    case 'R':
    LoadF(servSock,input+1);
    break;
    default:
    return 4;
  }
  closesocket(servSock);
  WSACleanup();
}

void LoadF(SOCKET servSock,const char* filename){
  std::cout<<"load\n";
  FILE* pf= fopen(filename,"wb");
  char* buf = new char[MAX_BUF_SIZE];
  int len = 0;
  while((len =recv(servSock,buf,MAX_BUF_SIZE,0)) > 0){
    fwrite(buf,len,1,pf);
  }
  delete[] buf;
  fclose(pf);
}

void SendF(SOCKET servSock,const char* filename){
  std::cout<<"send\n";
  FILE* pf = fopen(filename,"rb");
  if(!pf) return;
  char* buf = new char[MAX_BUF_SIZE];
  int len = 0;
  while((len = fread((void*)buf,MAX_BUF_SIZE,1,pf)) > 0){
    std::cout<<"read\n";
    send(servSock,buf,len,0);
  }
  recv(servSock,buf,1,0);
  fclose(pf);
  delete[] buf;
}