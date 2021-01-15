class alg
{
	public:
		alg(TString name);
		virtual ~alg();
		TGraph * get_score() { return gr_score; }
		TGraph * get_diff()  { return gr_diff; }
		double avg_score();
		double avg_diff();
		
		static void preset();
		static vector<TString> get_encodings() { return encodings; }

	private:
		double * xx;
		double * score;
		double * diff;
		TGraph * gr_score;
		TGraph * gr_diff;
		
		static vector<TString> encodings;
		static map<TString, TString> path;
		static bool setup;
};
		
vector<TString> alg::encodings = {"nominal", "head", "head_wo_len", "last2", "last2_trans", "random"};

map<TString, TString> alg::path;

bool alg::setup = false;

void alg::preset()
{
	path["nominal"]     = "nominal";
	path["last2"]       = "last2";
	path["last2_trans"] = "last2_trans";
	path["random"]      = "random";
	path["head"]        = "complete_at_head";
	path["head_wo_len"] = "complete_at_head_wo_len";
	setup = true;
}

alg::alg(TString name)
{
	if (!setup) {
		preset();
	}

	xx = new double[6];
	score = new double[6];
	diff = new double[6];
	for (int i = 0; i < 6; ++i) {
		xx[i] = i+0.5;
		score[i] = -1;
		diff[i] = -1;

		TString a = encodings[i];
		TString fname = path[a] + "/score.dat";
		ifstream ifs(fname);
		double s, d;
		TString algname;
		while (ifs >> algname) {
			ifs >> s >> s >> d;
			//if (algname.Contains(name)) {
			if (algname == name) {
				if (score[i] < 0 || s > score[i]) {
					score[i] = s;
					diff[i] = d;
				}
			}
		}
	}
	gr_score = new TGraph(6, xx, score);
	gr_diff  = new TGraph(6, xx, diff);
}

alg::~alg()
{
}
		
double alg::avg_score()
{
	double avg = 0;
	for (int i = 0; i < 6; ++i) {
		avg += score[i];
	}
	avg /= 6;
	return avg;
}

double alg::avg_diff()
{
	double avg = 0;
	for (int i = 0; i < 6; ++i) {
		avg += diff[i];
	}
	avg /= 6;
	return avg;
}

void draw_comparison()
{
	gStyle->SetOptStat(0);
	//gStyle->SetOptTitle(0);

	TString aa[8] = {"Likelihood", "KNN", "MLP", "MLP_L2Reg", "DNN_ReLU", "DNN_ReLU_mlp", "BDT", "BDTG"};
	vector<TGraph *> score, diff;
	for (auto a: aa) {
		alg test(a);
		cout << a << ": " << test.avg_score() << " " << test.avg_diff() << endl;
		score.push_back(test.get_score());
		diff .push_back(test.get_diff());
	}
	
	TH1F * hlabel = new TH1F("hlabel", "", 6, 0, 6);
	auto encodings = alg::get_encodings();
	for (int i = 0; i < 6; ++i) {
		hlabel->Fill(encodings[i], 0);
	}
	hlabel->GetYaxis()->SetTitle("score");
	hlabel->SetLabelSize(0.05);
	hlabel->LabelsOption("v", "X");
	hlabel->GetYaxis()->SetTitle("score");
	hlabel->GetYaxis()->SetTitleSize(0.07);
	hlabel->GetYaxis()->SetTitleOffset(0.5);
	hlabel->GetYaxis()->SetLabelSize(0.05);
	hlabel->GetXaxis()->SetLabelSize(0.1);
	hlabel->SetMaximum(1.05);
	hlabel->SetMinimum(0);

	TLine * line1 = new TLine(0, 1, 6, 1);
	line1->SetLineColor(1);
	line1->SetLineStyle(2);
	line1->SetLineWidth(2);
	
	//TH1F * hright = new TH1F("hright", "", 6, 0, 6);
	//hright->SetLineColor(2);
	//hright->SetLineWidth(2);
	//hright->SetLineStyle(2);
	//hright->SetMaximum(0.2);
	//hright->SetMinimum(-0.5);
	
	double zz = 0.2*1.05/1.2;
	TLine * line2 = new TLine(0, zz, 6, zz);
	line2->SetLineColor(2);
	line2->SetLineStyle(2);
	line2->SetLineWidth(2);
	

	TGaxis * axis = new TGaxis(6, 0.0, 6, 1.05, -0.2, 1, 510, "+L");
	axis->SetLineColor(2);
	axis->SetLabelColor(2);
	axis->SetLabelSize(0.04);
	axis->SetLabelOffset(0.015);
	axis->SetTitleColor(2);
	axis->SetTitleSize(0.07);
	axis->SetTitleOffset(0.5);
	axis->SetTitle("#Delta");

	void shift_and_scale(TGraph * gr, double shift = 0, double sf = 1);

	TCanvas * canv = new TCanvas("c", "", 1080, 960);
	canv->Divide(4, 2);
	for (int i = 0; i < 8; ++i) {
		canv->cd(i+1);
		gPad->SetTopMargin(0.1);
		gPad->SetBottomMargin(0.23);
		gPad->SetLeftMargin(0.1);
		gPad->SetRightMargin(0.15);
		hlabel->SetTitle(aa[i]);
		hlabel->DrawCopy();
		line1->Draw();
		line2->Draw();
		//hright->Draw("same");
		axis->Draw();
	
		score[i]->SetLineColor(3);
		score[i]->SetLineWidth(2);
		score[i]->SetMarkerColor(3);
		score[i]->SetMarkerStyle(29);
		score[i]->Draw("lp same");

		shift_and_scale(diff[i], zz, 1);
		diff[i]->SetLineColor(2);
		diff[i]->SetLineWidth(2);
		diff[i]->SetMarkerColor(2);
		diff[i]->SetMarkerStyle(47);
		diff[i]->Draw("lp same");
	}
}
	
void shift_and_scale(TGraph * gr, double shift = 0, double sf = 1)
{
	for (int i = 0; i < 6; ++i) {
		double x = gr->GetX()[i];
		double y = gr->GetY()[i];
		gr->SetPoint(i, x, y*sf+shift);
		cout << x << " " << y << " " << y*sf << endl;
	}
}
