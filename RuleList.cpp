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
	//统计需要的信息
	//duck DEC. 3统计各种overlap
	int num_overlap = 0;
	int num_nest = 0;
	int num_cross = 0;
    depDag = depDAG(list.size());
    node.resize(list.size()); //结点数
    mnested.resize(list.size());
    mncross.resize(list.size());
    rev_mnested.resize(list.size());
    rev_mncross.resize(list.size());
    r_rule b(list[0]);
    for (int i = 1; i < list.size(); ++i){
    	r_rule a(list[i]);
        vector<r_rule> residual;
        residual.push_back( r_rule(list[i]));
        for (int j = i-1; j >= 0; --j){
        	int type = range_minus(residual, r_rule(list[j])) ;
            if (type != NONE){ //duck overlap? why not considerate priority
            	if(type == NEST){ //nested
            		mnested[i].insert(j);
            	}
            	else{ //crossed
            		mncross[i].insert(j);
            		rev_mncross[j].insert(i);
            	}
                boost::add_edge(j, i, depDag);  //注意这么计算后，边是错的，存在着多重覆盖
            }
        }
    }
	for (int i = 0; i < mnested.size(); i++) { //处理多重覆盖的问题
		std::set<int> deleted_node;
		for (auto iter1 = mnested[i].begin(); iter1 != mnested[i].end();
				iter1++) {
			for (auto iter2 = mnested[*iter1].begin();
					iter2 != mnested[*iter1].end(); iter2++) {
				deleted_node.insert(*iter2);
			}
		}
		for (auto iter1 = deleted_node.begin(); iter1 != deleted_node.end();
				iter1++) {
			mnested[i].erase(mnested[i].find(*iter1));
		}
		//计算反向边rev_nested
		for (auto iter = mnested[i].begin(); iter != mnested[i].end(); iter++)
			rev_mnested[*iter].insert(i);
	}
	//print info
	cout<<"rulelist info:    "<<endl;
	for(int i = 0; i < list.size(); i++){
		num_overlap += mnested[i].size() + mncross[i].size();
		num_cross += mncross[i].size();
		num_nest += mnested[i].size();
	}
	cout<<"rule num:"<<'\t'<<list.size()<<endl;
	cout<<"overlap:"<<'\t'<<num_overlap<<endl;
	cout<<"nest:"<<'\t'<<num_nest<<endl;
	cout<<"cross"<<'\t'<<num_cross<<endl;
}

void rule_list::obtain_dep() { // obtain the dependency map  this way is wrong,应该用dfs所有能遍历到的节点都是dep-set，*的dep-set是所有节点
    /*for(uint32_t idx = 0; idx < list.size(); ++idx) {
        vector <uint32_t> dep_rules;
        for (uint32_t idx1 = 0; idx1 < idx; ++idx1) {
            if (list[idx].dep_rule(list[idx1])) {
                dep_rules.push_back(idx1);
            }
        }
        dep_map[idx] = dep_rules;
    }*/
}
/*
 * 感觉这种计算方法不是很好
 */
/*void rule_list::obtain_cover() {  //duck /obtain the cover map
   vertex_iterator iter, end;
   adjacency_iterator adj_iter, adj_end;
   for (boost::tie(iter, end) = vertices(depDag); iter != end; ++iter){  //duck vertices(G) return its iterators of begin and end
       cover_map[*iter] = vector<uint32_t>();
       for (boost::tie(adj_iter, adj_end) = adjacent_vertices(*iter, depDag);
               adj_iter != adj_end; ++adj_iter){
           cover_map[*iter].push_back(*adj_iter);
       }
   }
}*/

void rule_list::obtain_cover(){  //DEC.15 obtain cover-set
	for(int i = 0; i < node.size(); i++){
		for(auto iter_nested = mnested[i].begin(); iter_nested != mnested[i].end(); iter_nested++) children[i].insert(*iter_nested);
		for(auto iter_crossed = mncross[i].begin(); iter_crossed != mncross[i].end(); iter_crossed++) children[i].insert(*iter_crossed);

		for(auto &x : rev_mnested[i]) parents[i].insert(x);
		for(auto &y : rev_mncross[i]) parents[i].insert(y);
	}
}

int rule_list::obtain_weight(string trace){//DEC.16 obtain rule's weight
	for(int i = 0; i < list.size(); i++) list[i].weight = 0;
	int weight = 0;
	ifstream file;
	file.open(trace.c_str());
	string sLine = "";
	getline(file, sLine);
	while (!file.eof()) {
		/*vector<string> temp;
		boost::split(temp, sLine, boost::is_any_of("\t"));
		list[atoi(temp[6].c_str())].weight++;*/
		addr_5tup packet(sLine, false);
		//int x = linear_search(packet);
		//if(x != -1)list[x].weight++;
		int x1 = -1, x2 = -1;
		for (size_t i = 0; i < list.size(); ++i) { //DEC.5
		     if (list[i].packet_hit(packet)){
		    	 if(x1 == -1){
		    		 x1 = i;
		    		 list[i].weight ++;
		    	 }else if(x2 == -1){
		    		 x2 = i;
		    		 if(mncross[x2].find(x1) != mncross[x2].end()) list[x2].cross_weight++;
		    		 break;
		    	 }
		     }
		}
		weight++;
		getline(file, sLine);
	}
	file.close();
	ofstream out;
	out.open("rules zero");
	for(int i = 0; i < list.size(); i++){
		if(list[i].weight == 0)
			out<<i<<'\t'<<children[i].size()<<endl;
	}
	out.close();
	//add test DEC.28
	return weight;
}

void rule_list::trace_shape(string trace){ //shape的主要原因是子节点为0的rule占了太多的流量,子节点多的规则占的流量都很少
	//流量清0
	for(int i = 0; i < list.size(); i++) list[i].weight = 0;
	ifstream file;
	ofstream out;
	file.open(trace.c_str());
	string shape = trace + "_shape";
	out.open(shape.c_str());
	string sLine = "";
	getline(file, sLine);
	while (!file.eof()) {
		addr_5tup packet(sLine, false);
		int x = linear_search(packet);
		if(x != -1){
			if(!(children[x].size() == 0 && list[x].weight > 100) ){
				int a = children[x].size()+2;
				int MAXS = 8;
				int rad = (int)rand() % std::min(a, MAXS);
				while(rad--){
					list[x].weight++;
					out<<sLine<<endl;
				}
			}
		}
		getline(file, sLine);
	}
	file.close();
	out.close();
}
//添加的

int rule_list::linear_search(const addr_5tup & packet) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].packet_hit(packet))
            return i;
    }
    return -1;
}

void rule_list::clearHitFlag() {
    for (size_t idx = 0; idx != list.size(); ++idx) {
        occupancy[idx] = 0;
        list[idx].hit = false;
    }
}

int rule_list::test_trace(string trace) {
	int weight = 0;
	ifstream file;
	file.open(trace.c_str());
	string sLine = "";
	getline(file, sLine);
	ofstream out;
	out.open("packets crossed parents");
	std::map<int, int> parents;
	std::map<int, int> crossed_parents;
	while (!file.eof()) {
		int num = 0;
		int r1 = -1, r2 = -1;
		addr_5tup packet(sLine, false);
		for (size_t i = 0; i < list.size(); ++i) {
		   if (list[i].packet_hit(packet)){
			   num++;
			   if(r1 == -1) r1 = i;
			   else if(r2 == -1) r2 = i;
		   }
		}
		if(mncross[r2].find(r1) != mncross[r2].end()) crossed_parents[r2]++;
		if(children[r2].find(r1) != children[r2].end())parents[r2]++;
		if (num > 1) weight++;
		getline(file, sLine);
	}
	file.close();
	for(auto x : parents){
		out<<x.first<<'\t'<<x.second<<'\t'<<children[x.first].size()<<'\t'<<list[x.first].weight<<'\t';
		if(crossed_parents.count(x.first)) out<<crossed_parents[x.first]<<'\t'<<mncross[x.first].size()<<'\t';
		out<<endl;
	}
	out.close();
	return weight;
}

void rule_list::test_rulelist_info(string trace){

}

void rule_list::test_large_rule(int total_weight){
	int weight = 0;
	std::set<int> allsons;
	std::vector<int> largerules;
	for(int i = 0; i < list.size(); i++){
		if(children[i].size() > list.size() / 20){
			weight += list[i].weight;
			largerules.push_back(i);
			for(auto x : children[i]) allsons.insert(x);
		}
	}
	cout<<"all large_rules  size"<<'\t'<<largerules.size()<<endl;
	cout<<"all large_rules sons size"<<'\t'<<allsons.size()<<endl;
	cout<<"all large_rules sons weight's radio"<<'\t'<<(double)weight / total_weight<<endl;
	//large节点之间的交集
	/*for(int i = largerules.size() - 1; i >= 1; i--){
		int k = 0;
		for(int j = 0; j < i; j++)
			if(children[largerules[i]].find(j) != children[largerules[i]].end()) k++;
		cout<<largerules[i]<<'\t'<<k<<endl;
	}
	//large节点之间的共同子节点
	for(int i = 0; i < largerules.size()-1; i++){
		for(int j = i + 1; j < largerules.size(); j++){
			int k = 0;
			for(auto x: children[largerules[i]]) if(children[largerules[j]].find(x) != children[largerules[j]].end()) k ++;
			cout<<largerules[i]<<'\t'<<largerules[j]<<'\t'<<k<<endl;
		}
	}*/
}

void rule_list::get_depth(){
	depth = 0;
	vector<int> dp(list.size(), 1);
	for(int i = 0; i < list.size(); i++){
		if(children[i].size()) depth = std::max(depth, get_each_depth(i, dp));
	}
}

int rule_list::get_each_depth(int i, vector<int> &dp){
	int maxs = 0;
	if(dp[i] != 1) return dp[i];
	for(auto x : children[i]) maxs = std::max(maxs, get_each_depth(x, dp));
	return dp[i] =maxs + 1;
}

//DEC.17 have no idea

void rule_list::select_optimal_rules(int total_memory){
	vector<int> cpweight(list.size(), 0);
	int allweight = 0;
	for(int i = 0; i < list.size(); i++){
		cpweight[i] = list[i].weight;
		allweight += list[i].weight;
	}
	std::sort(cpweight.begin(), cpweight.end(), std::greater<int>());
	int optimal_weight = 0;
	int k = 0;
	while(total_memory-- && k < list.size()) optimal_weight += cpweight[k++];
	cout<<"optimal weight"<<'\t'<<optimal_weight<<'\t'<<"radio"<<'\t'<<(double)optimal_weight / allweight<<endl;
}

