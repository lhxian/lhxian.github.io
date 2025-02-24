// #include<utility>
#include<cassert>
#include<ctime>
#include<cstdlib>
#include<iostream>
using namespace std;
struct Node;
using PNODE =Node*;
struct Node{
  int val,prio;
  PNODE l,r;
  Node(int val):val(val),l(nullptr),r(nullptr),prio(rand() & 0xffff){}
};
void split(PNODE root,int val,PNODE* tree1,PNODE* tree2){
  PNODE cur = root;
  *tree1 = *tree2 = nullptr;
  while(cur){
    if(cur->val <= val){
      *tree1 = cur;
      tree1 = &cur->r;
      cur = cur->r;
      *tree1 = nullptr;
    }else{
      *tree2 = cur;
      tree2 = &cur->l;
      cur = cur->l;
      *tree2 = nullptr;
    }
  }
}
void merge(PNODE tree1,PNODE tree2,PNODE* root){
  while(tree1 && tree2){
    if(tree1->prio > tree2->prio){
      *root = tree1;
      root = &tree1->r;
      tree1 = tree1->r;
    }else{
      *root = tree2;
      root = &tree2->l;
      tree2 = tree2->l;
    }
  }
  *root = tree1 ? tree1 : tree2;
}
PNODE Add(PNODE root,int val){
  PNODE t1, t2,t3;
  split(root,val,&t2,&t3);

  split(t2,val-1,&t1,&t2);
  if(t2 == nullptr){
    t2 = new Node(val);
  }
  PNODE res = nullptr;
  merge(t1,t2,&res);
  merge(res,t3,&res);
  return res;
}
PNODE Remove(PNODE root,int val){
  PNODE t1,t2,t3;
  split(root,val,&t2,&t3);
  split(t2,val-1,&t1,&t2);
  if(t2){
    delete t2;
    t2 = nullptr;
  }
  PNODE res = nullptr;
  merge(t1,t2,&res);
  merge(res,t3,&res);
  return res;
}
void Rprint(PNODE node){
  if(!node) return;
  Rprint(node->l);
  cout<<node->val<<' ';
  Rprint(node->r);
}
void print(PNODE root){
  Rprint(root);
  cout<<endl;
}

void Check(PNODE root){
  if(!root) return;
  PNODE tem = nullptr;
  if(root->l){
    tem = root->l;
    assert(tem->val < root->val);
    assert(tem->prio <= root->prio);
  }
  if(root->r){
    tem = root->r;
    assert(tem->val > root->val);
    assert(tem->prio <= root->prio);
  }
}

void test(){
  cout<<"size of node "<<sizeof(Node)<<endl;
  int n,m;
  cin>>n>>m;
  PNODE root = nullptr;
  int target;
  for(int i =0;i< n;++i){
    target = rand() &0x7f;
    root = Add(root,target);
  }
  Check(root);
  print(root);
  for(int i =0;i<m;++i){
    cout<<"remove: ";
    cin>>target;
    root = Remove(root,target);
    print(root);
  }
}
int TreeDeepth(PNODE root){
  if(!root) return 0;
  int res = max(TreeDeepth(root->l),TreeDeepth(root->r)) + 1;
  return res;
}
void testDeepth(){
  int n;
  cin>>n;
  int target = 0;
  PNODE tree = nullptr;
  for(int i =0;i< n;++i){
    target = rand();
    tree = Add(tree,target);
  }
  Check(tree);
  cout<<"deepth "<<TreeDeepth(tree)<<endl;
}
signed main(){
  cin.tie(0)->sync_with_stdio(false);
  srand(clock());
  testDeepth();
}