#include "memory.h"
#include "tutil.h"
#include "colors.h"

#include <unistd.h>
#include <iostream>
using std::cout, std::cerr, std::endl;

Memory::Memory() {
	build();
	configure();

}


void Memory::build() {
	mInfo = new Info();
}

void Memory::configure() {

}


void Memory::load(Parser parser) {
	mParser = parser;
	mPid = mParser.isPid() ? mParser.pid() : mInfo->pidof(mParser.app());
	mInfo->loadFromPid(mPid);
	mInfo->setPid(mPid);
	mInfo->setFileMem(mInfo->fileMem());
	mInfo->setBadChars(mParser.badchars());
	
	printf ("pid: %d\n", mPid);
	openFsIn();
	openFsOut();

	parseIgnore();
	parseDebug();
	storeSeed();
	parseType();
//	mInfo->debug();

	closeFsIn();
	closeFsOut();
}

void Memory::parseType() { 
	string type = mParser.type();
	if (type == "get")
		get();
	else if (type == "put")
		put();
	else if (type == "scan")
		scan();
	else if (type == "monitor")
		monitor();
	else
		cerr << "unknown type!!!" << endl;
}

void Memory::parseFormat() {
	string format = mParser.format();
	if (format == "int")
		scanInt();
	else if (format == "float")
		scanFloat();
	else if (format == "string")
		scanString();
	else
		cerr << "unknown format!!!" << endl;
}


void Memory::parseIgnore() {
	string ignore = mParser.fileignore();
	if (not ignore.empty()) {
		fstream fs;
		fs.open(ignore);
		if (fs.is_open()) {
			string line;
			char ch;
			while (not fs.eof()) {
				line.clear();
				while ((ch=fs.get())!='\n' 
					&& not fs.eof()) 
					line.push_back(ch);
				if (not line.empty())
					mInfo->setIgnore(line);
			}
			fs.close();
		}
	}
}


void Memory::parseDebug() {
	string debugfn = mParser.debugoutput();
	if (not debugfn.empty())
		mInfo->setDebugOutput(debugfn);

}

void Memory::openFsOut() {
	outToFile = false;
	string fon = mParser.fileout();
	if (not fon.empty()) {
		mFsFileOut.open(fon, fstream::out);
		if (not mFsFileOut.is_open()) {
			perror("can't open file output\n");
			return;
		}
		outToFile = true;
	}
}

void Memory::openFsIn() {
	mFsFileIn.open(mParser.filein(), fstream::in);
	if (not mFsFileIn.is_open())
		perror("can't open file in!!!\n");
}

void Memory::printOnOut(string s) {
	if (outToFile) {
		mFsFileOut.write(s.c_str(), s.length());
		mFsFileOut.flush();
	} else {
		printf (s.c_str());
		fflush(stdout);
	}
}


void Memory::closeFsIn() {
	mFsFileIn.close();
}

void Memory::closeFsOut() {
	mFsFileOut.close();
}

vector<string> Memory::catFile() {
	mFsFileIn.seekp(0, mFsFileIn.beg);
	vector<string> mems;
	string str;
	char ch;
	while (not mFsFileIn.eof()) {
		while ((ch=mFsFileIn.get())!='\n' 
			&& not mFsFileIn.eof())
			str.push_back(ch);
		if (not str.empty())
			mems.push_back(str);
		str.clear();
	}
	return mems;
}


void Memory::get() {
	vector<string> ns = catFile();
	
	if (not fixSeed(ns))
		return;
	char out[1024];
	string s;
	for (string s : ns) {
		unsigned long ulv = strtoul(s.c_str(), NULL, 16);
		//if (not mInfo->isEndAt(ulv)) {
			s = mInfo->sMemoryAt(ulv);
			sprintf (out, "%lx[%s]: int='%ld' str='", ulv,
				mInfo->nameOf(ulv).c_str(),
				mInfo->liMemoryAt(ulv));
			printOnOut(out+s+"'\n");
		//}
	}

}



void Memory::put() {
	vector<string> ns = catFile();

	if (not fixSeed(ns))
		return;
	for (string s : ns) {
		vector<string> vs = explode(s, '=');
		unsigned long ulv = strtoul(vs[0].c_str(), NULL, 16);
		mInfo->setSMemoryAt(ulv, vs[1]);
	}

}


void Memory::scan() {
	parseFormat();
}


void Memory::scanInt() {
	vector<string> filters = catFile();
	vector<vector<unsigned long>> addrs;
	addrs = mInfo->mapMemoryFilter(filters);
	unsigned long beg, end;
	char out[10240];
	for (vector<unsigned long> addr : addrs) {
		beg = addr[0];
		end = addr[1];
		
		printf ("scaning %x-%x [%s]\n", beg, end, 
				mInfo->nameOf(beg).c_str());
		fflush(stdout);
		while (beg < end) {
			if (not mInfo->isEndAt(beg)) {
				sprintf (out, "%x: %ld\n", beg, 
						mInfo->liMemoryAt(beg));
				beg = mInfo->endLastPoint();
				printOnOut(out);
			}
			while (beg < end 
			&& mInfo->isEndAt(beg)) 
			beg++;
		}
	}
}


void Memory::scanFloat() {
	vector<string> filters = catFile();
	vector<vector<unsigned long>> addrs;
	addrs = mInfo->mapMemoryFilter(filters);
	unsigned long beg, end;
	char out[10240];
	for (vector<unsigned long> addr : addrs) {
		beg = addr[0];
		end = addr[1];
		
		printf ("scaning %x-%x [%s]\n", beg, end,
				mInfo->nameOf(beg).c_str());
		fflush(stdout);
		while (beg < end) {
			if (not mInfo->isEndAt(beg)) {
				sprintf (out, "%x: %f\n", beg,
					mInfo->fMemoryAt(beg));
				beg = mInfo->endLastPoint();
				printOnOut(out);
			}
			while (beg < end 
			&& mInfo->isEndAt(beg))
			beg++;
		}
	}
}



void Memory::scanString() {
	vector<string> filters = catFile();
	vector<vector<unsigned long>> addrs;
	addrs = mInfo->mapMemoryFilter(filters);
	unsigned long beg, end;
	string s;
	char out[10240];
	for (vector<unsigned long> addr : addrs) {
		beg = addr[0];
		end = addr[1];
		
		printf ("scaning %x-%x [%s]\n", beg, end,
				mInfo->nameOf(beg).c_str());
		fflush(stdout);
		while (beg < end) {
			if (not mInfo->isEndAt(beg)) {
				s = mInfo->sMemoryAt(beg, end);
				sprintf (out, "%x: ", beg);
				printOnOut((out+s+"\n"));
				beg = mInfo->endLastPoint();
			}
			while (beg < end && mInfo->isEndAt(beg)) beg++;
		}
	}

}

void Memory::monitor() {
	vector<string> addrs = catFile();
	
	if (not fixSeed(addrs))
		return;

	setRangeMonitor(addrs);
	char s[2048];
	while (true) {
		system("clear");
		map<string, int> mv;
		for (string addr : addrs) {
			unsigned long ula = strtoul(addr.c_str(), NULL, 16);
			string sma = mInfo->sMemoryAt(ula);
                        unsigned long lgua = mInfo->liMemoryAt(ula);
			sprintf (s, "%s%lx%s: str='%s' int='%ld'", 
				CYAN, ula, DEFAULT, sma.c_str(), lgua);
			int hul = hasULChanged(ula, lgua);
			mv[s] = hul > 0 ? hul : hasSChanged(ula, sma);
		}
		printSort(mv);
		sleep(1);
	}
}


bool Memory::getSeed(vector<string> v, unsigned long &seed) {
	
	if (v[0].find("seed") != string::npos) {
		seed = strtoul(explode(v[0], '=')[1].c_str(), NULL, 16);
		return true; 
	}
	return false;
}


unsigned long Memory::getCurrentSeed() {
	return mInfo->seed();
}


bool Memory::fixSeed(vector<string> &vec) {
	unsigned long oldSeed, currentSeed;
	if (not getSeed(vec, oldSeed)) {
		printf ("seed not found!!!");
		return false;
	}

	currentSeed = getCurrentSeed();
	
	vec.erase(vec.begin());
	vector<string> anotherVec;
	unsigned long toUl, anotherUl;
	char toStr[50];
	printf ("oldseed=%lu newseed=%lu\n", oldSeed, currentSeed);
	for (string s : vec) {
		toUl = strtoul(s.c_str(), NULL, 16);
		printf ("old=%lu ", toUl);
		toUl -= oldSeed;
		toUl += currentSeed;
		printf ("new=%lu\n", toUl);
		sprintf (toStr, "%x", toUl);
		anotherVec.push_back(toStr);
	}

	vec.swap(anotherVec);
	return true;
}	


void Memory::storeSeed() {
	string seedfn = mParser.seedOutput();
	fstream fs;
	fs.open(seedfn, fstream::out);
	if (not fs.is_open()) return;

	unsigned long seed = mInfo->seed();
	char out[1024];
	sprintf (out, "seed=%x\n", seed);
	fs.write(out, strlen(out));
	fs.flush();
	fs.close();
}


void Memory::printSort(map<string, int> value) {
	map<string, int>::iterator it, itBg;
	while (value.size() > 0) {
		itBg = value.begin();	
		for (it = itBg; it != value.end(); it++) {
			if (itBg->second < it->second) {
				itBg = it;
			}
		}
		
		value.erase(itBg);
		printf (itBg->first.c_str());
		printf ("%s %d\n", (itBg->second > 0 ? RED : DEFAULT), itBg->second);	
		fflush(stdout);
	}
}


void Memory::setRangeMonitor(vector<string> range) {
	unsigned long lv;
	for (string key : range) {
		lv = strtoul(key.c_str(), NULL, 16);
		mRangeLong[lv] = NULL;
		mRangeString[lv] = string();
		mLenChanged[lv] = 0;
	}
}

int Memory::hasULChanged(unsigned long key, unsigned long value) {
	if (mRangeLong[key] == (unsigned long)NULL) {
		 mRangeLong[key] = value;
		return 0;
	}
	if (mRangeLong[key] != value) {
		mRangeLong[key] = value;
		mLenChanged[key]++;
	}
	return mLenChanged[key];
}


int Memory::hasSChanged(unsigned long key, string value) {	
	if (mRangeString[key] == string()) {
		 mRangeString[key] = value;
		return 0;
	}
	if (mRangeString[key] != value) {
		mRangeString[key] = value;
		mLenChanged[key]++;
	}
	return mLenChanged[key];
}


