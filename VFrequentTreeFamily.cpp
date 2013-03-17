/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Class: VFrequentTreeFamily

	Description: Used to store a frequent tree and all its extended legs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFrequentTreeFamily.h"

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that the BFCS for the mother is given as the
	only parameter
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VFrequentTreeFamily::VFrequentTreeFamily(const vector<short>& cString) : mother(cString)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: the BFCF of the old mother and a new leg are given 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VFrequentTreeFamily::VFrequentTreeFamily(const BFCFTree& originalTree, Leg& newLeg)
: mother(originalTree, newLeg.legEdgeLabel, newLeg.legVertexLabel, newLeg.connectTo)
{
}


void VFrequentTreeFamily::test()
{
	cout << "======mother: " << endl << mother << endl;
	cout << "======motherOccList: " << endl;
	for ( long s = 0; s < motherOccList.size(); s++ ) 
		cout << motherOccList[s] << ' ';
	cout << endl;
	cout << "======legs: " << endl;
	for ( long s = 0; s < legs.size(); s++ )
		cout << legs[s] << endl;
	cout << endl;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	hybridExplore()

	Decription: recursively explore a family, both by join and by extension
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void VFrequentTreeFamily::hybrideExplore( vector<long>& frequency,
				ostream& outFile,
				const vector<ptrVFreeTree>& database,
				const vector< vector<short> > & frequent2Tree,
				const vector<long> & headIndex,
				const long & threshold,
				const bool & isFirstFamily )
{
	/******************************************************************
	step0: if mother is a BFCF, output it
	******************************************************************/	
	if ( isFirstFamily && mother.isBFCF() )
	{
		frequency[mother.vertexNumber()]++;
		//if ( mother.vCount == 5 )
		//{
		//	result2.push_back(mother.canonicalString);
		//	outFile << mother << endl; //for testing, print out this tree to be explored
		//}
	}

	//if ( mother.vCount == 4 )
	//cout << "******************mother is: *******************" << endl << mother << endl;
	/******************************************************************
	step1: growing flatter, by join operation
	******************************************************************/	
	vector<short> minEdge(mother.vertexNumber() + 1, -1);
	vector<short> minVertex(mother.vertexNumber() + 1, -1);
	for ( long s = 0; s < legs.size(); s++ )
	{
		if ( minEdge[legs[s].connectTo] == -1 )
			mother.possibleLegs(legs[s].connectTo, 
				minEdge[legs[s].connectTo], minVertex[legs[s].connectTo]);

			//cout << "constrains: " << endl;
			//cout << "minEdge: " << minEdge[legs[s].connectTo] << endl;
			//cout << "minVertex: " << minVertex[legs[s].connectTo] << endl;;
			//cout << "legs[s]: " << endl;
			//cout << "legEdgeLabel: " << legs[s].legEdgeLabel << endl;
			//cout << "legVertexLabel: " << legs[s].legVertexLabel << endl;


		if ( minEdge[legs[s].connectTo] > MAX_EDGE ) continue; //no legs possible
		else if ( legs[s].legEdgeLabel < minEdge[legs[s].connectTo] || 
			( (legs[s].legEdgeLabel == minEdge[legs[s].connectTo]) 
			   && (legs[s].legVertexLabel < minVertex[legs[s].connectTo])) ) continue; 
		//this leg is not possible

		////for test
		//if ( ( find(potentialAutomorphism.begin(), potentialAutomorphism.end(),
		//	legs[s].connectTo) != potentialAutomorphism.end() ))
		//{
		//	cout << "find it!" << endl;
		//	cout << "auto vector: " << endl;
		//	for ( short b = 0; b < potentialAutomorphism.size(); b++ )
		//		cout << potentialAutomorphism[b] << ' ';
		//	cout << endl;
		//	cout << "mother: " << mother << endl;
		//	cout << "legs: " << endl;
		//	cout << "legEdgeLabel: " << legs[s].legEdgeLabel << endl;
		//	cout << "legVertexLabel: " << legs[s].legVertexLabel << endl;
		//	cout << "last leg: " << endl;
		//	cout << "edgeLabel: " << mother.adj[mother.vCount]->eLabel << endl;
		//	cout << "vertexLabel: " << mother.vertexLabel[mother.vCount] << endl;
		//}

		vector<long> legLastTid(legs.size() - s, -1);
		vector<long> legSupport(legs.size() - s, 0);
		vector<long> currentPosition(legs.size() - s, 0); //current
		//position of a leg occ list
		vector<Leg> possibleLegs(legs.size() - s);
		vector<long> head(legs.size() - s);
		vector<long> tail(legs.size() - s);

		//step1_1: shallow copy all possible legs, including s itself
		for ( long p = 0; p < possibleLegs.size(); p++ )
		{
			possibleLegs[p].connectTo = legs[s + p].connectTo;
			possibleLegs[p].legEdgeLabel = legs[s + p].legEdgeLabel;
			possibleLegs[p].legVertexLabel = legs[s + p].legVertexLabel;
		}

		//step1_2: construct a new family with the first leg added
		VFrequentTreeFamily newFamily(mother,legs[s]);
		
		//step1_3: main loop, traverse the leg occ list of the first
		//leg to create occurrence of the new mother, as well as the
		//new leg occ lists of the new family
		Occurrence oneOcc;
		head[0] = currentPosition[0]; //head[0] is about leg s itself
		while ( head[0] < legs[s].legOccList.size() )
		{
			for ( long p = 0; p < possibleLegs.size(); p++ )
			{
				while ( currentPosition[p] < legs[s+p].legOccList.size()
					&& legs[s+p].legOccList[currentPosition[p]].motherID
					< legs[s].legOccList[head[0]].motherID )
					currentPosition[p]++;
				if ( currentPosition[p] >= legs[s+p].legOccList.size() 
					|| legs[s+p].legOccList[currentPosition[p]].motherID
					> legs[s].legOccList[head[0]].motherID )
				{
					//bad case, make it impossible, i.e., head > tail
					head[p] = 1;
					tail[p] = 0;
				}
				else
				{
					head[p] = currentPosition[p];
					while ( currentPosition[p] < legs[s+p].legOccList.size()
						&& legs[s+p].legOccList[currentPosition[p]].motherID
						== legs[s].legOccList[head[0]].motherID )
						currentPosition[p]++;
					tail[p] = currentPosition[p] - 1;
				}
			}//end of for ( long p = 0...

			for ( long headIdx = head[0]; headIdx <= tail[0]; headIdx++ )
			{
				//add to the occurrence list of the new mother
				oneOcc = motherOccList[legs[s].legOccList[headIdx].motherID];
				oneOcc.nodeIndex.push_back(legs[s].legOccList[headIdx].myIndex);
				newFamily.motherOccList.push_back(oneOcc);

				//update all possible legs
				for ( long q = 0; q < possibleLegs.size(); q++ )
				{
					for ( long r = head[q]; r <= tail[q]; r++ )
					{
						//if the leg is different from the last leg
						if ( legs[s].legOccList[headIdx].myIndex !=
							legs[s+q].legOccList[r].myIndex )
						{
							possibleLegs[q].legOccList.push_back( LegOccurrence
								( newFamily.motherOccList.size() - 1, 
								legs[s+q].legOccList[r].myIndex ) );
							if ( legLastTid[q] != newFamily.motherOccList.back().tid )
							{
								legLastTid[q] = newFamily.motherOccList.back().tid;
								legSupport[q]++;
							}
						}
					}//end of for ( long r = head[q]...)
				}//end of for (long q = 0...)
			}//end of for (headIdx = head[0])
		head[0] = currentPosition[0];
		}//end of while (head[0] < ...)

		for ( long m = 0; m < possibleLegs.size(); m++ )
		{
			if ( legSupport[m] >= threshold )
			newFamily.legs.push_back(possibleLegs[m]);
		}

		newFamily.hybrideExplore( frequency, outFile, database,
			frequent2Tree, headIndex, threshold, true);
	}//end of for(long s = 0 ...)


	/******************************************************************
	step2: growing taller, by extension operation
	******************************************************************/	
	//step2_0: if the mother is a single node then skip, to avoid repeat
	if ( mother.vertexNumber() == 1 ) return;
	if ( isFirstFamily == false ) return;

	short firstPos, lastPos;
	//step2_1: find the location of the new legs can grow
	lastPos = mother.vertexNumber(); //the parent of the last leg
	firstPos = lastPos;
	while ( mother.level[firstPos - 1] == mother.level[lastPos] ) firstPos--;

	//step2_2: find if extension is possible, i.e., if firstPos and
	//lastPos share different children of the root
	short child1, child2;
	TnodeLink t;

	child1 = firstPos;
	while ( mother.level[child1] != 2 )
	{
		t = mother.adj[child1];
		while ( t->next != 0 ) t = t->next;
		child1 = t->v;
	}
	child2 = lastPos;
	while ( mother.level[child2] != 2 )
	{
		t = mother.adj[child2];
		while ( t->next != 0 ) t = t->next;
		child2 = t->v;
	}

	if ( child2 == child1 )
		return; //not possible to extend

	//step2_3: main loop, for each possible position, explore
	vector<Leg> validLegs;
	for ( short i = firstPos; i <= lastPos; i++ )
	{
		//if no leg possible at this position, skip
		if ( headIndex[mother.vertexLabel[i] - MIN_VERTEX] == 0 ) continue;

		//first get the possible frequent legs
		vector<bool> isFrequent( (MAX_EDGE - MIN_EDGE + 1) 
								*(MAX_VERTEX - MIN_VERTEX + 1), false);
		for ( short j = headIndex[mother.vertexLabel[i] - MIN_VERTEX]; 
			  (j < frequent2Tree.size() && frequent2Tree[j][0] == mother.vertexLabel[i]); j++ )
			isFrequent[( frequent2Tree[j][1] - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 )
				+ ( frequent2Tree[j][2] - MIN_VERTEX )] = true;

		short parentLocation,parentIndex;
		LegOccurrence legOcc;
		vector<LegSupportNode> potential((MAX_EDGE - MIN_EDGE + 1) 
							*(MAX_VERTEX - MIN_VERTEX + 1));
		t = mother.adj[i];
		while ( t->next != 0 ) t = t->next; //t points to the parent of the 
		//location of the new leg
		parentLocation = t->v;
		for ( long n = 0; n < motherOccList.size(); n++ )
		{
			parentIndex = motherOccList[n].nodeIndex[parentLocation - 1];
			t = database[motherOccList[n].tid]->adj[motherOccList[n].nodeIndex[i-1]];
			while ( t != 0 )
			{
				if ( t->v != parentIndex )
				{
					short tempE = t->eLabel;
					short tempV = database[motherOccList[n].tid]->vertexLabel[t->v];
					short location = ( tempE - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 ) 
						+ ( tempV - MIN_VERTEX );
					if ( isFrequent[location] )
					{
						legOcc.motherID = n;
						legOcc.myIndex = t->v;
						potential[location].thisLeg.legOccList.push_back(legOcc);
						if ( potential[location].lastTid !=
							 motherOccList[n].tid )
						{
							potential[location].lastTid = motherOccList[n].tid;
							potential[location].support++;
						}
					}
				}
				t = t->next;
			}
		}
		for ( short tempE = MIN_EDGE; tempE <= MAX_EDGE; tempE++ )
			for ( short tempV = MIN_VERTEX; tempV <= MAX_VERTEX; tempV++ )
			{
				short location = ( tempE - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 ) 
						+ ( tempV - MIN_VERTEX );
				if ( potential[location].support >= threshold )
				{
					potential[location].thisLeg.connectTo = i;
					potential[location].thisLeg.legEdgeLabel = tempE;
					potential[location].thisLeg.legVertexLabel = tempV;
					validLegs.push_back(potential[location].thisLeg);
				}
			}
	}//end of for(i = firstPos ... )

	if ( validLegs.size() > 0 )
	{
		//cout << "mother is: " << mother << endl;
		//cout << "validLegs size: " << validLegs.size() << endl;
		legs = validLegs;

			//if ( mother.vCount == 4 &&  mother.vertexLabel[1] == 11
			//	&& mother.vertexLabel[2] == 12
			//	&& mother.vertexLabel[3] == 12
			//	&& mother.vertexLabel[4] == 12 )
			//{
			//	cout << "mother: ";
			//	for ( short u = 0; u < mother.canonicalString.size(); u++ )
			//		cout << mother.canonicalString[u] << ' ';
			//	cout << endl;
			//	cout << "motherOccList: ";
			//	for ( short u = 0; u < motherOccList.size(); u++ )
			//		cout << motherOccList[u];
			//	cout << endl;
			//	for ( short u = 0; u < legs.size(); u++ )
			//		cout << legs[u];
			//	cout << endl;
			//}

		hybrideExplore( frequency, outFile, database, frequent2Tree, 
			headIndex, threshold, false); //explore me again, with single legs at bottom
	}
}
