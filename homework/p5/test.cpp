#include<iostream>
#include<cstring>
using namespace std;


constexpr int N = 64,INF = 0x3f3f'3f3f;
using DV = int[N];
DV matrix[2][N], *pcur, * ppre;
// using Matrix =DV[N];
int n,m;
int dis[N][N];
const char* dict =" xyzuv";
void init(){
  memset(dis,0x3f,sizeof(dis));
  memset(matrix,0x3f,sizeof(matrix));
  cin>>n>>m;
  int u,v,w;
  for(int i=0;i<m;++i){
    cin>>u>>v>>w;
    dis[u][v]=dis[v][u] = w;
  }
  pcur =matrix[0], ppre = matrix[1];
  for(int i=1;i<=n;++i) ppre[i][i] =0;
  for(int i =1;i<=n;++i) dis[i][i] =0;
}
void update_i(int u){
  memset(pcur[u],0x3f,sizeof(DV));
  pcur[u][u] =0;
  for(int i =1;i<=n;++i){
    for(int j =1;j<=n;++j){
      pcur[u][i] = min(ppre[u][j] + dis[j][i],pcur[u][i]);
    }
  }
}
void show_i(int u,int iter){
  cout<<iter<<"'th iterate vectex: "<<dict[u]<<'\n';
  cout<<'|';
  for(int i =0;i<=n;++i) cout<<dict[i]<<" |";
  cout<<"\n|";
  for(int i =0;i<=n;++i) cout<<"-|";
  cout<<endl;
  int* pshow;
  for(int k =1;k<=n;++k){
    if(dis[u][k] >= INF) continue;
    cout<<'|'<<dict[k]<<" |";
    if(k == u) pshow =pcur[k];
    else pshow = ppre[k];
    for(int i =1;i<=n;++i){
      if(pshow[i] >= INF) cout<<-1;
      else cout<<pshow[i];
      cout<<" |";
    }
    cout<<'\n';
  }
  cout<<'\n';
}
int target;
void update_once(int iter){
  for(int i=1;i<=n;++i){
    update_i(i);
  }
  // for(int i =1;i<=n;++i) show_i(i);
  show_i(target,iter);
  std::swap(pcur,ppre);
}

int main(){
  init();
  cin>>target;
  for(int i =1;i<=5;++i) update_once(i);
}
