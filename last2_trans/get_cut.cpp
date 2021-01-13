class mva_result
{
	public:
		mva_result(const char * fname);
		virtual ~mva_result();
		void add(const char * method);
		void save(const char * fname1, const char * fname2);

	private:
		TFile * m_input;
		TDirectory * m_dir0;
		std::vector<TString> m_methods;
		std::map<TString, TDirectory *> m_dir;
		std::map<TString, TH1D *> m_hsig;
		std::map<TString, TH1D *> m_htot;
		std::map<TString, TH1D *> m_hsqr;
		std::map<TString, double> m_cut1;
		std::map<TString, double> m_cut2;
		std::map<TString, double> m_cut3;

	public:
		static const char * get_parent(const TString & child);

	private:
		static std::vector<TString> base_methods;
};

std::vector<TString> mva_result::base_methods =
{"Likelihood", "KNN", "MLP", "DNN", "BDT", "SVM"};

mva_result::mva_result(const char * fname)
{
	m_input = TFile::Open(fname);
	m_dir0 = (TDirectory *)m_input->Get("dataset");
}

mva_result::~mva_result()
{
	m_input->Close();
}

const char * mva_result::get_parent(const TString & child)
{
	for (auto bm: base_methods) {
		if (child.Contains(bm)) {
			return bm;
		}
	}
	return "";
}

void mva_result::add(const char * method)
{
	m_methods.push_back(method);
	const char * base_method = Form("Method_%s", get_parent(method));

	TDirectory * dir = (TDirectory *)m_dir0->Get(base_method);
	dir = (TDirectory *)dir->Get(method);
	m_dir[method] = dir;

	TH1D * effs = (TH1D *)dir->Get(Form("MVA_%s_effS", method));
	TH1D * effb = (TH1D *)dir->Get(Form("MVA_%s_effB", method));
	
	m_hsig[method] = (TH1D *)effs->Clone(Form("%s_sig_vs_cut", method));
	m_htot[method] = (TH1D *)effs->Clone(Form("%s_tot_vs_cut", method));
	m_hsqr[method] = (TH1D *)effs->Clone(Form("%s_sqr_vs_cut", method));
	
	for (int i = 1; i <= effs->GetNbinsX(); ++i) {
		//double s = effs->GetBinContent(i);
		//double b = effb->GetBinContent(i);
		double s = effs->GetBinContent(i);
		double b = effb->GetBinContent(i);
		double err_t1 = b;
		double err_t2 = 1-s;
		double sig = (s+b>0) ? s/sqrt(s+b) : 0;
		m_hsig[method]->SetBinContent(i, sig);
		m_htot[method]->SetBinContent(i, err_t1 + err_t2);
		m_hsqr[method]->SetBinContent(i, err_t1*err_t1 + err_t2*err_t2);
	}

	m_cut1[method] = m_hsig[method]->GetBinCenter(m_hsig[method]->GetMaximumBin());
	m_cut2[method] = m_htot[method]->GetBinCenter(m_htot[method]->GetMinimumBin());
	m_cut3[method] = m_hsqr[method]->GetBinCenter(m_hsqr[method]->GetMinimumBin());
}

void mva_result::save(const char * fname1, const char * fname2)
{
	TFile * outf = TFile::Open(fname1, "recreate");
	std::ofstream ofs(fname2);

	for (auto method: m_methods) {
		m_hsig[method]->Write();
		m_htot[method]->Write();
		m_hsqr[method]->Write();

		ofs << method;
		ofs << "  " << m_cut1[method];
		ofs << "  " << m_cut2[method];
		ofs << "  " << m_cut3[method];
		ofs << "\n";
	}
	outf->Close();
	ofs.close();
}

void get_cut()
{
	mva_result a("result.root");
	vector<TString> methods {"Likelihood", "KNN", "MLP", "MLP_L2Reg", "DNN_RELU", "SVM_SMALL", "SVM_LARGE", "BDT", "BDTG"};
	for (auto m: methods) {
		a.add(m);
	}
	a.save("plot.root", "cuts.dat");
}
