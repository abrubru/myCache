#include "stdafx.h"
#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/log/trivial.hpp>
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include "CacheFlow.hpp"
#include "AllSolution.h"
/*
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include "CacheFlow.hpp"
*/


int main() {
    // init log, rule list, randomness
    srand (time(NULL));
    std::string filename;
    std::cin>>filename;
    std::string rulefile = "./para_src/ruleset/" + filename;     //rules set
    std::string tracefile = "./para_src/ruleset/" + filename + "_trace"; //file
    rule_list *rList = new rule_list(rulefile, false);
    rList->createDAG();
    rList->obtain_cover();
    int total_weight = rList->obtain_weight(tracefile);
    int overlap_weight = rList->test_trace(tracefile); //test trace file has overlapping packet?
    cout<<"overlapping packets:  "<<overlap_weight<<"    all packets:  "<<total_weight<<"    radio:  "<<(double)overlap_weight/total_weight<<endl;
    Solutions ss;
    int cache_size = 0;
    std::cin>>cache_size;
    int sol = 0;
    std::cin>>sol;
    if(sol == 0)
    	ss.CacheFlow(cache_size, rList, total_weight);       //test CacheFlow
    else if(sol == 1)
    	ss.myCache(cache_size, rList, total_weight, tracefile);         //test MyCache
    else if(sol == 2)
    	ss.CAB(cache_size, rList, total_weight, tracefile);                 //test CAB
    else if(sol == 3){
    	ss.CacheFlow(cache_size, rList, total_weight);
    	ss.myCache(cache_size, rList, total_weight, tracefile);
    	//ss.CAB(cache_size, rList, total_weight, tracefile);
    }else{
    	ss.CacheFlow(cache_size, rList, total_weight);
    	ss.myCache(cache_size, rList, total_weight, tracefile);
    	ss.CAB(cache_size, rList, total_weight, tracefile);

    }
    //CAB
    /***** generate bTree ******/

    return 0;
}





