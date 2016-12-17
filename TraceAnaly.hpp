#ifndef TRACEANALY_H
#define TRACEANALY_H

#include "RuleList.h"
#include <map>

class trace{
private:
	rule_list *rL;
public:
	trace(rule_list *r): rL(r){}
	void trace_analyze_flow(string trace) {
		int num1, num5, num10, num20, num50, num100, num200, num500, num1000,
				num1001;
		int snum1, snum5, snum10, snum20, snum50, snum100, snum200, snum500,
				snum1000, snum1001;
		num1 = num5 = num10 = num20 = num50 = num100 = num200 = num500 =
				num1000 = num1001 = 0;
		snum1 = snum5 = snum10 = snum20 = snum50 = snum100 = snum200 = snum500 =
				snum1000 = snum1001 = 0;
		std::map<addr_5tup, int> mp;
		std::ifstream file;
		file.open(trace.c_str());
		string sLine = "";
		getline(file, sLine);
		while (!file.eof()) {
			addr_5tup packet(sLine, false);
			mp[packet]++;
			getline(file, sLine);
		}
		for (auto flow : mp) {
			int w = flow.second;
			if (w < 2) {
				num1++;
				snum1 += w;
			} else if (w < 5) {
				num5++;
				snum5 += 2;
			} else if (w < 10) {
				num10++;
				snum10 += w;
			} else if (w < 20) {
				num20++;
				snum20 += w;
			} else if (w < 50) {
				num50++;
				snum50 += w;
			} else if (w < 100) {
				num100++;
				snum100 += w;
			} else if (w < 200) {
				num200++;
				snum200 += w;
			} else if (w < 500) {
				num500++;
				snum500 += w;
			} else if (w < 1000) {
				num1000++;
				snum1000 += w;
			} else {
				num1001++;
				snum1001 += w;
			}
		}
		cout << "trace characteristics flow:  " << endl;
		cout << "[0, 2)" << '\t' << num1 << '\t' << snum1 << endl;
		cout << "[2, 5)" << '\t' << num5 << "\t" << snum5 << endl;
		cout << "[5, 10)" << '\t' << num10 << "\t" << snum10 << endl;
		cout << "[10, 20)" << '\t' << num20 << "\t" << snum20 << endl;
		cout << "[20, 50)" << '\t' << num50 << "\t" << snum50 << endl;
		cout << "[50, 100)" << '\t' << num100 << "\t" << snum100 << endl;
		cout << "[100, 200)" << '\t' << num200 << "\t" << snum200 << endl;
		cout << "[200, 500)" << '\t' << num500 << "\t" << snum500 << endl;
		cout << "[500, 1000)" << '\t' << num1000 << "\t" << snum1000 << endl;
		cout << "[1000, ~)" << '\t' << num1001 << "\t" << snum1001 << endl;
	}
	void trace_analyze_rule(){
		int size = rL->list.size();
		int num0, num1, num5, num10, num20, num50, num100, num200, num500, num1000, num1001;
		int snum0, snum1, snum5, snum10, snum20, snum50, snum100, snum200, snum500, snum1000, snum1001;
		int wnum0, wnum1, wnum5, wnum10, wnum20, wnum50, wnum100, wnum200, wnum500, wnum1000, wnum1001;
		num0 = num1 = num5 = num10 = num20 = num50 = num100 = num200 = num500 = num1000 = num1001 = 0;
		snum0 = snum1 = snum5 = snum10 = snum20 = snum50 = snum100 = snum200 = snum500 = snum1000 = snum1001 = 0;
		wnum0 = wnum1 = wnum5 = wnum10 = wnum20 = wnum50 = wnum100 = wnum200 = wnum500 = wnum1000 = wnum1001 = 0;
		for(int i = 0; i < size; i++){
			int w = rL->children[i].size();
			int we = rL->list[i].weight;
			if(w == 0){
				num0++;
				snum0 += w;
				wnum0 += we;
			}else if(w< 2){
				num1++;
				snum1 += w;
				wnum1 += we;
			}else if( w < 5){
				num5++;
				snum5 += 2;
				wnum5 += we;
			}else if( w < 10){
				num10++;
				snum10 += w;
				wnum10 += we;
			}else if( w < 20){
				num20++;
				snum20 += w;
				wnum20 += we;
			}else if( w < 50){
				num50++;
				snum50 += w;
				wnum50 += we;
			}else if( w < 100){
				num100++;
				snum100 += w;
				wnum100 += we;
			}else if( w < 200){
				num200++;
				snum200 += w;
				wnum200 += we;
			}else if( w < 500){
				num500++;
				snum500 += w;
				wnum500 += we;
			}else if( w < 1000){
				num1000++;
				snum1000 += w;
				wnum1000 += we;
			}else{
				num1001++;
				snum1001 += w;
				wnum1001 += we;
			}
		}
		cout<<"trace characteristics rule:  "<<endl;
		cout<<"[0, 1)"<<'\t'<<num0<<'\t'<<snum0<<'\t'<<wnum0<<endl;
		cout<<"[1, 2)"<<'\t'<<num1<<'\t'<<snum1<<'\t'<<wnum1<<endl;
		cout<<"[2, 5)"<<'\t'<<num5<<"\t"<<snum5<<'\t'<<wnum5<<endl;
		cout<<"[5, 10)"<<'\t'<<num10<<"\t"<<snum10<<'\t'<<wnum10<<endl;
		cout<<"[10, 20)"<<'\t'<<num20<<"\t"<<snum20<<'\t'<<wnum20<<endl;
		cout<<"[20, 50)"<<'\t'<<num50<<"\t"<<snum50<<'\t'<<wnum50<<endl;
		cout<<"[50, 100)"<<'\t'<<num100<<"\t"<<snum100<<'\t'<<wnum100<<endl;
		cout<<"[100, 200)"<<'\t'<<num200<<"\t"<<snum200<<'\t'<<wnum200<<endl;
		cout<<"[200, 500)"<<'\t'<<num500<<"\t"<<snum500<<'\t'<<wnum500<<endl;
		cout<<"[500, 1000)"<<'\t'<<num1000<<"\t"<<snum1000<<'\t'<<wnum1000<<endl;
		cout<<"[1000, ~)"<<'\t'<<num1001<<"\t"<<snum1001<<'\t'<<wnum1001<<endl;
	}
};
#endif
