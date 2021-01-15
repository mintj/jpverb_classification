void tmva()
{
	TFile * f = TFile::Open("../prepare_data/fake_data_training/jpverb.root");
	TTree * tree1 = (TTree *)f->Get("type1");
	TTree * tree2 = (TTree *)f->Get("type2");

	TString outfilename("result.root");
	TFile * outfile = TFile::Open(outfilename, "recreate");

	TMVA::Factory * factory = new TMVA::Factory("jpvc", outfile,
			"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

	TMVA::DataLoader * dataloader=new TMVA::DataLoader("dataset");

	dataloader->AddSignalTree(tree1,     1);
	dataloader->AddBackgroundTree(tree2, 1);
	
	dataloader->AddVariable("x[4]", 'F');
	dataloader->AddVariable("x[5]", 'F');

	dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
	//dataloader->PrepareTrainingAndTestTree( "", "nTrain_Signal=5000:nTrain_Background=5000:nTest_Signal=5000:nTest_Background=5000:SplitMode=Random:NormMode=NumEvents:!V" );
	
	factory->BookMethod(dataloader, TMVA::Types::kLikelihood, "Likelihood",
		"H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50");

	factory->BookMethod(dataloader, TMVA::Types::kKNN, "KNN",
		"H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim");
	
	factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP",
		"H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator");
	factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP_L2Reg",
		"H:!V:NeuronType=tanh:VarTransform=N:NCycles=60:HiddenLayers=N+5:TestRate=5:UseRegulator" );

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
	
	//factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_ReLU", cpuOptions_COMP);
	//factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN_ReLU_mlp", cpuOptions_SIMP);
	
	factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
		//"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
		"!H:!V:NTrees=1000:MinNodeSize=0.3%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3");
	factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
		//"!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
		"!H:!V:NTrees=1000:MinNodeSize=0.3%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
	//factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG_CE",
	//	"!H:!V:NTrees=1000:MinNodeSize=0.3%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:SeparationType=CrossEntropy");
	//factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT_CE",
	//	"!H:!V:NTrees=1000:MinNodeSize=0.3%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=CrossEntropy:nCuts=20");
	//
	//factory->BookMethod(dataloader, TMVA::Types::kSVM, "SVM_LARGE", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );
	//factory->BookMethod(dataloader, TMVA::Types::kSVM, "SVM_SMALL", "Gamma=25.0:Tol=0.001:VarTransform=Norm" );
	
	factory->TrainAllMethods();

	factory->TestAllMethods();

	factory->EvaluateAllMethods();

	outfile->Close();

	if (!gROOT->IsBatch()) {
		TMVA::TMVAGui(outfilename);
	}

}
