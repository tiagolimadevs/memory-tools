#include "parser.h"
#include "tutil.h"
#include <iostream>

using std::cout, std::endl;

Parser::Parser(int argc, char *argv[])
{
	mAllOk = true;
	mIsPid = false;
	parse(argc, argv);
	checkRequired();
}


void Parser::parse(int argc, char *argv[]) {
	mName = argv[0];
	
	if (argc <= 2) {
		printUseMode();
		mAllOk = false;
		return;
	}

	for (int x = 1; x < argc && mAllOk; x++) {
		if (string(argv[x]).find('=') != string::npos) {
			vector<string> s = explode(argv[x], '=');
			set(s[0], s[1]);
		} else {
			set(argv[x], argv[x+1]);
			++x;
		}

	}

}

void Parser::set(string key, string value) {
	if (key.find("app") != string::npos)
		mApp = value;
	 else if (key.find("filein") != string::npos)
		mFileIn = value;
	else if (key.find("fileout") != string::npos)
		mFileOut = value;
	else if (key.find("saveseed") != string::npos)
		mSeedOutput = value;
	else if (key.find("fileignore") != string::npos)
		mFileIgnore = value;
	else if (key.find("debugout") != string::npos)
		mDebugOut = value;
	else if (key.find("format") != string::npos) 
		mFormat = value;
	else if (key.find("type") != string::npos)
		mType = value;
	else if (key.find("pid") != string::npos) {
		mPid = strtol(value.c_str(), NULL, 10);
		mIsPid = true;
	} else if (key.find("help") != string::npos) {
		printUseMode();
		mAllOk = false;
	} else if (key.find("badchar") != string::npos) {
		char ch;
		sscanf(value.c_str(), "%x", &ch);
		mBadChars.push_back(ch);
	} else {
		mAllOk = false;
		printf (("unknown type "+key+"\n").c_str());
		printUseMode();
	}

}


void Parser::checkRequired() {
	if (mApp.empty() && not isPid())
		printf ("app or pid is required\n");
	else if (mFileIn.empty())
		printf ("file in is required\n");
	else if (mType.empty())
		printf ("type is required\n");
	else if (mFormat.empty() && mType == "scan")
		printf ("format is required on scan\n");
	else return;
	printUseMode();
	mAllOk = false;
}


void Parser::printUseMode() {
	cout << "execute: " << mName << " options" << endl << endl
		<< "options:" << endl
		<< "\t--app name target             [required or pid]"
		<< endl << endl
		<< "\t--pid name target             [required or app]"
		<< endl << endl
		<< "\t--filein file input           [required]" 
		<< endl << endl
		<< "\t--fileout file output         [default=stdout]" 
		<< endl << endl
		<< "\t--saveseed file seed output   [not required]"
		<< endl << endl
		<< "\t--fileignore file ignore      [not required]"
		<< endl << endl
		<< "\t--debugout file debug out     [not required]"
		<< endl << endl
		<< "\t--format int|float|string     [required only scan]" 
		<< endl << endl
		<< "\t--type scan|get|put|monitor   [required]"
		<< endl << endl
		<< "\t--badchar badchar exp: 0xf3   [not required]" 
		<< endl << endl;

}

