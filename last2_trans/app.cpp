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
	float fx1, fx2, fx3, fx4, fx5, fx6, flen;
	reader->AddVariable("x5%10", &fx5);
	reader->AddVariable("x6/10", &fx6);
   
	TFile * f1a = TFile::Open("../prepare_data/fake_data/type1_clean.fake_data.dat.tail_len6.root");
	TFile * f2a = TFile::Open("../prepare_data/fake_data/type2.fake_data.dat.tail_len6.root");
	TFile * f1b = TFile::Open("../prepare_data/type1_clean.dat.tail_len6.root");
	TFile * f2b = TFile::Open("../prepare_data/type2.dat.tail_len6.root");
	TTree * t1a = (TTree *)f1a->Get("save");
	TTree * t2a = (TTree *)f2a->Get("save");
	TTree * t1b = (TTree *)f1b->Get("save");
	TTree * t2b = (TTree *)f2b->Get("save");
	int x1, x2, x3, x4, x5, x6, len;
	t1a->SetBranchAddress("x1",  &x1);
	t1a->SetBranchAddress("x2",  &x2);
	t1a->SetBranchAddress("x3",  &x3);
	t1a->SetBranchAddress("x4",  &x4);
	t1a->SetBranchAddress("x5",  &x5);
	t1a->SetBranchAddress("x6",  &x6);
	t1a->SetBranchAddress("len", &len);
	t2a->SetBranchAddress("x1",  &x1);
	t2a->SetBranchAddress("x2",  &x2);
	t2a->SetBranchAddress("x3",  &x3);
	t2a->SetBranchAddress("x4",  &x4);
	t2a->SetBranchAddress("x5",  &x5);
	t2a->SetBranchAddress("x6",  &x6);
	t2a->SetBranchAddress("len", &len);
	t1b->SetBranchAddress("x1",  &x1);
	t1b->SetBranchAddress("x2",  &x2);
	t1b->SetBranchAddress("x3",  &x3);
	t1b->SetBranchAddress("x4",  &x4);
	t1b->SetBranchAddress("x5",  &x5);
	t1b->SetBranchAddress("x6",  &x6);
	t1b->SetBranchAddress("len", &len);
	t2b->SetBranchAddress("x1",  &x1);
	t2b->SetBranchAddress("x2",  &x2);
	t2b->SetBranchAddress("x3",  &x3);
	t2b->SetBranchAddress("x4",  &x4);
	t2b->SetBranchAddress("x5",  &x5);
	t2b->SetBranchAddress("x6",  &x6);
	t2b->SetBranchAddress("len", &len);

	TString dir    = "dataset/weights/";
	TString prefix = "TMVAClassification";

	// Book method(s)
	vector<TString> methods {"Likelihood", "KNN", "MLP", "MLP_L2Reg", "DNN_RELU", "SVM_SMALL", "SVM_LARGE", "BDT", "BDTG"};
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
	
	for (auto method: methods) {
		int count1a[3] = {0, 0, 0};
		int count2a[3] = {0, 0, 0};
		int count1b[3] = {0, 0, 0};
		int count2b[3] = {0, 0, 0};

		for (int i = 0; i < t1a->GetEntries(); ++i) {
			t1a->GetEntry(i);
			fx1 = x1; fx2 = x2; fx3 = x3; fx4 = x4; fx5 = x5%10; fx6 = x6/10; flen = len;
			double response = reader->EvaluateMVA(method);
			if (response > cut_sig[method]) ++count1a[0];
			if (response > cut_tot[method]) ++count1a[1];
			if (response > cut_sqr[method]) ++count1a[2];
		}

		for (int i = 0; i < t2a->GetEntries(); ++i) {
			t2a->GetEntry(i);
			fx1 = x1; fx2 = x2; fx3 = x3; fx4 = x4; fx5 = x5%10; fx6 = x6/10; flen = len;
			double response = reader->EvaluateMVA(method);
			if (response < cut_sig[method]) ++count2a[0];
			if (response < cut_tot[method]) ++count2a[1];
			if (response < cut_sqr[method]) ++count2a[2];
		}

		for (int i = 0; i < t1b->GetEntries(); ++i) {
			t1b->GetEntry(i);
			fx1 = x1; fx2 = x2; fx3 = x3; fx4 = x4; fx5 = x5%10; fx6 = x6/10; flen = len;
			double response = reader->EvaluateMVA(method);
			if (response > cut_sig[method]) ++count1b[0];
			if (response > cut_tot[method]) ++count1b[1];
			if (response > cut_sqr[method]) ++count1b[2];
		}

		for (int i = 0; i < t2b->GetEntries(); ++i) {
			t2b->GetEntry(i);
			fx1 = x1; fx2 = x2; fx3 = x3; fx4 = x4; fx5 = x5%10; fx6 = x6/10; flen = len;
			double response = reader->EvaluateMVA(method);
			if (response < cut_sig[method]) ++count2b[0];
			if (response < cut_tot[method]) ++count2b[1];
			if (response < cut_sqr[method]) ++count2b[2];
		}

		cout << Form("%9s cut at %12f%12f%12f", method.Data(), cut_sig[method], cut_tot[method], cut_sqr[method]) << endl;
		cout << Form("    type1@fake sample: %12d%12d%12d (%8lld in total)", count1a[0], count1a[1], count1a[2], t1a->GetEntries()) << endl;
		cout << Form("    type2@fake sample: %12d%12d%12d (%8lld in total)", count2a[0], count2a[1], count2a[2], t2a->GetEntries()) << endl;
		cout << Form("    type1@true sample: %12d%12d%12d (%8lld in total)", count1b[0], count1b[1], count1b[2], t1b->GetEntries()) << endl;
		cout << Form("    type2@true sample: %12d%12d%12d (%8lld in total)", count2b[0], count2b[1], count2b[2], t2b->GetEntries()) << endl;
		cout << endl;
	}
}

//int main( int argc, char** argv )
//{
//}
