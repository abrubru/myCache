#ifndef CACHE_FLOW_H
#define CACHE_FLOW_H

#include "stdafx.h"
#include "Address.hpp"
#include "Rule.hpp"
#include "RuleList.h"
#include <algorithm>
#include <unordered_set>

using std::set;
//using std::unordered_set;
using std::sort;

namespace cacheflow{

	class rule_info {
	public:
		uint32_t idx;
		bool is_cover;
		set<uint32_t> children;
		set<uint32_t> parents;
		int weight;             //流量
		int cost;                 //花费

	public:
		rule_info(){}    //DEC.15 没有这个形式的构造函数会报错
		rule_info(uint32_t i, rule_list * rL, bool is_cover2) {
			idx = i;
			is_cover = is_cover2;

			if (is_cover) {
				children = rL->children[i];
				parents = rL->parents[i];
			} else {
				children = rL->dep_map[i];
				parents = rL->parents[i];
			}
			weight = rL->list[i].weight;
			cost = children.size() + 1;
		}

		bool operator<(const rule_info & another) const {
			return (double)weight / cost < (double)another.weight / another.cost;
		}
	};

	class mixed_set {
	public:
		set<uint32_t> cache_rules;
		set<uint32_t> cover_rules;

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
				rule_info a(i, rList, true);
				candi_heap.push_back(a);  //DEC.14 cover-set
			}
			//cout<<candi_heap.size()<<" "<<total_memory<<endl;
			int cnt = 0;
			while(!candi_heap.empty()){
				std::make_heap(candi_heap.begin(), candi_heap.end());
				std::pop_heap(candi_heap.begin(), candi_heap.end());
				rule_info next_cache = candi_heap.back(); //下一条要缓存的rule
				candi_heap.pop_back();
				if(next_cache.cost + cache_rules.size() + cover_rules.size() <= total_memory){
					//cout<<next_cache.idx<<' '<<next_cache.weight<<' '<<next_cache.cost<<endl;
					//能装下
					cnt++;
					cache_rules.insert(next_cache.idx);   //rule进cache
					for(auto &child : next_cache.children ){
						if(cache_rules.find(child) == cache_rules.end()){//child不在cache中
							cover_rules.insert(child);  //cover-set
						}
					}

					//如果要添加的规则曾经作为cover-set，则在cover-set中删除
					auto finditer = cover_rules.find(next_cache.idx);
					if(finditer != cover_rules.end()) cover_rules.erase(finditer);

					//更新影响的parents
					vector<rule_info> associ_rules;
					for(auto canditer = candi_heap.begin(); canditer != candi_heap.end();){
						if(canditer->idx == next_cache.idx){
							canditer++;
							continue;
						}
						if(next_cache.parents.find(canditer->idx) != next_cache.parents.end()){ //属于next_cache的父亲
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
	};
}
#endif
