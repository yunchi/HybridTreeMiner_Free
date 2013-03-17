GCC=g++ -O3
OBJS=VerticalTreeMiner.o VFrequentTreeList.o VFrequentTreeFamily.o BFCFTree.o VFreeTree.o
HybridTreeMiner_Free: VerticalTreeMiner.o VFrequentTreeList.o VFrequentTreeFamily.o BFCFTree.o VFreeTree.o
	$(GCC) -o HybridTreeMiner_Free VerticalTreeMiner.o VFrequentTreeList.o VFrequentTreeFamily.o BFCFTree.o VFreeTree.o;
	rm -f $(OBJS)
VerticalTreeMiner.o: VerticalTreeMiner.cpp VFrequentTreeList.h VFrequentTreeFamily.h BFCFTree.h VFreeTree.h Vmisc.h
	$(GCC) -c VerticalTreeMiner.cpp
VFrequentTreeList.o: VFrequentTreeList.cpp VFrequentTreeList.h VFrequentTreeFamily.h BFCFTree.h VFreeTree.h Vmisc.h
	$(GCC) -c VFrequentTreeList.cpp 
VFrequentTreeFamily.o: VFrequentTreeFamily.cpp VFrequentTreeFamily.h BFCFTree.h VFreeTree.h Vmisc.h
	$(GCC) -c VFrequentTreeFamily.cpp
BFCFTree.o: BFCFTree.cpp BFCFTree.h VFreeTree.h Vmisc.h
	$(GCC) -c BFCFTree.cpp
VFreeTree.o: VFreeTree.cpp VFreeTree.h Vmisc.h
	$(GCC) -c VFreeTree.cpp
