#ifndef CAB_H
#define CAB_H

#include "Bucket.h"
#include "BucketTree.h"

namespace cab{
	class cache_bucket{
	public:
		uint32_t idx; //所属的rule
		bucket* cBucket; //bucket
		std::set<uint32_t> related_rules; //bucket中的related_rules是vector，不好删除
		int weight;     //命中该bucket的数据包
		int cost;         //该bucket的花费
		bool operator<(const cache_bucket & another) const {
			return (double)weight / cost < (double)another.weight / another.cost;
		}
	public:
		cache_bucket(bucket *b, int id = -1){
			idx = id;
			cBucket = b;
			for(auto x : b->related_rules) related_rules.insert(x);
			weight = b->weight;
			cost = b->related_rules.size() + 1;
		}

	};

	class pro_cache_set{
	public:
		vector<cache_bucket> candi_buckets;
		vector<cache_bucket> cache_buckets;
		std::set<uint32_t> cache_rules;
	private:
		int total_memory;
	public:
		pro_cache_set(bucket_tree *btree, int memory){
			obtain_all_buckets(btree->root);
			total_memory = memory;
		}
		void obtain_all_buckets(bucket *root){
			if(root->sonList.empty()) candi_buckets.push_back(cache_bucket(root));
			else for(auto bson  : root->sonList) obtain_all_buckets(bson);
		}
		void cal_cache_set(){
			cout<<candi_buckets.size()<<'	'<<total_memory<<endl;
			while(!candi_buckets.empty()){
				std::make_heap(candi_buckets.begin(), candi_buckets.end());
				std::pop_heap(candi_buckets.begin(), candi_buckets.end());
				cache_bucket next_cache = candi_buckets.back(); //下一条要缓存的rule
				candi_buckets.pop_back();
				if(cache_buckets.size() + cache_rules.size() + next_cache.cost > total_memory) continue;  //装不下
				//能装下
				//cout<<(double)next_cache.weight/next_cache.cost<<" "<<next_cache.cost<<endl;
				cache_buckets.push_back(next_cache); //bucket进buckets table
				for(auto rule : next_cache.related_rules){
					cache_rules.insert(rule); //rule进rules table
					for(auto iterbucket = candi_buckets.begin(); iterbucket != candi_buckets.end(); iterbucket++){
						auto deletediter = iterbucket->related_rules.find(rule);
						if(deletediter != iterbucket->related_rules.end()){
							iterbucket->related_rules.erase(deletediter); //删除已经放进cache的规则
							iterbucket->cost--;    //cost--
						}
					}
				}
			}
		}
	};
}
#endif
