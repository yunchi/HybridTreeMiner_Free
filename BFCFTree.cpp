/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Class: BFCFTree

	Description: Used to store one tree in BFCF
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFreeTree.h"
#include "BFCFTree.h"

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that the number of vertices v and the tid t are given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree(short v, long t) :
  VFreeTree(v, t), level(v+1,0), automorphism(v+1,0)
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that only tid t is given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree(long t) :
  VFreeTree(t)
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that nothing is given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree() : 
  VFreeTree()
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: copy constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree(const BFCFTree& rhs)
  : VFreeTree(rhs)
{
	level = rhs.level;
	automorphism = rhs.automorphism;
	canonicalString = rhs.canonicalString;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: copy from the parent in the enumeration tree, plus
	a new leg
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree(const BFCFTree& parent, 
				   short newEdgeLabel, short newVertexLabel, short position)
{
	/******************************************************************
	idea: copy the tree structure of the parent, plus one new leg
	Note: use deep copy and preserve the original order of link list
	at the end, recompute the BFCS and automorphisms
	******************************************************************/
	vCount = parent.vCount + 1;
	tid = parent.tid;
	adj.resize(vCount + 1);
	TnodeLink t1, t2, t3;
	for ( short i = 1; i <= vCount - 1; i++ ) //copy the parent part here
	{
		t1 = parent.adj[i];
		if ( t1 == 0 ) //unlike to happen for a tree
		{
			adj[i] = 0;	
			continue;
		}
		else
		{
			t2 = new Tnode(*t1);
			adj[i] = t2;
			while ( t1->next != 0 )
			{
				t1 = t1->next;
				t3 = new Tnode(*t1);
				t2->next = t3;
				t2 = t3;
			}
		}
	}
	vertexLabel = parent.vertexLabel;
	vertexLabel.push_back(newVertexLabel);
	degree = parent.degree;
	degree.push_back(0);
	level = parent.level;
	level.push_back(level[position]+1);
	insertEdge(Edge(position,vCount,newEdgeLabel));
	automorphism.resize(vCount+1);

	computeBFCS();
	computeAutomorphism();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: construct from a canonical string
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::BFCFTree(const vector<short>& cString) 
{
	tid = 0; //there is no tid information in this case
	short idx = 0;
	short vertexIndex;
	short parentIndex;
	
	vCount = cString[0];
	adj.resize(vCount+1, (TnodeLink) 0);
	vertexLabel.resize(vCount+1);
	degree.resize(vCount+1,0);
	
	level.resize(vCount+1);
	automorphism.resize(vCount+1);

	//for (short i = 0; i <= vCount; i++)
	//{
	//	adj[i] = 0;
	//	degree[i] = 0;
	//}

	vertexLabel[1] = cString[2];
	vertexIndex = 2;
	parentIndex = 1;
	idx = 4;

	if ( vCount == 1 ) 
	{
		level[1] = 1;
		automorphism[1] = 1;
		return;
	}

	while ( cString[idx] != ENDOFTREETAG )
	{
		if ( cString[idx] == ENDTAG )
		{
			parentIndex++;
			idx++;
			continue;
		}
		else
		{
			vertexLabel[vertexIndex] = cString[idx+1];
			insertEdge(Edge(vertexIndex,parentIndex,cString[idx]));
			vertexIndex++;
			idx+=2;
		}
	}

	computeBFCS();
	computeAutomorphism();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	destructor

	Decription: defaultly call the destructor of VFreeTree
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree::~BFCFTree()
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator=()

	Decription: assignment copy, same as copy constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BFCFTree& BFCFTree::operator=(const BFCFTree& rhs)
{
	VFreeTree::operator =((VFreeTree)rhs); //is this the correct way?
	level = rhs.level;
	automorphism = rhs.automorphism;
	canonicalString = rhs.canonicalString;	
	return *this;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	computeBFCS()

	Decription: compute the canonicalString of the BFCF tree, assuming
	that the tree has been normalized.
	Note: vector level is a byproduct of this step
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void BFCFTree::computeBFCS()
{
	short i;
	canonicalString.clear();
	short tempCount = vCount;
	short currentLevel = 1; //the level of the root
	queue<short> tempQ;
	stack<short> tempK;
	TnodeLink t;
	canonicalString.push_back(vCount);
	canonicalString.push_back(0); //the edgeToParent of the root
	canonicalString.push_back(vertexLabel[1]);
	canonicalString.push_back(ENDTAG);

	level[1] = currentLevel;
	tempQ.push(1);
	tempQ.push(-1);
	tempCount--;
	while ( !tempQ.empty() && tempCount > 0 )
	{
		i = tempQ.front();
		tempQ.pop();
		if ( i == -1 )
		{
			currentLevel++;
			tempQ.push(-1);
			continue;
		}
		t = adj[i];
		if ( i == 1 )
		{
			while ( t != 0 )
			{
				tempK.push( t->v );
				tempK.push( t->eLabel );
				t = t->next;
			}
		}
		else
		{
			while ( t->next != 0 ) //other than the parent
			{
				tempK.push( t->v );
				tempK.push( t->eLabel );
				t = t->next;
			}
		}

		while ( !tempK.empty() )
		{
			canonicalString.push_back(tempK.top());
			tempK.pop();
			canonicalString.push_back(vertexLabel[tempK.top()]);
			tempQ.push(tempK.top());
			level[tempK.top()] = currentLevel + 1;
			tempK.pop();
			tempCount--;
		}
		if ( tempCount > 0 )
			canonicalString.push_back(ENDTAG);
	}
	canonicalString.push_back(ENDOFTREETAG);

	////test
	//cout << "canonical string is : " << endl;
	//for ( i = 0; i < canonicalString.size(); i++ )
	//	cout << canonicalString[i] << ' ';
	//cout << endl;

	//cout << "level : " << endl;
	//for ( i = 1; i <= vCount ; i++ )
	//	cout << level[i] << ' ';
	//cout << endl;

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	computeAutomorphism()

	Decription: compute the automorphisms of the BFCF tree, assuming
	that the tree has been normalized, and the computeBFCF() has be called
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void BFCFTree::computeAutomorphism()
{
	vector<short> rank(vCount+1);
	vector<short> order(vCount+1);
	vector<short> parent(vCount+1);
	vector< vector<short> > codeString(vCount+1);

	TnodeLink t;
	short head, tail, i, j, k;
	
	for ( i = 0; i <= vCount; i++ ) order[i] = i; //initialize vector order

	//stage 1: upward stage
	tail = vCount;
	while ( level[tail] != 1 ) //not the root
	{
		head = tail;
		while ( level[head-1] == level[head] ) head--;
		
		//stage1_1: construct codeString for current level
		for ( i = head; i <= tail; i++ )
		{
			codeString[i].push_back(0); //temporary edgeToParent label
			codeString[i].push_back(vertexLabel[i]);
			t = adj[i];
			while ( t->next != 0 )
			{
				codeString[i].push_back(rank[t->v]);
				t = t->next;
			}
			codeString[i].push_back(ENDTAG);
			parent[i] = t->v;
			codeString[i][0] = t->eLabel; //put in correct edgeToParent label
		}

		//stage1_2: sort all vertices in current level to get order
		for ( i = head; i < tail; i++ )
		{
			for ( j = tail; j > head; j-- )
			{
				if ( codeString[order[j - 1]] > codeString[order[j]] )
				{
					k = order[j-1];
					order[j-1] = order[j];
					order[j] = k;
				}
			}
		}

		//stage1_3: scan current level to get rank
		k = 1; //current rank
		rank[order[head]] = k;
		for ( i = head + 1; i <= tail; i++ )
		{
			if ( codeString[order[i]] > codeString[order[i-1]] ) k++;
			rank[order[i]] = k;
		}
		tail = head - 1;
	}

	//stage 2: downward stage, compute automorphisms
	automorphism[1] = 1;
	head = 2;
	while ( head <= vCount )
	{
		tail = head;
		while ( (tail <= vCount) && (level[tail] == level[head]) ) tail++;
		tail--;
		//stage2_1: bubblesort again, refine the order, 
		//          taking automorphisms of parents into consideration
		for ( i = head; i < tail; i++ )
		{
			for ( j = tail; j > head; j-- )
			{
				if ( (rank[order[j - 1]] == rank[order[j]]) &&
					 (automorphism[parent[order[j-1]]] > automorphism[parent[order[j]]]) )
				{
					k = order[j-1];
					order[j-1] = order[j];
					order[j] = k;
				}
			}
		}

		//stage2_2: compute automorphims of current level 
		k = order[head]; //current automorphism
		automorphism[order[head]] = k;
		for ( i = head + 1; i <= tail; i++ )
		{
			if ( !((rank[order[i - 1]] == rank[order[i]]) &&
					 (automorphism[parent[order[i-1]]] == automorphism[parent[order[i]]])) )
				k = order[i];
			automorphism[order[i]] = k;
		}

		head = tail + 1;
	}

	////test
	//cout << "vector rank: " << endl;
	//for ( i = 1; i <= vCount; i++ ) cout << rank[i] << ' ';
	//cout << endl;

	//cout << "vector order: " << endl;
	//for ( i = 1; i <= vCount; i++ ) cout << order[i] << ' ';
	//cout << endl;

	//cout << "vector parent: " << endl;
	//for ( i = 1; i <= vCount; i++ ) cout << parent[i] << ' ';
	//cout << endl;

	//cout << "codeString: " << endl;
	//for ( i = 1; i <= vCount; i++ )
	//{
	//	for ( j = 0; j < codeString[i].size(); j++ )
	//		cout << codeString[i][j] << ' ' ;
	//	cout << endl;
	//}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	isBFCF()

	Decription: check if the current supplementary BFCF is really BFCF
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool BFCFTree::isBFCF()
{
	short child1, child2, i, j;
	TnodeLink t, t1;

	if ( vCount == 1 ) return true; //for single vertex, it is BFCF

	/******************************************************************
	step1: check if centered tree or simple bicentered tree whose BFCF
	can be resolved by comparing two bicenters
	******************************************************************/	
	child1 = vCount;
	while ( level[child1 - 1] == level[vCount] ) child1--;

	while ( level[child1] != 2 ) //search ancestor until the child of the root
	{
		t = adj[child1];
		while ( t->next != 0 ) t = t->next; //find the parent of child1
		child1 = t->v;
	}

	child2 = vCount;
	while ( level[child2] != 2 ) //search ancestor until the child of the root
	{
		t = adj[child2];
		while ( t->next != 0 ) t = t->next; //find the parent of child1
		child2 = t->v;
	}

	if ( child1 != child2 ) return true; //centered tree
	else if ( vertexLabel[child1] > vertexLabel[1] ) return true;
	else if ( vertexLabel[child1] < vertexLabel[1] ) return false;
	else if ( vCount == 2 ) return true;
	else
	{
	/******************************************************************
	step2: if cannot be solved easily, compute BFCF of the other center
	******************************************************************/
		vector<short> edgeToParent(vCount+1);
		vector< queue<short> > encodings(degree[child1]);
		queue<short> tempQ;
		stack<short> tempK;

		encodings[0].push(1);
		encodings[0].push(ENDTAG);
		t = adj[1];
		short tempCount = 0;
		while ( t != 0 )
		{
			if ( t->v == child1 )
				edgeToParent[1] = t->eLabel;
			else
			{
				tempK.push(t->v);
				edgeToParent[t->v] = t->eLabel;
			}
			t = t->next;
		}
		while ( ! tempK.empty() )
		{
			tempQ.push(tempK.top());
			tempK.pop();
			tempCount++;
		}
		tempQ.push(ENDTAG);

		while ( tempCount > 0 )
		{
			i = tempQ.front();
			tempQ.pop();
			if ( i == ENDTAG )
			{
				encodings[0].push(ENDTAG);
				continue;
			}
			else
			{
				tempCount--;
				encodings[0].push(i);
				t = adj[i];
				while ( t->next != 0 )
				{
					tempK.push(t->v);
					edgeToParent[t->v] = t->eLabel;
					t = t->next;
				}
				while ( ! tempK.empty() )
				{
					tempQ.push(tempK.top());
					tempK.pop();
					tempCount++;
				}
				tempQ.push(ENDTAG);
			}
		}
		encodings[0].push(ENDOFTREETAG);

		j = degree[child1] - 1;
		t1 = adj[child1];
		while ( t1->next != 0 )
		{
			while ( !tempQ.empty() ) tempQ.pop(); //clear the queue
			tempQ.push(t1->v);
			tempQ.push(ENDTAG);
			edgeToParent[t1->v] = t1->eLabel;
			tempCount = 1;

			while ( tempCount > 0 )
			{
				i = tempQ.front();
				tempQ.pop();
				if ( i == ENDTAG )
				{
					encodings[j].push(ENDTAG);
					continue;
				}
				else
				{
					tempCount--;
					encodings[j].push(i);
					t = adj[i];
					while ( t->next != 0 )
					{
						tempK.push(t->v);
						edgeToParent[t->v] = t->eLabel;
						t = t->next;
					}
					while ( ! tempK.empty() )
					{
						tempQ.push(tempK.top());
						tempK.pop();
						tempCount++;
					}
					tempQ.push(ENDTAG);
				}
			}
			encodings[j].push(ENDOFTREETAG);

			t1 = t1->next;
			j--;
		}

		vector< vector<short> > subStrings(degree[child1]);
		for ( i = 0; i < degree[child1]; i++ )
		{
			short k = encodings[i].front();
			encodings[i].pop();
			while ( k != ENDOFTREETAG )
			{
				if ( k != ENDTAG )
				{
					subStrings[i].push_back(edgeToParent[k]);
					subStrings[i].push_back(vertexLabel[k]);
				}
				else
					subStrings[i].push_back(k);
				encodings[i].push(k);
				k = encodings[i].front();
				encodings[i].pop();
			}
			encodings[i].push(k);
		}

		vector<short> order(degree[child1]);
		for ( i = 0; i < degree[child1]; i++ ) order[i] = i;
		i = 0;
		while ( i < degree[child1] - 1 )
		{
			if ( subStrings[order[i]] > subStrings[order[i+1]] )
			{
				short k = order[i];
				order[i] = order[i+1];
				order[i+1] = k;
				i++;
			}
			else
				break;
		}

		vector<short> numberOfVertices(degree[child1],1);
		vector<short> string2;
		string2.push_back(vCount);
		string2.push_back(0);
		string2.push_back(vertexLabel[child1]);
		string2.push_back(ENDTAG);
		i = 0;
		tempCount = 1;
		for ( i = 0; i < degree[child1]; i++ ) //put the roots of all
			//subtrees into the other BFCS
		{
			string2.push_back(edgeToParent[encodings[order[i]].front()]);
			string2.push_back(vertexLabel[encodings[order[i]].front()]);
			encodings[order[i]].pop();
			tempCount++;
			j = encodings[order[i]].front();
			if ( j == ENDTAG )
				encodings[order[i]].pop(); //$
		}
		if ( tempCount < vCount ) string2.push_back(ENDTAG);
		while ( tempCount < vCount )
		{
			for ( i = 0; i < degree[child1]; i++ ) 
			{
				short m = 0;
				while ( numberOfVertices[order[i]] > 0 )
				{
					j = encodings[order[i]].front();
					if ( j == ENDTAG )
					{
						encodings[order[i]].pop();
						string2.push_back(ENDTAG);
						numberOfVertices[order[i]]--;
						continue;
					}
					else if ( j == ENDOFTREETAG )
					{
						string2.push_back(ENDTAG);
						numberOfVertices[order[i]]--;
						continue;
					}
					else
					{
						m++;
						string2.push_back(edgeToParent[j]);
						string2.push_back(vertexLabel[j]);
						encodings[order[i]].pop();
						tempCount++;
						if ( tempCount >= vCount ) break;
					}
				}
				numberOfVertices[order[i]] = m;
				if ( tempCount >= vCount ) break;
			}
		}

		string2.push_back(ENDOFTREETAG);


		////test
		//cout << "String2: " << endl;
		//for ( i = 0; i < string2.size(); i++ )
		//	cout << string2[i] << ' ';
		//cout << endl;
		//cout << "canonicalString: " << endl;
		//for ( i = 0; i < canonicalString.size(); i++ )
		//	cout << canonicalString[i] << ' ';
		//cout << endl;


		return (canonicalString <= string2);

		////test
		//for ( i = 0; i < degree[child1]; i++ )
		//{
		//	for ( j = 0; j < encodings[i].size(); j++ )
		//	{
		//		cout << encodings[i][j] << ' ';
		//	}
		//	cout << endl;
		//}

	}//end of else above step2
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	possibleLegs()

	Decription: for a location i, compute the range of the possible legs
	Note: (0,0) means no restriction, (MAX_EDGE+1,MAX_VERTEX+1) means impossible
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void BFCFTree::possibleLegs(const short i, short& minEdge, short& minVertex )
{
	if ( automorphism[i] != i ) //no leg possible
	{
		minEdge = MAX_EDGE + 1;
		minVertex = MAX_VERTEX + 1;
		return;
	}

	minEdge = MIN_EDGE;
	minVertex = MIN_VERTEX;
	//first, check the position itself
	if ( (level[i] != 1 && degree[i] != 1) ||
			(level[i] == 1 && degree[i] != 0) ) 
	//degree[i] is 1 and i is not the root, it's a leaf, no restriction at this level
	{
		minEdge = adj[i]->eLabel; //the last leg
		minVertex = vertexLabel[adj[i]->v]; //the last leg
	}
	
	//second, check all above ancestors
	short j = i;
	short p, q;
	bool possible = true;
	while ( possible && level[j] != 1 )
	{
		//find the parent of j
		TnodeLink t = adj[j];
		while ( t->next != 0 ) t = t->next;
		p = t->v; //p is the parent of j;
		if ( level[p] != 1 && degree[p] <= 2 ) 
				//if p is not the root and j has no sibling, nothing to do
		{
			j = p;
			continue;
		}
		else if ( level[p] == 1 && degree[p] <= 1 )
			//if p is the root and j has no sibling, nothing to do
		{
			j = p;
			continue;
		}

		//find the sibling to the left of j
		t = adj[p];
		while ( t->v != j ) t = t->next; //find the correct sibling
		
		//now t->next points to the left sibling
		if ( (level[p] == 1 && t->next == 0) || 
				(level[p] != 1 && t->next->next == 0) ) //if j is the left most sibling
		{
			j = p;
			continue;
		}

		//now t points to j, t->next points to the left sibling of j
		if ( t->eLabel != t->next->eLabel || 
				vertexLabel[t->next->v] != vertexLabel[j] )
		{
			j = p;
			continue;
		}

		//find the restriction on the new leg by current sibling
		queue<short> vertexQ1, vertexQ2;
		stack<short> bufferK, labelK1, labelK2;
		vertexQ1.push(t->next->v); //compare two subtrees, then
		vertexQ2.push(j);          //neither cannot be a root
		short tempE1, tempE2, tempV1, tempV2;

		vertexQ1.push(ENDTAG);
		vertexQ2.push(ENDTAG);

		while ( true )
		{
			tempV1 = vertexQ1.front();
			vertexQ1.pop();
			tempV2 = vertexQ2.front();
			vertexQ2.pop();

			if ( tempV2 == ENDTAG )
			{
				if ( tempV1 != ENDTAG )
				{
					j = p;
					break;
				}
				else
					continue;
			}

			t = adj[tempV1];
			while ( t->next != 0 )
			{
				bufferK.push(t->v);
				labelK1.push(vertexLabel[t->v]);
				labelK1.push(t->eLabel);
				t = t->next;
			}
			while ( !bufferK.empty() )
			{
				vertexQ1.push(bufferK.top());
				bufferK.pop();
			}
			vertexQ1.push(ENDTAG);

			t = adj[tempV2];
			while ( t->next != 0 )
			{
				bufferK.push(t->v);
				labelK2.push(vertexLabel[t->v]);
				labelK2.push(t->eLabel);
				t = t->next;
			}
			while ( !bufferK.empty() )
			{
				vertexQ2.push(bufferK.top());
				bufferK.pop();
			}
			vertexQ2.push(ENDTAG);

			if ( tempV2 != i ) //if not reach the position for new leg yet
								//then the two stack either the same (then
								//continue), or stack1 is smaller, then no
								//need to continue
			{
				bool result = false;
				while ( !labelK1.empty() ) //if labelK1 is empty, so is labelK2
				{
					if ( labelK2.empty() )
					{
						result = true;
						break;
					}
					if ( labelK1.top() < labelK2.top() ) //if the edges resolve 
						//the order among the two subtrees...
						//note: it's impossible that labelK1.top > labelK2.top
					{
						result = true;
						break;
					}
					labelK1.pop();
					labelK2.pop();
					if ( labelK1.top() < labelK2.top() ) //if the vertices resolve
						//the order among the two subtrees...
					{
						result = true;
						break;
					}
					labelK1.pop();
					labelK2.pop();
				}
				if ( result )
				{
					j = p;
					break; //break the while(true) loop
				}
			}
			else //if now we are comparing something to the new position
			{
				if ( labelK1.empty() ) //no child, cannot add leg at all
				{
					possible = false;
					break;
				}
				else
				{
					while ( !labelK1.empty() )
					{
						tempE1 = labelK1.top();
						labelK1.pop();
						tempV1 = labelK1.top();
						labelK1.pop();
						if ( labelK2.empty() )
						{
							if ( tempE1 > minEdge ||
								( tempE1 == minEdge && tempV1 > minVertex) )
							{
								minEdge = tempE1;
								minVertex = tempV1;
							}
							break;
						}
						else
						{
							tempE2 = labelK2.top();
							labelK2.pop();
							tempV2 = labelK2.top();
							labelK2.pop();
							if ( tempE1 != tempE2 || tempV1 != tempV2 )
								break;
						}
					}
					j = p;
					break;//break the while(true) loop
				}
			}//end of if()else()
		}//end of while(true)
	}//end of while(possible...)

	if ( !possible )
	{
		minEdge = MAX_EDGE + 1;
		minVertex = MAX_VERTEX + 1;
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	extensionExplore()

	Decription: find all possible children in the enumeration tree that
	are frequent. Using extension operation only.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void BFCFTree::extensionExplore( vector<long>&frequency,
						ostream & outFile,
						const vector<ptrVFreeTree>& database,
					    const vector<Occurrence> & occList,
					   	const vector< vector<short> > & frequent2Tree,
						const vector<long> & headIndex,
						const long & threshold )
{
	short firstPos, lastPos, child1, child2;
	TnodeLink t;

	/******************************************************************
	step0: if this is a BFCF, output it
	******************************************************************/	
	if ( isBFCF() )
	{
		frequency[vCount]++;
		//if ( vCount == 5 )
		//{
		//	result1.push_back(canonicalString);
		//	outFile << *this << endl; //for testing, print out this tree to be explored
		//}
	}

	/******************************************************************
	step1: find the locations where a new leg can grow
	******************************************************************/	
	firstPos = adj[vCount]->v; //the parent of the last leg
	lastPos = firstPos;
	while ( lastPos < vCount && level[firstPos] == level[lastPos+1] ) lastPos++;
	if ( lastPos + 1 < vCount ) //if tempPos==vCount, single leg, skip
	{
		child1 = lastPos + 1;
		while ( level[child1] != 2 ) //search ancestor until the child of the root
		{
			t = adj[child1];
			while ( t->next != 0 ) t = t->next;
			child1 = t->v;
		}

		child2 = vCount;
		while ( level[child2] != 2 ) //search ancestor until the child of the root
		{
			t = adj[child2];
			while ( t->next != 0 ) t = t->next;
			child2 = t->v;
		}

		if ( child2 != child1 ) 
			lastPos = vCount;
	}

	/******************************************************************
	step2: main loop, for each position, explore
	******************************************************************/	
	for ( short i = firstPos; i <= lastPos; i++ )
	{
		//step2_1: get the range of valid legs
		short minEdge;
		short minVertex;
		possibleLegs(i, minEdge, minVertex);

		if ( minEdge > MAX_EDGE ) continue; //continue the for loop

		if ( headIndex[vertexLabel[i] - MIN_VERTEX] == 0 ) continue;

		//step2_2: get the possible frequent legs
		vector<bool> isFrequent( (MAX_EDGE - MIN_EDGE + 1) 
								*(MAX_VERTEX - MIN_VERTEX + 1), false);
		for (short j = headIndex[vertexLabel[i] - MIN_VERTEX]; 
			  (j < frequent2Tree.size() && frequent2Tree[j][0] == vertexLabel[i]); j++ )
			isFrequent[( frequent2Tree[j][1] - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 )
				+ ( frequent2Tree[j][2] - MIN_VERTEX )] = true;


		//step2_3: explore each potential leg
		set<short> neighbors;
		set<short>::iterator pos;
		Occurrence tempOcc;
		vector<SupportNode> potential((MAX_EDGE - MIN_EDGE + 1) 
									  *(MAX_VERTEX - MIN_VERTEX + 1));

		for ( long s = 0; s < occList.size(); s++ )
		{
			neighbors.clear();
			t = adj[i];
			while ( t != 0 ) //insert index of all neighbors of the position i
			{
				neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
				t = t->next;
			}
			t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
			while ( t != 0 )
			{
				pos = neighbors.find( t->v );
				if ( pos == neighbors.end() ) //if the vertex hasn't been used
				{
					short tempE = t->eLabel;
					short tempV = database[occList[s].tid]->vertexLabel[t->v];
					short location = ( tempE - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 ) 
						+ ( tempV - MIN_VERTEX ); 
					if ( ((tempE > minEdge) || (tempE == minEdge && tempV >= minVertex)) &&
						isFrequent[location] ) //if the leg is potentially frequent
					{
						tempOcc = occList[s];
						tempOcc.nodeIndex.push_back(t->v);
						potential[location].occList.push_back(tempOcc);
						if ( tempOcc.tid != potential[location].lastTid )
						{
							potential[location].lastTid = tempOcc.tid;
							potential[location].support++;
						}
					}
				}
				t = t->next;
			}//end of while ( t != 0 )
		}//end of for ( s = 0; ...)

		for ( long s = 0; s < potential.size(); s++ )
		{
			if ( potential[s].support >= threshold )
			{
				short tempE = MIN_EDGE + (short)(floor(s/(MAX_VERTEX - MIN_VERTEX + 1)));
				short tempV = MIN_VERTEX + (s % (MAX_VERTEX - MIN_VERTEX + 1));
				BFCFTree *pbfcf = new BFCFTree(*this,tempE,tempV,i);
				pbfcf->extensionExplore(frequency, outFile, database,potential[s].occList,
							   frequent2Tree,headIndex,threshold);
				delete pbfcf;
			}
		}

		////test
		//cout << "leg position is: " << i << " vertex label is: " << vertexLabel[i] << endl;
		//cout << "min edge and min vertex are: " << minEdge << ' ' << minVertex << endl;
		//for ( j = 0; j < isFrequent.size(); j++ )
		//	cout << isFrequent[j] << ' ';
		//cout << endl;
		//cout << endl;

	}//end of for(short i = ...)
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator<()

	Decription: compare two trees in BFCF
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool BFCFTree::operator<(const BFCFTree& rhs) const
{
	return canonicalString < rhs.canonicalString;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator>>()

	Decription: read in BFCF trees
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
istream& operator>>(istream& in, BFCFTree& rhs)
{
  long t;	//tid
  short v;	//number of vertices
  short p1, p2;
  short vLabel, eLabel;

  in >> t;
  in >> v;

  rhs.adj.resize(v+1);
  rhs.vertexLabel.resize(v+1);
  rhs.degree.resize(v+1);

  rhs.level.resize(v+1);
  rhs.automorphism.resize(v+1);

  for (short i = 0; i <= v; i++)
  {
	rhs.adj[i] = 0;
	rhs.degree[i] = 0;
  }

  rhs.tid = t;
  rhs.vCount = v;
  rhs.vertexLabel[0] = 0;	//null
  for (short i = 1; i <= v; i++)
  {
	in >> vLabel;
	rhs.vertexLabel[i] = vLabel;
  }
	  
  for (short i = 1; i < v; i++) //the number of edges is v-1
  {
	in >> eLabel;
	in >> p1;
	in >> p2;
	rhs.insertEdge(Edge(p1,p2,eLabel));
  }
return in;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator<<()

	Decription: write out BFCF trees
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
ostream& operator<<(ostream& out, const BFCFTree& rhs)
{
  out << "Tid: " << rhs.tid << endl;
  out << "Vertices Number: " << rhs.vCount << endl;
  for ( short i = 1; i <= rhs.vCount; i++ )
    {
		out << rhs.vertexLabel[i] << ' ' 
		<< rhs.degree[i] << ':';
		TnodeLink n = rhs.adj[i];
		while ( n != 0 ) 
		{
			out << "--" << n->v << '_' << n->eLabel << ' ';
			n = n->next;
		}
		out << endl;
    }
  out << "automorphism: ";
  for ( short i = 1; i <= rhs.vCount; i++ ) out << rhs.automorphism[i] << ' ';
  out << endl;
  out << "canonical string: ";
  for ( short i = 0; i < rhs.canonicalString.size(); i++ ) out << rhs.canonicalString[i] << ' ';
  out << endl;
  return out;
}  


