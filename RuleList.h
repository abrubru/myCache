#ifndef RULELIST_H
#define RULELIST_H

#include "stdafx.h"
#include "Address.hpp"
#include "Rule.hpp"
#include <unordered_map>

#include <boost/graph/adjacency_list.hpp>


class rule_list {
public:
	int depth;                                                             //dag的深度
    std::vector<p_rule> list;                                       //ruleset

    std::vector<uint32_t> node;
    std::vector<std::set<uint32_t> > mnested;         //正向边  parent(low)--> child(high)
    std::vector<std::set<uint32_t> > mncross;
    std::vector<std::set<uint32_t> > rev_mnested;         //反向边  child(high)--> parent(low)
    std::vector<std::set<uint32_t> > rev_mncross;

    std::unordered_map <uint32_t, std::set<uint32_t> > dep_map;
    std::unordered_map <uint32_t, std::set<uint32_t> > children;
    std::unordered_map <uint32_t, std::set<uint32_t> > parents;
    std::vector<size_t> occupancy;

    rule_list();
    rule_list(std::string &, bool = false);

    // for dependency set

    // for micro set
    r_rule get_micro_rule (const addr_5tup &);
    int linear_search(const addr_5tup &);

    // for covering set#
private:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> depDAG; // edgelist, vertexlist, is-directed
    typedef boost::graph_traits<depDAG>::vertex_descriptor vertex_descriptor; //as its name
    typedef boost::graph_traits<depDAG>::vertex_iterator vertex_iterator;  //as its name 顶点
    typedef boost::graph_traits<depDAG>::adjacency_iterator adjacency_iterator;// 临接点
    depDAG depDag;

public:
    void createDAG();
    void obtain_dep();  //duck
    void obtain_cover();  //duck
    int obtain_weight(string trace); //duck
    int test_trace(string trace); //duck
    void trace_shape(string trace);//duck
    void test_large_rule(int);
    void get_depth();
    int get_each_depth(int, vector<int>&);

    void clearHitFlag();

    void rule_dep_analysis();
    void print(const std::string &);
    void test_rulelist_info(string trace = "");

    //have node idea DEC.17
    void select_optimal_rules(int total_memory);
};
#endif
