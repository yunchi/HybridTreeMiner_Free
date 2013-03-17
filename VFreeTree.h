/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Header: VFreeTree.h
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"

using namespace std;

#ifndef VFreeTree_H
#define VFreeTree_H

struct Tnode
{
  short v;		//the other vertex this link connects to
  short eLabel;	//edge label
  Tnode* next;	//head of the link list for adjacent vertices
  Tnode(short x, short e, Tnode* t) 
  { v = x; eLabel = e; next = t; }

  // shallow copy constructor
  Tnode(const Tnode& rhs) { v = rhs.v; eLabel = rhs.eLabel; next = 0; } 
  Tnode& operator=(const Tnode& rhs) 
  { v = rhs.v; eLabel = rhs.eLabel; next = 0; return *this; }
};

typedef Tnode *TnodeLink;

class VFreeTree
{
 public:
  VFreeTree(short v, long t);
  VFreeTree(long t);
  VFreeTree();
  VFreeTree(const VFreeTree& rhs); //copy constructor
  ~VFreeTree();

  VFreeTree& operator=(const VFreeTree& rhs);

  short vertexNumber() const;
  long returnTid() const;
  void changeVertexLabel(short idx, short vLabel);
  void insertEdge(Edge e);
  
  friend istream& operator>>(istream&, VFreeTree&);
  friend ostream& operator<<(ostream&, const VFreeTree&);

public:
  vector <TnodeLink> adj; //adjacency list (heads)
  short vCount; //number of vertices
  long tid;  //transaction ID
  vector <short> vertexLabel; //labels of vertices
  vector <short> degree; //degrees of vertices
};

#endif //VFreeTree_H
