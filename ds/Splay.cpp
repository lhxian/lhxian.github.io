#include<iostream>
#include<ctime>
#include<cstdlib>
#define NDEBUG
#include<cassert>
using namespace std;
struct Node;
using PNODE = Node*;
struct Node{
  int val,cnt;
  PNODE p,l,r;
  Node() = delete;
  Node(int val):val(val),cnt(1){
    p = l = r = nullptr;
  }
  void Joint();
  void Rotate();
};


void Rprint(PNODE node){
  if(!node) return;
  Rprint(node->l);
  // cout<<node->val<<'('<<node->cnt<<')'<<' ';
  cout<<node->val<<' ';
  Rprint(node->r);
}
void Node::Joint(){
  if(l) l->p = this;
  if(r) r->p = this;
}

void Node::Rotate(){
  if(!p)  return;
  PNODE pparent = p,pgrand = pparent->p;
  bool Dire1 = pparent->l == this,Dire2;
  if(pgrand)  Dire2 = pgrand->l == pparent;
  else Dire2 = Dire1;

  if(Dire1 == Dire2){
    if(Dire1){
      pparent->l = r;
      r = pparent;
      if(pgrand) pgrand->l = this;
    }else{
      assert(pparent->r == this);
      pparent->r = l;
      l = pparent;
      if(pgrand) pgrand->r = this;
    }
    p = pgrand;
  }else{
    assert(pgrand);
    if(Dire2){
      assert(pgrand->l == pparent);
      pgrand->l = r;
      pparent->r = l;
      l = pparent; r = pgrand;
    }else{
      assert(pgrand->r == pparent);
      pgrand->r = l;
      pparent->l = r;
      l = pgrand; r = pparent;
    }
    p = pgrand->p;
    if(p){
      if(p->l == pgrand) p->l = this;
      else {
        assert(p->r == pgrand);
        p->r = this;
      }
    }
    pgrand->Joint();
  }
  pparent->Joint();
  Joint();
}

class Splay{
protected:
  PNODE root;
public:
  Splay():root(nullptr){}
  ~Splay();
  void Add(int target);
  void Remove(int target);
  PNODE Maintain(PNODE node);
  PNODE Find(int target);
  PNODE Merge(PNODE t1,PNODE t2);
  int Deepth();
  void print();
};
int RDeep(PNODE root){
  if(!root) return 0;
  return max(RDeep(root->l),RDeep(root->r)) + 1;
}
int Splay::Deepth(){
  return RDeep(root);
}
Splay::~Splay(){
  PNODE cur = root,tem = nullptr;
  while(cur){
    if(cur->l){
      cur = cur->l;
    }else if(cur->r){
      cur = cur->r;
    }else{
      tem = cur->p;
      if(tem){
        if(tem->l == cur) tem->l = nullptr;
        else {
          assert(tem->r == cur);
          tem->r = nullptr;
        }
      }
      delete cur;
      cur = tem;
    }
  }
  root = nullptr;
}
void Splay::print(){
  Rprint(root);
  cout<<endl;
}

PNODE Splay::Find(int target){
  PNODE res = root;
  while(res){
    if(res->val > target) res = res->l;
    else if(res->val < target) res = res->r;
    else break;
  }
  return res;
}

PNODE Splay::Maintain(PNODE node){
  if(!node) return nullptr;
  while(node->p){
    // cout<<"rotate in maintain"<<endl;
    node->Rotate();
  }
  return node;
}

void Splay::Add(int target){
  if(!root){
    root = new Node(target);
    return;
  }
  PNODE cur = root,pre = nullptr;
  while(cur){
    pre = cur;
    if(cur->val > target) cur = cur->l;
    else if(cur->val < target) cur = cur->r;
    else{
      ++cur->cnt;
      return;
    }
  }
  cur = new Node(target);
  cur->p = pre;
  if(pre->val > target)  pre->l = cur;
  else pre->r = cur;
  Maintain(cur);
  root = cur;
}
void Splay::Remove(int target){
  PNODE pt = Find(target);
  if(!pt) return;
  --pt->cnt;
  if(pt->cnt > 0) return;

  pt = Maintain(pt);
  assert(pt->val == target);
  root = Merge(pt->l,pt->r);
  delete pt;
}

PNODE Splay::Merge(PNODE t1,PNODE t2){
  if(t1) t1->p = nullptr;
  if(t2)  t2->p = nullptr;
  if(!t1 || !t2){
    return t1? t1: t2;
  }
  if(t1->r){
    PNODE newt1 = t1;
    while(newt1->r) newt1 = newt1->r;
    t1 = newt1;
    Maintain(t1);
  }
  assert(t1->r == nullptr);
  t1->r = t2;
  t2->p = t1;
  return t1;
}

void test(){
  int n,m;
  cin>>n>>m;
  Splay tree;
  for(int i =0;i< n;++i) {
    int target = rand() & 0xff;
    tree.Add(target);
  }
  tree.print();
  int rm;
  for(int i =0;i<m;++i){
    cerr<<"remove val: ";
    cin>>rm;
    tree.Remove(rm);
    cout<<"deepth "<<tree.Deepth()<<' ';
    tree.print();
  }

}

signed main(){
  cin.tie(0)->sync_with_stdio(false);
  test();
}