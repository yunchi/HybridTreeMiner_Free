/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Header: VFrequentTreeFamily.h
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFreeTree.h"
#include "BFCFTree.h"

using namespace std;

#ifndef VFrequentTreeFamily_H
#define VFrequentTreeFamily_H

class VFrequentTreeFamily
{
public:

	VFrequentTreeFamily(const vector<short>& cString); //constructor with a BFCS as
	//parameter, which is passed to the BFCFTree--mother

	VFrequentTreeFamily(const BFCFTree& originalTree, Leg& newLeg); 
	//constructor with an old mother plus a leg
	void hybrideExplore( vector<long>& frequency,
				  ostream& outFile,
				  const vector<ptrVFreeTree>& database,
				  const vector< vector<short> > & frequent2Tree,
				  const vector<long> & headIndex,
				  const long & threshold,
				  const bool & isFirstFamily );
	void test();

	friend istream& operator>>(istream&, VFrequentTreeFamily&);
	friend ostream& operator<<(ostream&, const VFrequentTreeFamily&);

	friend class VFrequentTreeList;

protected:
	BFCFTree mother;
	vector<Occurrence> motherOccList;
	vector<Leg> legs;
};

#endif //VFrequentTreeFamily_H
