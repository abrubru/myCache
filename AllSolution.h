#ifndef ALLSOLUTION_H
#define ALLSOLUTION_H
#include "CacheFlow.hpp"
#include "myCache.hpp"
#include "CAB.hpp"
class Solutions{
public:
	void CacheFlow(int memory, rule_list * rL, int weight);  //CacheFlow
	void myCache(int memory, rule_list * rL, int weight); //myCache
	void CAB(int memory, rule_list *rL, int weight, string trace);  //CAB
};
#endif
