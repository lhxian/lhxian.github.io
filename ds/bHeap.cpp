#include<cassert>
#include<cstdlib>
#include<ctime>
#include<iostream>
using namespace std;
struct Node;
using PNODE = Node*;
struct ListNode;
using PLIST =ListNode*;
using EleType = int;
struct Node{
  EleType val;
  int rank;
  PLIST pson;

  Node() = delete;
  Node(EleType target):val(target),rank(1),pson(nullptr){}
  // ~Node();
};

struct ListNode{
  PNODE pnode;
  PLIST nex;
  // ListNode();
  // ~ListNode();
};
PNODE mergetwo(PNODE p1,PNODE p2){
  if(p1->val < p2->val) swap(p1,p2);
  assert(p1->rank == p2->rank);
  PLIST new_head = new ListNode;
  new_head->pnode = p2;
  new_head->nex = p1->pson;
  p1->pson = new_head;
  ++p1->rank;
  return p1;
}
PNODE split(PNODE p){
  if(!p || p->rank == 1) return nullptr;
  --p->rank;
  PLIST tem = p->pson;
  PNODE res = tem->pnode;
  p->pson = tem->nex;
  delete tem;
  return res;
}
bool check(PLIST head){
  PLIST pcur= head;
  int pre = 0;
  while(pcur){
    if(pcur->pnode->rank < pre) return false;
    pcur = pcur->nex;
  }
  return true; 
}
void ShowNode(PNODE pnode){
  if(!pnode) return;
  cout<<pnode->val<<'('<<pnode->rank<<')'<<' ';
  PLIST pcur = pnode->pson;
  while(pcur){
    ShowNode(pcur->pnode);
    pcur = pcur->nex;
  }
}
void ShowList(PLIST head ){
  while(head){
    ShowNode(head->pnode);
    head = head->nex;
    cout<<'\n';
  }
}
PLIST merge(PLIST l1,PLIST l2){
  assert(check(l1));
  assert(check(l2));
  PLIST p1 = l1, p2 = l2,pnex =nullptr,phead = nullptr;
  while(p1 && p2){
    if(p1->pnode->rank <= p2->pnode->rank){
      pnex = p1->nex;
      p1->nex = phead;
      phead=p1;
      p1 = pnex;
    }else{
      pnex = p2->nex;
      p2->nex = phead;
      phead = p2;
      p2 = pnex;
    }
  }
  if(p2) p1 = p2;
  while(p1){
    pnex = p1->nex;
    p1->nex = phead;
    phead =p1;
    p1= pnex;
  }
  PLIST pstack = nullptr;
  PNODE targetnode = nullptr;
  while(phead){
    pnex = phead->nex;
    targetnode = phead->pnode;
    while(pstack && pstack->pnode->rank == targetnode->rank){
      PNODE othernode = pstack->pnode;
      PLIST removenode = pstack;
      pstack = pstack->nex;
      delete removenode;
      // merge the two node;
      targetnode = mergetwo(targetnode,othernode);
    }
    phead->pnode = targetnode;
    
    phead->nex = pstack;
    pstack = phead;
    phead = pnex;
  }
  return pstack;
}
PLIST reverse(PLIST head){
  if(!head) return nullptr;
  PLIST res = nullptr,pnex = nullptr;
  while(head){
    pnex=head->nex;
    head->nex = res;
    res = head;
    head=pnex;
  }
  return res;
}
PLIST extract(PNODE tree){
  if(!tree) return nullptr;
  PLIST sons = tree->pson;
  delete tree;
  return reverse(sons);
}

PLIST pop(PLIST head){
  if(!head) return nullptr;
  PLIST res = nullptr;
  PLIST pcur = head,preremove = nullptr,pre = nullptr;
  EleType mmax = INT_MIN;
  while(pcur){
    if(pcur->pnode->val > mmax) {
      preremove = pre;
      mmax = pcur->pnode->val;
    }
    pre = pcur;
    pcur = pcur->nex;
  }
  PLIST removenode = nullptr;
  if(preremove){
    // remve the head;
    removenode = preremove->nex;
    assert(removenode);
    preremove->nex =removenode->nex;
    res = merge(extract(removenode->pnode),head);
  }else{
    removenode = head;
    res = extract(removenode->pnode);
  }
  delete removenode;
  return res;
}
// PLIST adjustlist(PLIST head){
//   if(!head) return nullptr;
//   head = reverse(head);
//   PLIST pstack = nullptr;

// }
bool CheckList(PLIST const head){
  PLIST pcur = head;
  int pre_rank = 0;
  while(pcur){
    if(pcur->pnode->rank <= pre_rank) return false;
    pcur = pcur->nex;
  }
  return true;
}
bool CheckNode(PNODE const pnode){
  if(!pnode) return true;
  int rk = pnode->rank-1;
  PLIST pcur = pnode->pson;
  while(pcur){
    if(rk != pcur->pnode->rank) return false;
    --rk;
    if(CheckNode(pcur->pnode) == false) return false;
    pcur = pcur->nex;
  }
  return rk == 0;
}
PLIST add(PLIST head,EleType target){
  assert(CheckList(head));
  PLIST cur;
  cur = new ListNode;
  PNODE node = new Node(target);
  cur->pnode= node;
  cur->nex = head;
  while(cur->nex){
    PLIST pnex = cur->nex;
    if(pnex->pnode->rank != cur->pnode->rank){
      break;
    }
    cur->nex = pnex->nex;
    cur->pnode = mergetwo(cur->pnode,pnex->pnode);
    delete pnex;
  }
  
  return cur; 
}

void test(){
  int n,m;
  cin>>n>>m;
  int target;
  PLIST tree = nullptr;
  for(int i =0;i<n;++i){
    target = rand()&0xff;
    tree = add(tree,target);
    cout<<target<<' ';
  }
  cout<<"\nshow\n";
  ShowList(tree);
  PLIST otherlist = nullptr;

  for(int i=0;i<m;++i){
    target = rand() &0xff;
    otherlist = add(otherlist,target);
  }
  cout<<"show otherlist\n";
  ShowList(otherlist);
  PLIST mergelist = merge(tree,otherlist);
  cout<<"merge list:\n";
  ShowList(mergelist);
  // PLIST pcur = tree;
  // while(pcur){
  //   cout<<pcur->pnode->val<<' ';
  //   assert(CheckNode(pcur->pnode));
  //   pcur = pcur->nex;
  // }
  int c;
  cin>>c;
  for(int i=0;i<c;++i){
    cin>>target;
    mergelist = pop(mergelist);
    cout<<"pop one\n";
    ShowList(mergelist);
  }

  cout<<endl;
}

int main(){
  srand(time(0));
  test();
}