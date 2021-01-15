/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides a simple example on how to use the trained classifiers
/// within an analysis module
/// - Project   : TMVA - a Root-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Exectuable: TMVAClassificationApplication
///
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

using namespace TMVA;

void copy_vals(float * fx, float & flen, int * x, int len)
{
	for (int i = 0; i < 6; ++i) {
		fx[i] = x[i];
	}
	flen = len;
}
		
void get_numbers(int * cnt, TTree * t, int * n, float * r)
{
	n[0] = cnt[0];
	n[1] = cnt[1];
	n[2] = cnt[2];
	n[3] = t->GetEntries();
	r[0] = 100.*n[0]/n[3];
	r[1] = 100.*n[1]/n[3];
	r[2] = 100.*n[2]/n[3];
}

void app( TString myMethodList = "" )
{

	//---------------------------------------------------------------
	// This loads the library
	TMVA::Tools::Instance();

	std::cout << std::endl;
	std::cout << "==> Start TMVAClassificationApplication" << std::endl;

	// Create the Reader object

	TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

	// Create a set of variables and declare them to the reader
	// - the variable names MUST corresponds in name and type to those given in the weight file(s) used
	float fx[6];
	float flen;
	reader->AddVariable("x[0]", fx);
	reader->AddVariable("x[1]", fx+1);
	reader->AddVariable("x[2]", fx+2);
	reader->AddVariable("x[3]", fx+3);
	reader->AddVariable("x[4]", fx+4);
	reader->AddVariable("x[5]", fx+5);
	reader->AddVariable("len",  &flen);
   
	TFile * f1 = TFile::Open("../prepare_data/fake_data_test/jpverb.root");
	TFile * f2 = TFile::Open("../prepare_data/real_data/jpverb.root");
	TTree * t1a = (TTree *)f1->Get("type1_alt");
	TTree * t1b = (TTree *)f1->Get("type2_alt");
	TTree * t2a = (TTree *)f2->Get("type1_alt");
	TTree * t2b = (TTree *)f2->Get("type2_alt");
	int x[6], len;
	t1a->SetBranchAddress("x",   x);
	t1a->SetBranchAddress("len", &len);
	t1b->SetBranchAddress("x",   x);
	t1b->SetBranchAddress("len", &len);
	t2a->SetBranchAddress("x",   x);
	t2a->SetBranchAddress("len", &len);
	t2b->SetBranchAddress("x",   x);
	t2b->SetBranchAddress("len", &len);

	TString dir    = "dataset/weights/";
	TString prefix = "TMVAClassification";

	// Book method(s)
	vector<TString> methods {"Likelihood", "KNN", "MLP", "MLP_L2Reg", "DNN_ReLU", "DNN_ReLU_mlp", "BDT", "BDTG"};
	for (auto method: methods) {
		TString weightfile = Form("dataset/weights/jpvc_%s.weights.xml", method.Data());
		reader->BookMVA(method, weightfile);
	}
	
	map<TString, double> cut_sig;
	map<TString, double> cut_tot;
	map<TString, double> cut_sqr;
	ifstream ifs("cuts.dat");
	TString method;
	double c;
	while (ifs >> method) {
		ifs >> c;
		cut_sig[method] = c;
		ifs >> c;
		cut_tot[method] = c;
		ifs >> c;
		cut_sqr[method] = c;
	}
	
	map<TString, float> score_test, score_real, score_diff;
	for (auto method: methods) {
		int count1a[3] = {0, 0, 0};
		int count1b[3] = {0, 0, 0};
		int count2a[3] = {0, 0, 0};
		int count2b[3] = {0, 0, 0};

		for (int i = 0; i < t1a->GetEntries(); ++i) {
			t1a->GetEntry(i);
			copy_vals(fx, flen, x, len);
			double response = reader->EvaluateMVA(method);
			if (response > cut_sig[method]) ++count1a[0];
			if (response > cut_tot[method]) ++count1a[1];
			if (response > cut_sqr[method]) ++count1a[2];
		}

		for (int i = 0; i < t1b->GetEntries(); ++i) {
			t1b->GetEntry(i);
			copy_vals(fx, flen, x, len);
			double response = reader->EvaluateMVA(method);
			if (response < cut_sig[method]) ++count1b[0];
			if (response < cut_tot[method]) ++count1b[1];
			if (response < cut_sqr[method]) ++count1b[2];
		}

		for (int i = 0; i < t2a->GetEntries(); ++i) {
			t2a->GetEntry(i);
			copy_vals(fx, flen, x, len);
			double response = reader->EvaluateMVA(method);
			if (response > cut_sig[method]) ++count2a[0];
			if (response > cut_tot[method]) ++count2a[1];
			if (response > cut_sqr[method]) ++count2a[2];
		}

		for (int i = 0; i < t2b->GetEntries(); ++i) {
			t2b->GetEntry(i);
			copy_vals(fx, flen, x, len);
			double response = reader->EvaluateMVA(method);
			if (response < cut_sig[method]) ++count2b[0];
			if (response < cut_tot[method]) ++count2b[1];
			if (response < cut_sqr[method]) ++count2b[2];
		}

		const char * format_a = "    type1@fake sample: %12d(%5.1f)%12d(%5.1f)%12d(%5.1f) (%8lld in total)";
		const char * format_b = "    type2@fake sample: %12d(%5.1f)%12d(%5.1f)%12d(%5.1f) (%8lld in total)";

		cout << Form("%9s cut at %12f%12f%12f", method.Data(), cut_sig[method], cut_tot[method], cut_sqr[method]) << endl;
		int n[4];
		float r[3];

		int nc1, nt1;
		float ws1;
		int nc2, nt2;
		float ws2;
		
		get_numbers(count1a, t1a, n, r);
		cout << Form(format_a, n[0], r[0], n[1], r[1], n[2], r[2], n[3]) << endl;
		nc1 = n[1];
		nt1 = n[3];

		get_numbers(count1b, t1b, n, r);
		cout << Form(format_b, n[0], r[0], n[1], r[1], n[2], r[2], n[3]) << endl;
		nc1 += n[1];
		nt1 += n[3];
		ws1 = 1.0*nc1/nt1;
		cout << "    average score: " << 100*ws1 << endl;
		
		get_numbers(count2a, t2a, n, r);
		cout << Form(format_a, n[0], r[0], n[1], r[1], n[2], r[2], n[3]) << endl;
		nc2 = n[1];
		nt2 = n[3];
		
		get_numbers(count2b, t2b, n, r);
		cout << Form(format_b, n[0], r[0], n[1], r[1], n[2], r[2], n[3]) << endl;
		nc2 += n[1];
		nt2 += n[3];
		ws2 = 1.0*nc2/nt2;
		cout << "    average score: " << 100*ws2 << endl;
		cout << "    score down by: " << 100*(ws1-ws2) << endl;
	
		score_test[method] = ws1;
		score_real[method] = ws2;
		score_diff[method] = ws1 - ws2;
	
		cout << endl;
	}
	
	ofstream ofs("score.dat");
	for (auto method: methods) {
		ofs << method << " " << score_test[method] << " " << score_real[method] << " " << score_diff[method] << endl;
	}
}

//int main( int argc, char** argv )
//{
//}
