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
#include "CAB.hpp"
#include <algorithm>
#include <set>
using std::set;

namespace mycache{

	class split_nodes{ //切分子节点多的规则
	public:
		rule_list *rL;
		vector<pair<uint32_t, bucket_tree*> > mbtree;
	public:
		/*
		 * 这样设定阈值需要切分的字节点数目太多了 A
		 */

		split_nodes(rule_list *r, int splited_max, int splited_keep, string trace){
			rL = r;
			mbtree.clear();
			int sum_height = 0;
			int num = 0;
			vector<bool> deleted_rules(r->list.size(), false);
			for(int i = 0; i < r->list.size(); i++){
				if(r->mncross[i].size() + r->mnested[i].size() > splited_max){ //split
					deleted_rules[i] = true;
					num++; //cal splited num;
					bucket_tree *bTree = new bucket_tree(*r, splited_keep, false, 0, i);
					/*
					bTree->pre_alloc();
					set_bucket_hit2(bTree->root);
					bTree->merge_bucket_CPLX_test(bTree->root);
					*/
					bTree->obtain_bucket_weight(trace, i, splited_max);
					bTree->cal_tree_depth(bTree->root);
					cout<<"tree depth:"<<'\t'<<bTree->tree_depth<<endl;
					sum_height += bTree->tree_depth; //cal all tree's height
					mbtree.push_back(std::make_pair(i, bTree));
				}
			}
			bucket_tree *bTree = new bucket_tree(*r, splited_keep, false, 0, -2, deleted_rules);
			/*
			bTree->pre_alloc();
			set_bucket_hit2(bTree->root);
			bTree->merge_bucket_CPLX_test(bTree->root);
			*/
			bTree->cal_tree_depth(bTree->root);
			cout<<"Global Tree depth"<<'\t'<<bTree->tree_depth<<endl;
			cout<<"splited radio"<<'\t'<<(double) num / r->list.size()<<endl;
			cout<<"average tree depth"<<'\t'<<(double) sum_height / num<<endl;
		}

		//直接切 B
		/*
		split_nodes(rule_list *r, int splited_max, int splited_keep, string trace){
			rL = r;
			mbtree.clear();
			bucket_tree *bTree = new bucket_tree(*r, splited_keep, false, 0);
			bTree->pre_alloc();
			set_bucket_hit2(bTree->root);
			bTree->merge_bucket_CPLX_test(bTree->root);
			bTree->obtain_bucket_weight(trace);
			mbtree.push_back(std::make_pair(rL->list.size(), bTree));
		}
		*/
		//分开合并切分 C
		/*
		split_nodes(rule_list *r, int splited_max, int splited_keep, string trace){
			rL = r;
			mbtree.clear();
			vector<bool> deleted_rules(r->list.size(), true);
			for(int i = 0; i < r->list.size(); i++){
				if(r->mncross[i].size() + r->mnested[i].size() > splited_max){
					for(auto x : r->children[i]) deleted_rules[x] = false;
				}
			}
			bucket_tree *bTree = new bucket_tree(*r, splited_keep, false, 0, -2, deleted_rules);
			bTree->pre_alloc();
			set_bucket_hit2(bTree->root);
			bTree->merge_bucket_CPLX_test(bTree->root);
			bTree->cal_tree_depth(bTree->root);
			cout<<"Global Tree depth"<<'\t'<<bTree->tree_depth<<endl;
		}*/
		void set_bucket_hit2(bucket *bk){
			if (bk->sonList.empty()){
				if (!bk->related_rules.empty())
					bk->hit = true;
				}
				else{
				  for (auto iter = bk->sonList.begin(); iter != bk->sonList.end(); iter++)
					  set_bucket_hit2(*iter);
			 }
			 return;
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
			weight = rL->list[i].weight /*+ rL->list[i].cross_weight*/;
			cost = children_nested.size() + children_crossed.size() + 1;
		}

		bool operator<(const rule_info & another) const {
			return (double)weight / cost < (double)another.weight / another.cost;
		}
	};

	class cover_set{
	public:
		uint32_t idx; //ruleId;
		bool cross; //true cross, false nest
		cover_set(uint32_t a, bool b): idx(a), cross(b){}
		bool operator <(const cover_set &c) const{
			return idx < c.idx;
		}
		bool operator ==(const cover_set &c) const{
			return idx == c.idx;
		}
	};
	class mixed_set {
	public:
		vector<rule_info> candi_heap; //候选堆
		set<uint32_t> cache_rules;   //rules table
		set<cover_set> cover_rules;  //cover-set table

		vector<cab::cache_bucket> candi_buckets; //候选bucket
		vector<cab::cache_bucket> cache_buckets; //buckets
		std::map<uint32_t,  bucket_tree*>mbtree;

		int allweight_less; //计算cache的流量，只是计算一部分
	private:
		int total_memory;
		rule_list * rList;
	public:
		/*
		 * memory:             cache的大小
		 * rL:                        规则集
		 * split:                    是否split
		 * trace:                   trace
		 * splited_max:       split的阈值
		 * splited_keep:      切分之后的字节点个数
		 */
		mixed_set(int memory, rule_list * rL, bool split, string trace, int splited_max, int splited_keep) {
			total_memory = memory;
			rList = rL;
			if(!split){//no split, like cacheflow
				for (int i = 0; i < rList->list.size(); ++i) {
					rule_info a(i, rList);
					candi_heap.push_back(a);  //DEC.14 cover-set
				}
			}else{ //split
				split_nodes sp = split_nodes(rL, splited_max, splited_keep, trace);  //切分字节点多的规则
				for(auto iterbt = sp.mbtree.begin(); iterbt != sp.mbtree.end(); iterbt++){ //获得切分之后的规则桶,candi_bucket
					mbtree[iterbt->first] = iterbt->second;
					obtain_all_buckets(iterbt->second->root, iterbt->first);
				}
				for (int i = 0; i < rList->list.size(); ++i) {//candi_heap
					if(rList->mncross[i].size() + rList->mnested[i].size() > splited_max) continue;
					rule_info a(i, rList);
					candi_heap.push_back(a);  //DEC.14 cover-set
				}
			}
		}
		void cal_mixed_set(bool split) {
			allweight_less = 0;
			cout<<"candi_heap: "<<candi_heap.size()<<" candi_bucket: "<<candi_buckets.size()<<endl;

			while(candi_heap.size() && candi_buckets.size()){
				std::make_heap(candi_heap.begin(), candi_heap.end());
				std::pop_heap(candi_heap.begin(), candi_heap.end());
				rule_info next_rule = candi_heap.back(); //下一条要缓存的rule
				if(!split){
					cache_next_rule(next_rule);
				}else{
					//bucket能不能插入
					std::make_heap(candi_buckets.begin(), candi_buckets.end());
					std::pop_heap(candi_buckets.begin(), candi_buckets.end());
					cab::cache_bucket next_bucket = candi_buckets.back(); //下一個要緩存的bucket
					//cout<<next_rule.weight<<'\t'<<next_rule.cost<<'\t'<<next_bucket.weight<<'\t'<<next_bucket.cost<<endl;
					if((double) next_rule.weight / next_rule.cost > (double)next_bucket.weight / next_bucket.cost){//缓存rule还是bucket
						cache_next_rule(next_rule);
					}else{
						cache_next_bucket(next_bucket);
					}
				}
			}
			while(candi_heap.size()){
				std::make_heap(candi_heap.begin(), candi_heap.end());
				std::pop_heap(candi_heap.begin(), candi_heap.end());
				rule_info next_rule = candi_heap.back(); //下一条要缓存的rule
				cache_next_rule(next_rule);
			}
			while(candi_buckets.size()){
				std::make_heap(candi_buckets.begin(), candi_buckets.end());
				std::pop_heap(candi_buckets.begin(), candi_buckets.end());
				cab::cache_bucket next_bucket = candi_buckets.back(); //下一個要緩存的bucket
				cache_next_bucket(next_bucket);
			}
			//cout<<cnt<<endl;
		}
		void cache_next_rule(rule_info &next_cache){
			candi_heap.pop_back();
			if(next_cache.cost + cache_rules.size() + cover_rules.size() + cache_buckets.size() > total_memory ) return;
			//cout<<next_cache.idx<<' '<<next_cache.weight<<' '<<next_cache.cost<<endl;
			//能装下
			allweight_less += next_cache.weight;
			cache_rules.insert(next_cache.idx);   //rule进cache
			for(auto &child_nested : next_cache.children_nested ){ //children_nested进cover_set_nested
				if(cache_rules.find(child_nested) == cache_rules.end()){//child不在cache中
					cover_set a(child_nested, false);
					auto checkIter = cover_rules.find(a);
					if(checkIter == cover_rules.end()){//不在cover_rules里
						cover_rules.insert(a);
					}else if(checkIter->cross == true){//在cover_rules里，但是原来的状态是crossed,改为nested
						cover_rules.erase(checkIter);
						cover_rules.insert(a);
					}
				}
			}
			for(auto &child_crossed : next_cache.children_crossed ){ //children_crossed进cover_set_crossed
				if(cache_rules.find(child_crossed) == cache_rules.end()){//child不在cache中
					cover_set a(child_crossed, true);
					auto checkIter = cover_rules.find(a);
					if(checkIter == cover_rules.end()){//不在cover_rules里
						cover_rules.insert(a);
					}
				}
			}

			//如果要添加的规则曾经作为cover-set，则在cover-set中删除
			auto finditer = cover_rules.find(cover_set(next_cache.idx, true));
			if(finditer != cover_rules.end()) cover_rules.erase(finditer);

			//更新影响的parents
			vector<int> associ_rules; //itselft
			associ_rules.push_back(next_cache.idx);
			for(auto x : next_cache.children_crossed) associ_rules.push_back(x); //children_crossed
			for(auto x : next_cache.children_nested) associ_rules.push_back(x);  //children_nested

			for(auto arule : associ_rules){
				for(auto canditer = candi_heap.begin(); canditer != candi_heap.end(); canditer++){
					if(canditer->children_crossed.find(arule) != canditer->children_crossed.end()){ //受影响的parent
						canditer->children_crossed.erase(arule);
						canditer->cost--;
					}
					if(canditer->children_nested.find(arule) != canditer->children_nested.end()){ //受影响的parent
						canditer->children_nested.erase(arule);
						canditer->cost--;
					}
				}
				//受影响的split
				for(auto iterbucket = candi_buckets.begin(); iterbucket != candi_buckets.end(); iterbucket++){
					auto deletediter = iterbucket->related_rules.find(arule);
					if(deletediter != iterbucket->related_rules.end()){
						iterbucket->related_rules.erase(deletediter); //删除已经放进cache的规则
						iterbucket->cost--;    //cost--
					}
				}
			}
		}

		void cache_next_bucket(cab::cache_bucket next_cache){
			candi_buckets.pop_back();
			if(next_cache.cost + cache_rules.size() + cover_rules.size() + cache_buckets.size() > total_memory) return;
			//能装下
			//cout<<(double)next_cache.weight/next_cache.cost<<" "<<next_cache.cost<<endl;
			allweight_less += next_cache.weight;
			cache_buckets.push_back(next_cache); //bucket进buckets table
			for(auto rule : next_cache.related_rules){
				/*
				 *A
				 */

				if(rList->mncross[next_cache.idx].find(rule) !=  rList->mncross[next_cache.idx].end()){//crossed
					if(cover_rules.find(cover_set(rule, false) ) == cover_rules.end()){//防止改变原来false的状态
						cover_rules.insert(cover_set(rule, true));
					}
				}else{//nested
					cover_rules.insert(cover_set(rule, false));
				}

				/*
				 * B
				 */
				/*
				cover_set a(rule, false);
				auto checkIter = cover_rules.find(a);
				if(checkIter == cover_rules.end()){//a不在cover_rules里
					cover_rules.insert(cover_set(rule, true));
				}
				*/
				for(auto iterbucket = candi_buckets.begin(); iterbucket != candi_buckets.end(); iterbucket++){
					auto deletediter = iterbucket->related_rules.find(rule);
					if(deletediter != iterbucket->related_rules.end()){
						iterbucket->related_rules.erase(deletediter);
						iterbucket->cost--;
					}
				}

				//受影响的candi_heap
				for(auto canditer = candi_heap.begin(); canditer != candi_heap.end(); canditer++){
					if(canditer->children_crossed.find(rule) != canditer->children_crossed.end()){ //受影响的parent
						canditer->children_crossed.erase(rule);
						canditer->cost--;
					}
					if(canditer->children_nested.find(rule) != canditer->children_nested.end()){ //受影响的parent
						canditer->children_nested.erase(rule);
						canditer->cost--;
					}
				}
			}
		}

		int cal_all_weight(string trace){
			int allweight = 0;
			ifstream file;
			file.open(trace.c_str());
			string sLine = "";
			getline(file, sLine);
			vector<string> addp;
			while (!file.eof()) {
				addr_5tup packet(sLine, false);
				string pac = sLine;
				getline(file, sLine);
				int x  = -1;
				//第一级cache
				for(auto ruleId : cache_rules){   //现在rules_table 的cache_rules里面找
					if (rList->list[ruleId].packet_hit(packet)){
						x = ruleId;
						break;
					}
				}
				/*
				 * A
				 */

				for(auto bucket: cache_buckets){//在rules_table的buckets里面找
					if(rList->list[bucket.idx].packet_hit(packet)){
						auto result = mbtree[bucket.idx]->search_bucket(packet, mbtree[bucket.idx]->root);
						if(result.first == bucket.cBucket){
							x = bucket.idx;
						}
					}
				}

				/*
				 * B
				 */
				/*
				auto result = mbtree[rList->list.size()]->search_bucket(packet, mbtree[rList->list.size()]->root);
				for(int i = 0; i < cache_buckets.size(); i++) if(result.first == cache_buckets[i].cBucket){
					x = rList->list.size();
					break;
				}
				*/
				if(x == -1) continue; //不匹配第一级cache
				 allweight++;
				//第二级cache
				for(auto cover : cover_rules){ //在cover-set里面找
					if(rList->list[cover.idx].packet_hit(packet)){
						if(!cover.cross && cover.idx < x){
							allweight--;   //crossed才加
						}else if(cover.cross && cover.idx < x && x != cover.idx){
							addp.push_back(pac);
						}
						break;
					}
				}
			}
			file.close();
			ofstream out;
			out.open("add packets");
			for(auto paci : addp) out<<paci<<endl;
			out.close();
			return allweight;
		}

		void obtain_all_buckets(bucket *root, int idx){
			if(root->sonList.empty()) candi_buckets.push_back(cab::cache_bucket(root, idx));
			else for(auto bson  : root->sonList) obtain_all_buckets(bson, idx);
		}

		//test function
		void test_splited_rule(int splited_max){
			for(int i = 0; i < rList->list.size(); i++)
			if(rList->mncross[i].size() + rList->mnested[i].size() > splited_max){
				cout<<i<<" "<<rList->list[i].weight<<" "<<rList->children[i].size()<<" "<<rList->list[i].weight / rList->children[i].size()<<endl;
			}
		}
	};
}
#endif

