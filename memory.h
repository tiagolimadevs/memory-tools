#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "info.h"
#include "parser.h"
#include <fstream>
using std::fstream;

class Memory {

public:
	Memory();
	void load(Parser parser); 

private:
	void build();
	void configure();
	vector<string> catFile();

	void parseType();
	void parseFormat();
	void parseIgnore();
	void parseDebug();
	void openFsOut();
	void openFsIn();
	void closeFsIn();
	void closeFsOut();
	void storeSeed();
	void scan();
	void scanInt();
	void scanFloat();
	void scanString();
	void get();
	void put();
	void monitor(); 
	void printOnOut(string str);
	bool getSeed(vector<string>, unsigned long&);
	bool fixSeed(vector<string>&);
	unsigned long getCurrentSeed();
	void printSort(map<string, int>);
	void setRangeMonitor(vector<string>);
	int hasULChanged(unsigned long, unsigned long);
	int hasSChanged(unsigned long, string);

	int mPid;
	Info *mInfo;
	Parser mParser;
	bool outToFile;
	fstream mFsFileIn, mFsFileOut;
	string mFileIn;



	map<unsigned long, int> mLenChanged;
	map<unsigned long, unsigned long> mRangeLong;
	map<unsigned long, string> mRangeString;

};


#endif
