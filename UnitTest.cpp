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

/*
int main() {
    // init log, rule list, randomness
    srand (time(NULL));
    std::string filename;
    std::cin>>filename;
    std::string rulefile = "./para_src/ruleset/" + filename;     //rules set
    std::string tracefile = "./para_src/ruleset/" + filename + "_trace"; //file
    rule_list *rList = new rule_list(rulefile, true);
    rList->createDAG();
    rList->obtain_cover();
    int total_weight = rList->obtain_weight(tracefile);
    Solutions ss;
    int cache_size = 0;
    std::cin>>cache_size;
    ss.CacheFlow(cache_size, rList, total_weight);      //test CacheFlow
    ss.myCache(cache_size, rList, total_weight);        //test MyCache
    return 0;
}

*/



