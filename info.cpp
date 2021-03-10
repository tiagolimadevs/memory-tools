#include "info.h"
#include "tutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <list>
#include <iostream>
using std::cout, std::endl, std::list;

Info::Info()
{
	build();
	configure();
}

void Info::build() {

}

void Info::configure() {
	setBadChars({NULL, '\0'});
	mOnDebug = true;
}
	
vector<vector<string>> Info::msplit(FILE *fp) {
	vector<vector<string>> vsts;
	vector<string> sts;
	string s;
	int lsp;
	char ch;
	while (not feof(fp)) {
		lsp=0;
		while ((ch=fgetc(fp)) != '\n' && not feof(fp)) {
			if (ch == ' ')
				lsp += 1;
			
			switch (lsp) {
				case 0:
					if (ch == '-') {
						sts.push_back(s);
						s.clear();
					} else
						s.push_back(ch);
					break;
				case 1:
				case 2:
				case 3:
				case 4:
					continue;
					break;
				case 5:
					if (ch == ' ') {
						sts.push_back(s);
						s.clear();
					}
					else
						s.push_back(ch);
			}
		}	
		sts.push_back(s);
		s.clear();	
		vsts.push_back(sts);
		sts.clear();
	}
	return vsts;
}

bool Info::isBadChar(char ch) {
	//printf ("checking char %x %d %c\n", ch, ch, ch);
	for (char ch2 : mBadChars) {
		if (ch == ch2)
		   return true;
	}
	return false;
}


void Info::mappend(vector<string> vs) {
	vector<unsigned long> addrs;
	addrs.push_back(strtoul(vs[0].c_str(), NULL, 16));
	addrs.push_back(strtoul(vs[1].c_str(), NULL, 16));
	string name = vs[2];

	mData[addrs] = name;
}


void Info::loadFromPid(int pid) {
	fstream fs;
	char cmd[1024];
	sprintf (cmd, "cat /proc/%d/smaps | grep -E \"([a-f0-9]){8}\" | sed -E 's/(\ ){2,}/\ /g' | grep ' r' ", pid);

	FILE *fp = popen(cmd, "r");

	vector<vector<string>> vsts = msplit(fp);	
	for (vector<string> vts : vsts)
		mappend(vts);
	fs.close();

}

int Info::pidof(string name) {
	int len = 1024;
	char str[len];
	FILE *fp = popen(string("pidof "+name).c_str(), "r");
	fgets(str, len, fp);
	return strtol(str, NULL, 10);
}


void Info::setPid(int pid) {
	mPid = pid;
}


vector<vector<unsigned long>> Info::mapMemory() {	
	return allKeys(mData);
}


vector<vector<unsigned long>> Info::mapMemoryFilter(vector<string> filters) {
	vector<vector<unsigned long>> ret;
	auto v = mapMemory();
	for (vector<unsigned long> s : v) {
		for (string f : filters) {
			if (mData[s].find(f) != string::npos) {
				ret.push_back(s);
				break;
			}
		}
	}
	return static_cast<vector<vector<unsigned long>>>(ret);
}


string Info::nameOf(unsigned long addr) {
	for (auto it = mData.begin(); it != mData.end(); it++) {
		vector<unsigned long> vul = it->first;
		if (addr >= vul[0] && addr < vul[1])
			return it->second;
	}
	return "unknown";
}


int Info::fileMem() {
	int fs;
	char fn[1024];
	sprintf (fn, "/proc/%d/mem", mPid);
	
	if ((fs=open(fn, O_RDWR)) == NULL)
		perror (("can't open "+string(fn)+"!!!").c_str());
	
	return fs;
}


void Info::setFileMem(int fs) {
	mFileMem = fs;
}


unsigned long Info::endLastPoint() {
	return mLastPoint;
}

bool Info::isEndAt(unsigned long memory) {
	lseek(mFileMem, memory, SEEK_SET);
	char ch;
	if (read (mFileMem, &ch, sizeof(char)) < 0)
		return true;
	return isBadChar(ch);
}


bool Info::isValidChar(char ch) {
	return (ch >= 0x20 && ch <= 0x7e);
	   
}


template<typename T>
T Info::memoryAt(unsigned long memory) {
	lseek(mFileMem, memory, SEEK_SET);

	T ret;
	read (mFileMem, &ret, sizeof(T));
	mLastPoint = memory+sizeof(T);

	return ret;
}




long int Info::liMemoryAt(unsigned long memory) {
	return memoryAt<long int>(memory);
}


int Info::iMemoryAt(unsigned long memory) {
	return memoryAt<int>(memory);
}


unsigned long Info::ulMemoryAt(unsigned long memory) {
	return memoryAt<unsigned long>(memory);
}


float Info::fMemoryAt(unsigned long memory) {
	return memoryAt<float>(memory);
}

string Info::sMemoryAt(unsigned long memory, unsigned long maxMemory) {
	lseek(mFileMem, memory, SEEK_SET);
	
	char ch;
	string ret;
	mLastPoint = memory;
	char out[50];
	while (read (mFileMem, &ch, sizeof(char)) >= 0
		&& not isBadChar(ch)
		&& isValidChar(ch)
		&& (memory < maxMemory || maxMemory == NULL)) {
		if (mOnDebug) {
			sprintf (out, "ch=%x\n", ch);
			mFsDebugOut.write(out, 5);
			mFsDebugOut.flush();
		}
		ret.push_back(ch);
		mLastPoint += sizeof(char);
	}
	mLastPoint++;
	return ret;
}


void Info::setSMemoryAt(unsigned long memory, string s) {	
	lseek(mFileMem, memory, SEEK_SET);
	write(mFileMem, s.c_str(), s.length());
}

void Info::setBadChars(vector<char> badChars) {
	for (char badchar : badChars) 
		mBadChars.push_back(badchar);
}

void Info::setIgnore(string ignore) {
	vector<vector<unsigned long>> mems = mapMemoryFilter({ignore});
	for (vector<unsigned long> mem : mems)
		mData.erase(mData.find(mem));
}

void Info::setDebugOutput(string out) {
	mFsDebugOut.open(out, fstream::out);
	mOnDebug = mFsDebugOut.is_open();
}

unsigned long Info::seed() {
	for (auto it = mData.begin(); it != mData.end(); it++)
		if (it->second == "[stack]")
			return it->first[0];	
	return 0;
}


void Info::debug() {
	

}
