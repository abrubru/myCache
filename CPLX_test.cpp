/*Bash commands
ruleset genaration (using classbench)
for i in {1..20}; do ./db_generator -bc ../parameter_files/fw1_seed $((1000*$i)) 4 -0.8 -0.8 ../${i}k_4; done

H: Maximum split level
for i in {1..10}; do ./test -r ../ruleset/8k_4 $i; done

H: Rule set size
for i in {1..20}; do ./test -r ../ruleset/${i}k_4 5; done
*/
#include "stdafx.h"
#include "RuleList.h"
#include "BucketTree.h"
#include <time.h>
#include <fstream>

using namespace std;
clock_t t;
float seconds;

void set_bucket_hit(bucket *bk){// ???
	if (bk->sonList.empty()){
        if (!bk->related_rules.empty())
            bk->hit = true;
    }
	else{
        for (auto iter = bk->sonList.begin(); iter != bk->sonList.end(); iter++)
            set_bucket_hit(*iter);
    }
    return;
}

int HAHA(){
    ofstream out;
    out.open("CPLX_test_results", ofstream::app);

    int level;
    string filename;
    cin>>filename>>level;
    string rulefile = "./para_src/ruleset/" + filename;
    rule_list rList(rulefile, false);
//    rList.print("./rulelist");

    int rlist_size = rList.list.size();
    out<<"rlist size: "<<rlist_size<<"\t";
    out<<"level: "<<level<<"\t";


/***** generate bTree ******/
    bucket_tree bTree(rList, 20, false, rlist_size/4);

    bTree.tree_depth = 0;
    bTree.cal_tree_depth(bTree.root);
    out <<"bTree depth: "<< bTree.tree_depth << "\t";
    bTree.print_tree("./tree_before_alloc", true);
/***** pre_allocate large rules ******/
    bTree.pre_alloc();
//    bTree.tree_depth = 0;
//    bTree.cal_tree_depth(bTree.root);
//    out <<"bTree depth: "<< bTree.tree_depth << "\t";


/***** Test merge ******/
    out<<"TEST merge ";
    set_bucket_hit(bTree.root);
    bTree.print_tree("./tree_before_merge",true);

    t = clock();
    bTree.merge_bucket_CPLX_test(bTree.root);
    t = clock() - t;
    seconds = (float)t/CLOCKS_PER_SEC;
    out<<"take time: "<<seconds<<"\t";

    bTree.print_tree("./tree_after_merge", true);

//    bTree.tree_depth = 0;
//    bTree.cal_tree_depth(bTree.root);
//    out <<"bTree depth: "<< bTree.tree_depth << "\t";

/***** Test split ******/
    /*out<<"TEST split ";

    t = clock();
    bTree.repart_bucket_CPLX_test(level);
    t = clock() - t;
    seconds = (float)t/CLOCKS_PER_SEC;
    out<<"take time: "<<seconds<<"\t";*/

//    bTree.print_tree("./tree_after_split");

//    bTree.tree_depth = 0;
//    bTree.cal_tree_depth(bTree.root);
//    out <<"bTree depth: "<< bTree.tree_depth << "\t";

    out<<endl;
    out.close();
    //test DEC.19
    string trace_file = "./para_src/ruleset/" + filename + "_trace";
    bTree.search_test(trace_file);
	return 0;
}
