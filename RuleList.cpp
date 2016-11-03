#include "RuleList.h"

using std::ifstream;
using std::ofstream;
using std::string;

/* constructor
 *
 * options:
 * 	()			default
 * 	(string &)		draw from file
 */
rule_list::rule_list() {}

rule_list::rule_list(string & filename, bool test_bed) {
    ifstream file;
    file.open(filename.c_str());
    string sLine = "";
    getline(file, sLine);
    while (!file.eof()) {
        p_rule sRule(sLine, test_bed);
        list.push_back(sRule);
        getline(file, sLine);
    }
    occupancy = vector<size_t>(list.size(), 0);
    file.close();
    for(auto iter = list.begin(); iter != list.end(); ++iter) { //delete the same rule
    	 //test list rules
        for (auto iter_cp = iter+1; iter_cp != list.end(); ) {
            if (*iter == *iter_cp) 
                iter_cp = list.erase(iter_cp);
	else
		++iter_cp;
        }
    }
}


void rule_list::createDAG(){ //duck O(n^2)
    depDag = depDAG(list.size());
    //duck DEC. 3统计各种overlap
    int num_overlap = 0;
    int num_nest = 0;
    int num_cross = 0;
    for (int i = 1; i < list.size(); ++i){
        vector<r_rule> residual;
        residual.push_back(r_rule(list[i]));
        for (int j = i-1; j >= 0; --j){
        	int type = range_minus(residual, list[j]) ;
            if (type != NONE){ //duck overlap? why not considerate priority
            	if(type == NEST) num_nest++;
            	else num_cross++;
                boost::add_edge(j, i, depDag); 
                num_overlap++;
            }
        }
    }
    cout<<"node: "<<list.size()<<endl;
    cout<<"overlap: "<<num_overlap<<endl;
    cout<<"nest: "<<num_nest<<endl;
    cout<<"cross: "<<num_cross<<endl;
}

