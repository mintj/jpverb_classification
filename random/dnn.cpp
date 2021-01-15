void dnn()
{
	TFile * f = TFile::Open("../prepare_data/fake_data_training/jpverb.root");
	TTree * tree1 = (TTree *)f->Get("type1_rndm");
	TTree * tree2 = (TTree *)f->Get("type2_rndm");

	TString outfilename("result.root");
	TFile * outfile = TFile::Open(outfilename, "update");

	TMVA::Factory * factory = new TMVA::Factory("jpvc", outfile,
			"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

	TMVA::DataLoader * dataloader=new TMVA::DataLoader("dataset");

	dataloader->AddSignalTree(tree1,     1);
	dataloader->AddBackgroundTree(tree2, 1);
	
	dataloader->AddVariable("x[0]", 'F');
	dataloader->AddVariable("x[1]", 'F');
	dataloader->AddVariable("x[2]", 'F');
	dataloader->AddVariable("x[3]", 'F');
	dataloader->AddVariable("x[4]", 'F');
	dataloader->AddVariable("x[5]", 'F');

	dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );

	TString layoutString_COMP ("Layout=RELU|100,RELU|100,RELU|100,LINEAR");
	//TString layoutString ("Layout=RELU|N,RELU|5,RELU|5,RELU|5,RELU|5,SIGMOID");//RELU|100,RELU|100,SIGMOID");
	TString layoutString_SIMP ("Layout=RELU|N,RELU|5,SIGMOID");
	//TString layoutString ("Layout=TANH|N,TANH|5,LINEAR");
	TString training0_COMP("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
	TString training1_COMP("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.3+0.0+0.0, Multithreading=True");
	TString training2_COMP("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.1+0.0+0.0, Multithreading=True");
	TString trainingStrategyString_COMP ("TrainingStrategy=");
	trainingStrategyString_COMP += training0_COMP + "|" + training1_COMP + "|" + training2_COMP;
	
	TString training0_SIMP("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
	TString training1_SIMP("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
	TString training2_SIMP("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
	TString trainingStrategyString_SIMP ("TrainingStrategy=");
	trainingStrategyString_SIMP += training0_SIMP + "|" + training1_SIMP + "|" + training2_SIMP;

	// General Options.
	TString dnnOptions_COMP ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
		"WeightInitialization=XAVIERUNIFORM");
	dnnOptions_COMP.Append (":"); dnnOptions_COMP.Append (layoutString_COMP);
	dnnOptions_COMP.Append (":"); dnnOptions_COMP.Append (trainingStrategyString_COMP);
	//dnnOptions_COMP.Append (":"); dnnOptions_COMP.Append (trainingStrategyString_SIMP);

	TString dnnOptions_SIMP ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
		"WeightInitialization=XAVIERUNIFORM");
	dnnOptions_SIMP.Append (":"); dnnOptions_SIMP.Append (layoutString_SIMP);
	dnnOptions_SIMP.Append (":"); dnnOptions_SIMP.Append (trainingStrategyString_SIMP);

	TString cpuOptions_COMP = dnnOptions_COMP + ":Architecture=CPU";
	TString cpuOptions_SIMP = dnnOptions_SIMP + ":Architecture=CPU";
	
	factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_ReLU", cpuOptions_COMP);
	factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_ReLU_mlp", cpuOptions_SIMP);
	
	factory->TrainAllMethods();

	factory->TestAllMethods();

	factory->EvaluateAllMethods();

	outfile->Close();

	if (!gROOT->IsBatch()) {
		TMVA::TMVAGui(outfilename);
	}

}
