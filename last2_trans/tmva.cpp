void tmva()
{
	TFile * f1 = TFile::Open("../prepare_data/fake_data/type1_clean.fake_data.dat.tail_len6.root");
	TFile * f2 = TFile::Open("../prepare_data/fake_data/type2.fake_data.dat.tail_len6.root");
	TTree * tree1 = (TTree *)f1->Get("save");
	TTree * tree2 = (TTree *)f2->Get("save");

	TString outfilename("result.root");
	TFile * outfile = TFile::Open(outfilename, "recreate");

	TMVA::Factory * factory = new TMVA::Factory("jpvc", outfile,
			"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

	TMVA::DataLoader * dataloader=new TMVA::DataLoader("dataset");

	dataloader->AddSignalTree(tree1,     1);
	dataloader->AddBackgroundTree(tree2, 1);
	
	dataloader->AddVariable("x5%10", 'F');
	dataloader->AddVariable("x6/10", 'F');

	dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
	
	factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "Likelihood",
		"H:!V:CreateMVAPdfs:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50");

	factory->BookMethod(dataloader, TMVA::Types::kKNN, "KNN",
		"H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim");
	
	factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP",
		"H:!V:NeuronType=tanh:CreateMVAPdfs:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator");
	factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP_L2Reg",
		"H:!V:NeuronType=tanh:CreateMVAPdfs:VarTransform=N:NCycles=60:HiddenLayers=N+5:TestRate=5:UseRegulator" );
	TString layoutString ("Layout=RELU|100,RELU|100,RELU|100,SIGMOID");
	TString training0("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
	TString training1("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
	TString training2("LearningRate=1e-1,Momentum=0.0,Repetitions=1,"
		"ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
		"WeightDecay=1e-4,Regularization=L2,"
		"DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
	TString trainingStrategyString ("TrainingStrategy=");
	trainingStrategyString += training0 + "|" + training1 + "|" + training2;

	// General Options.
	TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:CreateMVAPdfs:"
		"WeightInitialization=XAVIERUNIFORM");
	dnnOptions.Append (":"); dnnOptions.Append (layoutString);
	dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);

	TString cpuOptions = dnnOptions + ":Architecture=CPU";
	factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_RELU", cpuOptions);
	
	factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
		//"!H:!V:NTrees=1000:CreateMVAPdfs:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
		"!H:!V:NTrees=1000:CreateMVAPdfs:MinNodeSize=0.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
	factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
		//"!H:!V:NTrees=1000:CreateMVAPdfs:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
		"!H:!V:NTrees=1000:CreateMVAPdfs:MinNodeSize=0.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
	
	factory->BookMethod( dataloader, TMVA::Types::kSVM, "SVM_LARGE", "Gamma=0.01:Tol=0.001:VarTransform=Norm:CreateMVAPdfs" );
	factory->BookMethod( dataloader, TMVA::Types::kSVM, "SVM_SMALL", "Gamma=0.25:Tol=0.001:VarTransform=Norm:CreateMVAPdfs" );
	
	factory->TrainAllMethods();

	factory->TestAllMethods();

	factory->EvaluateAllMethods();

	outfile->Close();

	if (!gROOT->IsBatch()) {
		TMVA::TMVAGui(outfilename);
	}

}
