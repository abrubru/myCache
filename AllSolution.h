#ifndef ALLSOLUTION_H
#define ALLSOLUTION_H
#include "CacheFlow.hpp"
#include "myCache.hpp"
#include "CAB.hpp"
class Solutions{
public:
	void CacheFlow(int memory, rule_list * rL, int weight, string trace);  //CacheFlow
	void myCache(int memory, rule_list * rL, int weight, string trace,bool splited, int splited_max, int splited_keep); //myCache
	void CAB(int memory, rule_list *rL, int weight, string trace, int splited_keep);  //CAB
};
#endif
