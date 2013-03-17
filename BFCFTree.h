/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Header: BFCFTree.h
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFreeTree.h"

using namespace std;

#ifndef BFCFTree_H
#define BFCFTree_H

struct Bnode { //used in the function of normalization
	short bParent;
	short bEdgeToParent;
	short bVertexLabel;
	short bFirstChild;
};

class BFCFTree : public VFreeTree
{
public:
	BFCFTree(short v, long t);
	BFCFTree(long t);
	BFCFTree();
	BFCFTree(const BFCFTree& rhs);
	BFCFTree(const BFCFTree& parent, 
				   short newEdgeLabel, short newVertexLabel, short position);

	BFCFTree(const vector<short>& cString);
	~BFCFTree();

	BFCFTree& operator=(const BFCFTree& rhs);

	void computeBFCS();
	void computeAutomorphism();
	bool isBFCF();
	void possibleLegs(const short i, short& minEdge, short& minVertex );
	void extensionExplore( vector<long>& frequency,
				  ostream& outFile,
				  const vector<ptrVFreeTree>& database,
				  const vector<Occurrence> & occList,
				  const vector< vector<short> > & frequent2Tree,
				  const vector<long> & headIndex,
				  const long & threshold);
//	void explore();

	bool operator<(const BFCFTree& rhs) const;

	friend istream& operator>>(istream&, BFCFTree&);
	friend ostream& operator<<(ostream&, const BFCFTree&);

	friend class VFrequentTreeFamily;

public:
	vector <short> canonicalString; //the canonical string of this tree

protected:
	vector <short> level; //the levels of vertices
	vector <short> automorphism; //the list of automorphisms
};

#endif //BFCFTree_H
