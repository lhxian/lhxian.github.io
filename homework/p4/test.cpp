#include<iostream>
#include<queue>
#include<cstring>
using namespace std;
const char* dict =" xyztuvw";
constexpr int N = 1e2+1,INF =0x3f3f'3f3f;
int head[N],cnt,n,m;
int d[N];
bool vis[N];
struct E{
  int nex,to,w;
}e[N*2];
struct Info{
  int u,d;
  bool operator >(const Info& other) const{
    return d > other.d;
  }
};
inline void add(int u,int v,int w){
  e[++cnt] = {head[u],v,w};
  head[u] = cnt;
}
void printHead(){
  cout<<'|';
  for(int i =0;i<=n;++i) cout<<dict[i]<<" |";
  cout<<endl;
  cout<<'|';
  for(int i=0;i<=n;++i) cout<<"-|";
  cout<<endl;
}
void dij(int s){
  cout<<"source "<<dict[s]<<'\n';
  printHead();
  priority_queue<Info,vector<Info>,greater<Info>> q;
  memset(d,0x3f,sizeof(d));
  memset(vis,0,sizeof(vis));
  d[s] = 0;
  q.push({s,0});
  while(q.size()){
    Info tem = q.top();
    q.pop();
    int u = tem.u;
    if(vis[u])  continue;
    vis[u] = true;
    for(int i =head[u];i;i =e[i].nex){
      int v =e[i].to;
      int tem_d = tem.d + e[i].w;
      if(d[v] > tem_d){
        d[v] = tem_d;
        q.push({v,d[v]});
      }
    }
    cout<<"| "<<dict[u]<<" |";
    for(int i =1;i<=n;++i){
      if(d[i] == INF) cout<<-1;
      else cout<<d[i];
      cout<<" |";
    } 
    cout<<'\n';
  }
  cout<<'\n';
}

int main(){
  cin>>n>>m;
  int u,v,w;
  for(int i =0;i<m;++i){
    cin>>u>>v>>w;
    add(u,v,w); add(v,u,w);
  }
  for(int i=1;i<=n;++i) dij(i);
}