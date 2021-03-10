#ifndef __TUTIL_H__
#define __TUTIL_H__

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;


template<class T1, class T2>
vector<T1> allKeys(map<T1, T2> m) {
	vector<T1> ret;
	for (auto it = m.begin(); it != m.end(); it++)
		ret.push_back(it->first);
	return ret;
}


template<class T1, class T2>
vector<T1> allValues(map<T1, T2> m) {
	vector<T2> ret;
	for (auto it = m.begin(); it != m.end(); it++)
		ret.push_back(it->second);
	return ret;
}


template<typename T1>
vector<string> explode(string s1, T1 ch) {
	vector<string> ret;
	string str;
	s1.push_back(ch);
	for (char ch2 : s1) {
		if (ch2 == ch) {
			if (not str.empty())
				ret.push_back(str);
			str.clear();
		} else
			str.push_back(ch2);
	}
	return ret;
}


#endif
