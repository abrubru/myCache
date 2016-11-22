#ifndef MY_CACHE_H
#define MY_CACHE_H

using std::ifstream;
using std::ofstream;
using std::string;

#include "stdafx.h"
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include "Bucket.h"
#include "BucketTree.h"
#include <algorithm>
#include <set>
using std::set;

namespace mycache{

	class split_nodes{ //切分子节点多的规则
	public:
		rule_list *rL;
	public:
		split_nodes(rule_list *r, int maxs, int splited){
			for(int i = 0; i < r->list.size(); i++){
				if(r->mncross[i] + r->mnested[i] > maxs){ //split
					//bucket_tree bTree(*rL, 20, false, 0);
				}
			}
		}
	};
	class rule_info {
	public:
		uint32_t idx;
		set<uint32_t> children_nested;
		set<uint32_t> children_crossed;
		set<uint32_t> parents_nested;
		set<uint32_t> parents_crossed;
		int weight;             //流量
		int cost;                 //花费

	public:
		rule_info(){}    //DEC.15 没有这个形式的构造函数会报错
		rule_info(uint32_t i, rule_list * rL) {
			idx = i;
			children_nested = rL->mnested[i];
			children_crossed = rL->mncross[i];
			parents_nested = rL->rev_mnested[i];
			parents_crossed = rL->rev_mncross[i];
			weight = rL->list[i].weight;
			cost = children_nested.size() + children_crossed.size() + 1;
		}

		bool operator<(const rule_info & another) const {
			return (double)weight / cost < (double)another.weight / another.cost;
		}
	};

	class mixed_set {
	public:
		set<uint32_t> cache_rules;
		set<uint32_t> cover_rules_nested;
		set<uint32_t> cover_rules_crossed;

	private:
		int total_memory;
		rule_list * rList;

	public:
		mixed_set(int memory, rule_list * rL) {
			total_memory = memory;
			rList = rL;
		}

		void cal_mixed_set() {
			//std::map<uint32_t, rule_info> candi_heap;   //候选堆
			//std::map<rule_info, uint32_t> candi_heap;//候选堆
			vector<rule_info> candi_heap;
			for (int i = 0; i < rList->list.size(); ++i) {
				rule_info a(i, rList);
				candi_heap.push_back(a);  //DEC.14 cover-set
			}
			//cout<<candi_heap.size()<<" "<<total_memory<<endl;
			int cnt = 0;
			while(!candi_heap.empty()){
				std::make_heap(candi_heap.begin(), candi_heap.end());
				std::pop_heap(candi_heap.begin(), candi_heap.end());
				rule_info next_cache = candi_heap.back(); //下一条要缓存的rule
				candi_heap.pop_back();
				if(next_cache.cost + cache_rules.size() + cover_rules_nested.size() + cover_rules_crossed.size() <= total_memory){
					//cout<<next_cache.idx<<' '<<next_cache.weight<<' '<<next_cache.cost<<endl;
					//能装下
					cnt++;
					cache_rules.insert(next_cache.idx);   //rule进cache
					for(auto &child_nested : next_cache.children_nested ){ //children_nested进cover_set_nested
						if(cache_rules.find(child_nested) == cache_rules.end()){//child不在cache中
							cover_rules_nested.insert(child_nested);  //cover-set
						}
					}
					for(auto &child_crossed : next_cache.children_crossed ){ //children_nested进cover_set_crossed
						if(cache_rules.find(child_crossed) == cache_rules.end()){//child不在cache中
							cover_rules_crossed.insert(child_crossed);  //cover-set
						}
					}

					//如果要添加的规则曾经作为cover-set，则在cover-set中删除
					auto finditer = cover_rules_nested.find(next_cache.idx);
					if(finditer != cover_rules_nested.end()) cover_rules_nested.erase(finditer);
					finditer = cover_rules_crossed.find(next_cache.idx);
					if(finditer != cover_rules_crossed.end()) cover_rules_crossed.erase(finditer);

					//更新影响的parents
					vector<rule_info> associ_rules;
					for(auto canditer = candi_heap.begin(); canditer != candi_heap.end();){
						if(canditer->idx == next_cache.idx){
							canditer++;
							continue;
						}
						if(next_cache.parents_nested.find(canditer->idx) != next_cache.parents_nested.end() ||
								next_cache.parents_crossed.find(canditer->idx) != next_cache.parents_crossed.end()){ //属于next_cache的父亲
							rule_info parent = *canditer;
							parent.cost--;
							associ_rules.push_back(parent);
							canditer = candi_heap.erase(canditer);
						}else{
							canditer++;
						}
					}
					for(auto x : associ_rules) candi_heap.push_back(x);
				}
			}
			//cout<<cnt<<endl;
		}

		int cal_cover_table_weight(string trace){
			int cover_weight = 0;
			ifstream file;
			file.open(trace.c_str());
			string sLine = "";
			getline(file, sLine);
			while (!file.eof()) {
				addr_5tup packet(sLine, false);
				int x = rList->linear_search(packet);
				if(x != -1 && cover_rules_crossed.find(x) != cover_rules_crossed.end()){ //可以表征流量的,set，优先级已经排好
					for(auto iterrule : cache_rules){
						//cout<<"haha"<<endl;
						if(rList->list[iterrule].packet_hit(packet)){
							cover_weight++;
							break;
						}
					}
				}
				getline(file, sLine);
			}
			file.close();
			//test
			set<int> overlapped_rules;
			for(auto x : cache_rules){
				for(auto y : cover_rules_crossed){
					if(rList->mncross[x].find(y) != rList->mncross[x].end()) overlapped_rules.insert(x);
				}
			}
			cout<<"overlapped_rules:  "<<overlapped_rules.size()<<"cover_weight:  "<<cover_weight<<endl;
			for(auto x : overlapped_rules) cout<<x<<" ";
			return cover_weight;
		}
	};
}
#endif

