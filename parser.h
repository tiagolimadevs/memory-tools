#ifndef __PARSER__
#define __PARSER__

#include <string>
#include <vector>

using std::string, std::vector;

class Parser {

public:
	Parser() {}
	Parser(int argc, char *argv[]);

	string name() { return mName; }
	string app() { return mApp; }
	string filein() { return mFileIn; }
	string fileout() { return mFileOut; }
	string fileignore() { return mFileIgnore; }
	string seedOutput() { return mSeedOutput; }
	string debugoutput() { return mDebugOut; }
	string format() { return mFormat; }
	string type() { return mType; }
	vector<char> badchars() { return mBadChars; }
	bool allOk() { return mAllOk; }
	bool isPid() { return mIsPid; }
	int pid() { return mPid; }

private:
	void parse(int argc, char *argv[]);
	void set(string key, string value);
	void checkRequired();
	void printUseMode();

	string mName, mFormat,
	       mApp, mFileIn, 
	       mFileIgnore, 
	       mFileOut, mDebugOut,
	       mSeedOutput, mType;
	vector<char> mBadChars;
	bool mIsPid;
	bool mAllOk;
	int mPid;

};

#endif
