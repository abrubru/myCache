#include "AllSolution.h"

//CacheFlow
void Solutions::CacheFlow(int memory, rule_list * rL, int weight){
	cout<<endl<<"					CacheFlow :"<<endl;
	int cache_weight = 0;
	cacheflow::mixed_set cache_flow(memory, rL);
	cache_flow.cal_mixed_set();
	cout<<"rules table:  "<<cache_flow.cache_rules.size()<<'	'<<"cover-set table: "<<cache_flow.cover_rules.size()<<endl;
	cout<<"rules table: "<<endl;
	for(auto &x : cache_flow.cache_rules){
		cout<<x<<" ";
		cache_weight += rL->list[x].weight;
	}
	cout<<endl<<"cover-set table: "<<endl;
	for(auto &x : cache_flow.cover_rules) cout<<x<<" ";
	cout<<endl<<"cache weight: "<<cache_weight<<"    radio: "<<double(cache_weight) /weight<<endl;
}

void Solutions::CAB(int memory, rule_list * rL, int weight){
	cout<<endl<<"					CAB :"<<endl;
	int cache_weight = 0;

}

//myCache
void Solutions::myCache(int memory, rule_list * rL, int weight){
	cout<<endl<<"					MyCache :"<<endl;
	int cache_weight = 0;
	mycache::mixed_set my_cache(memory, rL);
	my_cache.cal_mixed_set();
	cout<<"rules table:  "<<my_cache.cache_rules.size()<<endl;
	cout<<"cover-set nested table: "<<my_cache.cover_rules_nested.size()<<'	'<<"cover-set crossed table: "<<my_cache.cover_rules_crossed.size()<<endl;
	cout<<"rules table: "<<endl;
	for(auto &x : my_cache.cache_rules){
		cout<<x<<" ";
		cache_weight += rL->list[x].weight;
	}
	cout<<endl<<"cover-set nested table: "<<endl;
	for(auto &x : my_cache.cover_rules_nested) cout<<x<<" ";
	cout<<endl<<"cover-set crossed table: "<<endl;
	for(auto &x : my_cache.cover_rules_crossed) cout<<x<<" ";
	cout<<endl<<"cache weight: "<<cache_weight<<"    radio: "<<double(cache_weight) /weight<<endl;
}
