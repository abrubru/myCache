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

//统计需要的信息
//duck DEC. 3统计各种overlap
int num_overlap = 0;
int num_nest = 0;
int num_cross = 0;
//duck DEC. 3统计覆盖域广的规则
void rule_list::print_info() {
	cout << "node: " << list.size() << endl;
	cout << "overlap: " << num_overlap << endl;
	cout << "nest: " << num_nest << endl;
	cout << "cross: " << num_cross << endl;
	num_nest = num_cross = 0;
	//duck DEC. 4 处理嵌套覆盖
	num_nest = 0;
	int num10 = 0, num50 = 0, num100 = 0, num200 = 0, num500 = 0, num1000 = 0,
			num1001 = 0;
	int snum10 = 0, snum50 = 0, snum100 = 0, snum200 = 0, snum500 = 0,
			snum1000 = 0, snum1001 = 0;
	for (int i = 0; i < mnested.size(); i++) { //处理多重覆盖的问题
		std::set<int> deleted_node;
		for (auto iter1 = mnested[i].begin(); iter1 != mnested[i].end(); iter1++) {
			for (auto iter2 = mnested[*iter1].begin(); iter2 != mnested[*iter1].end(); iter2++) {
				deleted_node.insert(*iter2);
			}
		}
		for (auto iter1 = deleted_node.begin(); iter1 != deleted_node.end(); iter1++) {
			mnested[i].erase(mnested[i].find(*iter1));
		}
		//计算反向边rev_nested
		for(auto iter = mnested[i].begin(); iter != mnested[i].end(); iter++) rev_mnested[*iter].insert(i);
	}
	for(int i = 0; i < node.size(); i++){ //统计规则覆盖条目落在各个范围内的情况
		node[i] = mnested[i].size() + mncross[i].size();
		num_nest += mnested[i].size();
		num_cross += mncross[i].size();
		if(node[i] < 10){
			num10++;
			snum10 += node[i];
		}else if(node[i] < 50){
			num50++;
			snum50 += node[i];
		}else if(node[i] < 100){
			num100++;
			snum100 += node[i];
		}else if(node[i] < 200){
			num200++;
			snum200 += node[i];
		}else if(node[i] < 500){
			num500++;
			snum500 += node[i];
		}else if(node[i] < 1000){
			num1000++;
			snum1000 += node[i];
		}else{
			num1001++;
			snum1001 += node[i];
		}
	}
	cout << "nest: " << num_nest << endl;
	cout << "cross: " << num_cross << endl;
	cout << "[0, 10):          " << num10 << "    " << snum10 << endl;
	cout << "[11, 50):        " << num50 << "    " << snum50 << endl;
	cout << "[50, 100):      " << num100 << "    " << snum100 << endl;
	cout << "[100, 200):    " << num200 << "    " << snum200 << endl;
	cout << "[200, 500)     " << num500 << "    " << snum500 << endl;
	cout << "[500, 1000):  " << num1000 << "	    " << snum1000 << endl;
	cout << "[1000, ~):      " << num1001 << "	    " << snum1001 << endl;
	num10 = num50 = num100 = num200 = num500 = num1000 = num1001 = 0;
	snum10 = snum50 = snum100 = snum200 = snum500 = snum1000 = snum1001 = 0;
}

void rule_list::createDAG(){ //duck O(n^2)
    depDag = depDAG(list.size());
    node.resize(list.size()); //结点数
    mnested.resize(list.size());
    mncross.resize(list.size());
    rev_mnested.resize(list.size());
    rev_mncross.resize(list.size());
    for (int i = 1; i < list.size(); ++i){
        vector<r_rule> residual;
        residual.push_back( r_rule(list[i]));
        for (int j = i-1; j >= 0; --j){
        	int type = range_minus(residual, r_rule(list[j])) ;
            if (type != NONE){ //duck overlap? why not considerate priority
            	if(type == NEST){ //nested
            		mnested[i].insert(j);
            		num_nest++;
            	}
            	else{ //crossed
            		mncross[i].insert(j);
            		rev_mncross[j].insert(i);
            		num_cross++;
            	}
                boost::add_edge(j, i, depDag);  //注意这么计算后，边是错的，存在着多重覆盖
                num_overlap++;
            }
        }
    }
    print_info();
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
	int weight = 0;
	ifstream file;
	file.open(trace.c_str());
	string sLine = "";
	getline(file, sLine);
	while (!file.eof()) {
		vector<string> temp;
		boost::split(temp, sLine, boost::is_any_of("\t"));
		list[atoi(temp[6].c_str())].weight++;
		weight++;
		getline(file, sLine);
	}
	file.close();
	return weight;
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
