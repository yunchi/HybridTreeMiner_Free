/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	main function for VerticalTreeMiner

	Description: 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Vmisc.h"
#include "VFreeTree.h"
#include "BFCFTree.h"
#include "VFrequentTreeList.h"
#include "VFrequentTreeFamily.h"

using namespace std;

short MIN_EDGE = ENDTAG;
short MAX_EDGE = 0;
short MIN_VERTEX = ENDTAG;
short MAX_VERTEX = 0;

//for debugging the inconsistency between extension and hybrid
vector< vector<short> > result1;
vector< vector<short> > result2;


bool PURE_EXTENSION = true;

int main(int argc, char* argv[])
{

	if ( argc != 4 )
	{	
		cout << "Usage: HybridTreeMiner_Free support input_file output_file" << endl;
		exit (1);
	}
	
	long support;
	istringstream iss(argv[1]);
    iss >> support;
    if(!iss)
    {
        cout << "invalid argument, not an integer value" << endl;
        exit (1);
    }

	time_t start_time;
	time_t stop_time;
	long temp;


	/******************************************************************
	step1: read in the database
	******************************************************************/
	string inputFile = argv[2];
	string outputFile = argv[3];

	ofstream dataFile(outputFile.c_str());
	if(!dataFile)
	{
		cerr << "cannot open OUTPUT file!" << endl;
		exit(1);
	}

	ifstream inFile(inputFile.c_str());
	if(!inFile)
	{
		cerr << "cannot open INPUT file!" << endl;
		exit(1);
	}


	dataFile << "#Output Statistics for " << argv[2] << endl;
        dataFile << "#with support " << argv[1] << endl << endl;	
	
	vector<ptrVFreeTree> database;

	//read in the database
	ptrVFreeTree pft;
	char c;
	long counter = 1;
	database.push_back(new VFreeTree()); //a dummy tree
	while ( !inFile.eof() )
	{
		c = inFile.get();
		if ( inFile.eof() ) break;
		else inFile.putback(c);
		pft = new VFreeTree();
		inFile >> *pft;
		pft->tid = counter++; //force the tid list to be
		database.push_back(pft);
		c = inFile.get(); //to eat the last "end of line"
	}
	inFile.close();

	/******************************************************************
	step2: explore frequent free trees, either using pure extension, or
	use the hybrid method (extension + join)
	******************************************************************/
	vector<long> frequency(100);

	//if ( PURE_EXTENSION )
	//{
	//	VFrequentTreeList freq2;
	//	freq2.populate2(database);
	//	freq2.finalize2(support);
	//	freq2.extensionExploreList(database, dataFile, frequency, support);
	//}
	//else
	//{
	//	VFrequentTreeList freq1;
	//	freq1.populate1(database);
	//	freq1.finalize1(support);
	//	freq1.hybridExploreList(database, dataFile, frequency, support);
	//}

  	//start_time = time(0);

	  //VFrequentTreeList freq2;
	  //freq2.populate2(database);
	  //freq2.finalize2(support);
	  //freq2.extensionExploreList(database, dataFile, frequency, support);
	
	  //stop_time = time(0);

	  //dataFile << "pure extension: " << endl;
	  //for ( int i = 2; i < 100; i++ )
	  //{
	  //	if ( frequency[i] > 0 )
	  //	dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
	  //else break;
	  //}
	  //dataFile << "time spent for pure extension: " << difftime(stop_time, start_time) << endl;
	  //dataFile << endl;

	  //for ( short j = 0; j < 100; j++ ) frequency[j] = 0;

	start_time = time(0);

	VFrequentTreeList freq1;
	freq1.populate1(database);
	freq1.finalize1(support);
	  long temp1 = freq1.frequent1List.size();
	freq1.hybridExploreList(database, dataFile, frequency, support);

	stop_time = time(0);


	dataFile << "hybrid: " << endl;

	  dataFile << "frequent 1 trees: " << temp1 << endl;;
	long totalSum = 0;
	for ( int i = 2; i < 100; i++ )
	{
		if ( frequency[i] > 0 )
		{
			dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
			totalSum += frequency[i];
		}
		else break;
	}
	dataFile << "total frequent trees: " << totalSum + temp1 << endl;
	dataFile << "time spent for hybrid method: " << difftime(stop_time, start_time) << endl;
	dataFile << endl;

	/******************************************************************
	step3: remove the database from the memory and output the results
	******************************************************************/
	for ( long s = 0; s < database.size(); s++ )
	{
		delete database[s];
	}

	//cout << "first size: " << result1.size() << endl;
	//cout << "second size: " << result2.size() << endl;
	
	//for ( short j = 0; j < result1.size(); j++ )
	//{
	//	if ( find(result2.begin(), result2.end(), result1[j]) == result2.end() )
	//	{
	//		cout << "j is: " << j << endl;
	//		for ( short k = 0; k < result1[j].size(); k++ )
	//			cout << result1[j][k] << ' ' ;
	//		cout << endl;
	//	}
	//}


	//short j = 0;
	//while ( j < result2.size() )
	//{
	//	if ( result1[j] != result2[j] )
	//	{
	//		cout << "j is: " << j << endl;
	//		for ( short k = 0; k < result1[j].size(); k++ )
	//			cout << result1[j][k] << ' ' ;
	//		cout << endl;
	//		break;
	//	}
	//	j++;
	//}

	//sort(result1.begin(), result1.end());
	//sort(result2.begin(), result2.end());

	//for ( int i = 2; i < 100; i++ )
	//{
	//	if ( frequency[i] > 0 )
	//		dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
	//	else break;
	//}

	//stop_time = time(0);
	//dataFile << "time spent: " << difftime(stop_time, start_time) << endl;
	//dataFile << endl;

	dataFile.close();
	return 0;
}
