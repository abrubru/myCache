#include "stdafx.h"
#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/log/trivial.hpp>
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
/*
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include "CacheFlow.hpp"
*/


int main() {
    // init log, rule list, randomness
    srand (time(NULL));
    std::string rulefile = "./para_src/ruleset/acl_8000";
    rule_list rList(rulefile, true);
    rList.createDAG();
    return 0;
}





