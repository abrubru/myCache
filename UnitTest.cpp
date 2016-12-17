#include "stdafx.h"
#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/log/trivial.hpp>
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include "TraceAnaly.hpp"
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
    std::string rulefile = "./para_src/advtest/" + filename;     //rules set
    std::string tracefile = "./para_src/advtest/" + filename + "_trace"; //file
    rule_list *rList = new rule_list(rulefile, true);
    rList->createDAG();   //create DAG
    rList->obtain_cover();   //计算cover-set
    rList->get_depth(); //获得DAG的高度
    cout<<"max depth:"<<'\t'<<rList->depth<<endl;
    int total_weight = rList->obtain_weight(tracefile); //计算每条规则的流量，并计算总流量
    rList->test_large_rule(total_weight); //test，large rules son
    cout<<"before shape:"<<'\t'<<total_weight<<endl;
    //rList->trace_shape(tracefile); //整形
    //tracefile = tracefile + "_shape";
    //total_weight = rList->obtain_weight(tracefile); //整形后
    cout<<"after shape:"<<'\t'<<total_weight<<endl;
    trace TR = trace(rList); //trace analyzation
    TR.trace_analyze_flow(tracefile);
    TR.trace_analyze_rule();
    //int overlap_weight = rList->test_trace(tracefile); //test trace file has overlapping packet?
    //cout<<"overlapping packets:  "<<overlap_weight<<"    all packets:  "<<total_weight<<"    radio:  "<<(double)overlap_weight/total_weight<<endl;
    Solutions ss;

    int cache_size = 0;
    std::cin>>cache_size;

    rList->select_optimal_rules(cache_size); //test optimal weight

    int sol = 0;
    std::cin>>sol;
    bool splited;
    int splited_max, splited_keep;
    std::cin>>splited>>splited_max>>splited_keep;
    if(sol == 0)
    	ss.CacheFlow(cache_size, rList, total_weight, tracefile);       //test CacheFlow
    else if(sol == 1)
    	ss.myCache(cache_size, rList, total_weight, tracefile, splited, splited_max, splited_keep);         //test MyCache
    else if(sol == 2)
    	ss.CAB(cache_size, rList, total_weight, tracefile, splited_keep);                 //test CAB
    else if(sol == 3){
    	ss.CacheFlow(cache_size, rList, total_weight, tracefile);
    	ss.myCache(cache_size, rList, total_weight, tracefile, splited, splited_max, splited_keep);
    }else if(sol > 3){
    	ss.CacheFlow(cache_size, rList, total_weight, tracefile);
    	ss.myCache(cache_size, rList, total_weight, tracefile, splited, splited_max, splited_keep);
    	ss.CAB(cache_size, rList, total_weight, tracefile, splited_keep);
    }

    return 0;
}





