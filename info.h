#ifndef __INFO__
#define __INFO__
#define _FILE_OFFSET_BITS 64

#include <string>
#include <map>
#include <vector>
#include <fstream>

using std::string, std::map, std::vector, std::fstream;

class Info {

public:
	Info();

	void loadFromPid(int pid);
	void setPid(int pid);
	int pidof(string name);
	void setFileMem(int fs);
	int fileMem();

	string nameOf(unsigned long addr);
	vector<vector<unsigned long>> mapMemory();
	vector<vector<unsigned long>> mapMemoryFilter(vector<string> filters);

	
	long int liMemoryAt(unsigned long memory);
	int iMemoryAt(unsigned long memory);
	unsigned long ulMemoryAt(unsigned long memory);
	float fMemoryAt(unsigned long memory);
	string sMemoryAt(unsigned long memory,
			 unsigned long maxMemory=NULL);	

	void setSMemoryAt(unsigned long memory, string s);
	void setBadChars(vector<char> badChars);
	bool isEndAt(unsigned long memory);
	void setIgnore(string ignore);
	void setDebugOutput(string out);
	unsigned long endLastPoint();
	bool isValidChar(char ch);
	unsigned long seed();
	void debug();


private:
	void build();
	void configure();

	vector<vector<string>> msplit(FILE *fp);
	void mappend(vector<string> vs);
	bool isBadChar(char ch);
	void freshSeek();

	template<typename T>
	T memoryAt(unsigned long memory); 

	map<vector<unsigned long>, string> mData;
	vector<char> mBadChars;
	unsigned long mLastPoint;
	int mFileMem;
	int mPid;

	bool mOnDebug;
	fstream mFsDebugOut;

};

#endif
