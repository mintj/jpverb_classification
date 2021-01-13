#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

using namespace std;

struct HKSeq6
{
	int code[6];
	float shift[6];
	int len;
};

std::istream & operator>>(std::istream & is, HKSeq6 & word)
{
	static TRandom3 rndm;

	is >> word.code[0] >> word.code[1] >> word.code[2] >> word.code[3] >> word.code[4] >> word.code[5];
	word.len = 0;
	for (int i = 0; i < 6; ++i) {
		word.shift[i] = 0;
		if (word.code[i] != 999) {
			word.shift[i] = rndm.Gaus(0, 0.1);
			++word.len;
		}
	}
	return is;
}

std::ostream & operator<<(std::ostream & os, const HKSeq6 & word)
{
	os << "(len="<< word.len << ")";
	for (int i = 0; i < 6; ++i) {
		os << " " << word.code[i] << "(" << word.shift[i] << ")";
	}
	return os;
}

HKSeq6 complete_at_tail(HKSeq6 & word)
{
	HKSeq6 word_alt;
	for (int i = 0; i < 6; ++i) {
		int j = (i+word.len)%6;
		word_alt.code[i]  = word.code[j];
		word_alt.shift[i] = word.shift[j];
	}
	word_alt.len = word.len;

	return word_alt;
}

HKSeq6 re_encode(HKSeq6 & word, std::map<int, int> & dict)
{
	HKSeq6 word_alt;
	for (int i = 0; i < 6; ++i) {
		int code_new = word.code[i];
		if (dict.find(code_new) != dict.end()) {
			code_new = dict[word.code[i]];
		}
		word_alt.code[i]  = code_new;
		word_alt.shift[i] = word.shift[i];
	}
	word_alt.len = word.len;
	
	return word_alt;
}

std::map<int, int> shuffle_dict(int max = 200)
{
	std::vector<int> pool;
	for (int i = 0; i < max; ++i) {
		pool.push_back(i);
	}
	
	static TRandom3 rndm;
	std::map<int, int> dict;
	for (int i = 0; i < max; ++i) {
		int rest = pool.size();
		int j = (int)(rest*rndm.Rndm());
		dict[i] = pool[j];
		pool[j] = pool[rest-1];
		pool.pop_back();
	}
	return dict;
}

void AddBranch(TTree * t, HKSeq6 & word)
{
	t->Branch("x",     word.code,  "x[6]/I");
	t->Branch("shift", word.shift, "shift[6]/F");
	t->Branch("len",   &word.len,  "len/I");
}

int main(int argc, char ** argv)
{
	const char * ofname = "jpverb.root";
	TFile * outfile = TFile::Open(ofname, "recreate");

	TTree * t1  = new TTree("type1", "");
	TTree * t2  = new TTree("type2", "");
	TTree * t1a = new TTree("type1_alt", "");
	TTree * t2a = new TTree("type2_alt", "");
	TTree * t1b = new TTree("type1_rndm", "");
	TTree * t2b = new TTree("type2_rndm", "");

	HKSeq6 word, word_alt, word_rndm;
	AddBranch(t1,  word);
	AddBranch(t2,  word);
	AddBranch(t1a, word);
	AddBranch(t2a, word);
	AddBranch(t1b, word);
	AddBranch(t2b, word);

	ifstream infile1("type1_clean.encoded");
	ifstream infile2 ("type2.encoded");

	auto dict = shuffle_dict();
	
	while (infile1 >> word) {
		word_alt  = complete_at_tail(word);
		word_rndm = re_encode(word, dict);
		t1 ->Fill();
		t1a->Fill();
		t1b->Fill();
	}
	
	while (infile2 >> word) {
		word_alt  = complete_at_tail(word);
		word_rndm = re_encode(word, dict);
		t2 ->Fill();
		t2a->Fill();
		t2b->Fill();
	}

	outfile->Write();
}
