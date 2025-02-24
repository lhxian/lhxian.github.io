#include<utility>
using namespace std;
using pii = pair<int,int>;
constexpr int N = 1e2;
class MinStack{
  pii s[N];
  int pos = -1;

  pii& top(){
    if(pos == -1) return s[N-1];
    return s[pos];
  }

  void push(int val);
  void pop(){
    if(pos > -1) --pos;
  }
};

void MinStack::push(int val){
  s[++pos] = {val,val};
  if(pos > 0){
    s[pos].second = min(s[pos].second,s[pos-1].second);
  }
}