/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Header: VFrequentTreeList.h
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFreeTree.h"
#include "BFCFTree.h"
#include "VFrequentTreeFamily.h"

using namespace std;

#ifndef VFrequentTreeList_H
#define VFrequentTreeList_H

class VFrequentTreeList
{
public:
	long returnSize(); //return number of elements in the map

	//this set of functions are defined for extention-only exploration
	void populate2(vector<ptrVFreeTree>& database);
	void finalize2(const long threshold); //remove elements whose support is less than a threshold
	void extensionExploreList(const vector<ptrVFreeTree>& database, 
							  ostream& outFile,
							  vector<long> & frequency,
							  long & threshold); //starting from list, find all frequent trees

	//this set of functions are defined for hybrid exploration
	void populate1(vector<ptrVFreeTree>& database);
	void finalize1(const long threshold);
	void hybridExploreList(const vector<ptrVFreeTree>& database, 
							  ostream& outFile,
							  vector<long> & frequency,
							  long & threshold); //starting from list, find all frequent trees

	void test();

	friend istream& operator>>(istream&, VFrequentTreeList&);
	friend ostream& operator<<(ostream&, const VFrequentTreeList&);

public:
	map<vector<short>, SupportNode> frequent2List;
	vector< vector<short> > frequent2Tree;
	map<short, SupportNode> frequent1List;
	map<vector<short>, LegSupportNode> frequent2Leg;
	vector<long> headIndex;
};

#endif //VFrequentTreeList_H
