#include "AllSolution.h"

//CacheFlow
void Solutions::CacheFlow(int memory, rule_list * rL, int weight, string trace){
	cout<<endl<<"					CacheFlow :"<<endl;
	int cache_weight = 0;
	cacheflow::mixed_set cache_flow(memory, rL);
	cache_flow.cal_mixed_set();
	cache_weight = cache_flow.cal_all_weight(trace);
	cache_flow.cal_weights_diff(trace); //test cal weight
	cout<<"rules table:  "<<cache_flow.cache_rules.size()<<'	'<<"cover-set table: "<<cache_flow.cover_rules.size()<<endl;
	cout<<"rules table: "<<endl;
	for(auto &x : cache_flow.cache_rules){
		cout<<x<<" ";
	}
	cout<<endl<<"cover-set table: "<<endl;
	for(auto &x : cache_flow.cover_rules) cout<<x<<" ";
	cout<<endl<<"cache weight: "<<cache_weight<<"    radio: "<<double(cache_weight) /weight<<endl;
}

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
void Solutions::CAB(int memory, rule_list * rL, int weight, string trace, int splited_keep){
	cout<<endl<<"					CAB :"<<endl;
	int cache_weight = 0;
	bucket_tree bTree(*rL, splited_keep, false, 0);
    /*
	bTree.pre_alloc();
	set_bucket_hit2(bTree.root);
	bTree.merge_bucket_CPLX_test(bTree.root);
    */
	bTree.obtain_bucket_weight(trace);
	//tree depth
	bTree.cal_tree_depth(bTree.root);
	cout<<"tree depth:"<<'\t'<<bTree.tree_depth<<endl;
	cab::pro_cache_set cab_cache(&bTree, memory);
	cab_cache.cal_cache_set();
	cout<<"buckets table:  "<<cab_cache.cache_buckets.size()<<'	'<<"rules table: "<<cab_cache.cache_rules.size()<<endl;
	cout<<"rules table: "<<endl;
	int allrelated_weight = 0;
	for(auto &x : cab_cache.cache_rules){
		allrelated_weight += rL->list[x].weight;
		cout<<x<<" ";
	}
	for(auto x : cab_cache.cache_buckets) cache_weight += x.weight;
	cout<<endl<<"cache weight: "<<cache_weight<<"    radio: "<<double(cache_weight) /weight<<"   all relative weight: "<<allrelated_weight<<endl;

	//test fault
	for(auto bucket: cab_cache.cache_buckets){
		int weight = 0;
		for(auto x : bucket.cBucket->related_rules) weight += rL->list[x].weight;
		if(weight <bucket.weight) cout<<"fault :" <<weight<<"	"<<bucket.weight<<"	"<<endl;
	}

}

//myCache
void Solutions::myCache(int memory, rule_list * rL, int weight, string trace, bool splited, int splited_max, int splited_keep){
	cout<<endl<<"					MyCache :"<<endl;
	int cache_weight = 0;
	mycache::mixed_set my_cache(memory, rL, splited, trace, splited_max, splited_keep);
	my_cache.cal_mixed_set(splited);   //计算proactive_cache
	cache_weight = my_cache.cal_all_weight(trace);//计算cover_set table中可表征的流量
	//my_cache.test_splited_rule(memory); //test splited rule
	cout<<"rules table:  "<<my_cache.cache_rules.size()<<endl;
	cout<<"buckets table:  "<<my_cache.cache_buckets.size()<<endl;
	cout<<"cover-set table: "<<my_cache.cover_rules.size()<<endl;
	cout<<"rules table: "<<endl;
	for(auto &x : my_cache.cache_rules){
		cout<<x<<" ";
	}
	cout<<endl<<"buckets table crossed: "<<endl;
	for(auto &x : my_cache.cache_buckets){
			cout<<x.idx<<" ";
		}
	cout<<endl<<"cover-set table crossed: "<<endl;
	int num_crossed = 0;
	ofstream out;
	out.open("crossed rules");
	for(auto &x: my_cache.cover_rules){
		if(x.cross){
			cout<<x.idx<<' ';
			out<<x.idx<<'\t'<<rL->children[x.idx].size()<<'\t'<<rL->list[x.idx].weight<<endl;
			num_crossed++;
		}
	}
	out.close();
	cout<<endl<<"nums of cover-set table crossed: "<<'\t'<<num_crossed<<endl;
	cout<<endl<<"cache weight: "<<cache_weight<<"    radio: "<<(double)cache_weight/weight<<endl;
	cout<<endl<<"cache weight less:	"<<my_cache.allweight_less<<"	radio"<<(double)my_cache.allweight_less/weight<<endl;
}
