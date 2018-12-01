
void runAnalysis(TString outName = "output_LHC10d")
{
    Bool_t local = kTRUE;
    Bool_t gridTest = kFALSE;
    
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");

    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("AnalysisTaskExample");
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);

    // compile the class (locally)
    //gROOT->LoadMacro("AliAnalysisTaskSounds.cxx++g");
    // load the addtask macro
    //gROOT->LoadMacro("AddMyTask.C");
    // create an instance of your analysis task
    // black notes
    AliAnalysisTaskSounds *task = AddMyTask("task1");
    task->SetPitchOption(AliAnalysisTaskSounds::kEbony); 
    task->SetOutputName(outName); 
    task->SetPtHigh(5.);
    task->SetPtLow(1.0);
    task->SetIsLowPtCutoff(kTRUE);
    task->SetNmaxEvents(300);

    // white notes
    AliAnalysisTaskSounds *task2 = AddMyTask("task2");
    task2->SetPitchOption(AliAnalysisTaskSounds::kIvory); 
    task2->SetOutputName(outName); 
    task2->SetPtHigh(5.);
    task2->SetPtLow(1.0);
    task2->SetIsLowPtCutoff(kTRUE);
    task2->SetNmaxEvents(300);

    // all notes
    AliAnalysisTaskSounds *task3 = AddMyTask("task3");
    task3->SetPitchOption(AliAnalysisTaskSounds::kAll); 
    task3->SetOutputName(outName); 
    task3->SetPtHigh(5.);
    task3->SetPtLow(1.0);
    task3->SetIsLowPtCutoff(kTRUE);
    task3->SetNmaxEvents(300);

    if(!mgr->InitAnalysis()) return;
    mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) {
        // if you want to run locally, we need to define some input
        TChain* chain = new TChain("aodTree");
        // add a few files to the chain (change this so that your local files are added)
        //chain->Add("~/Lc/AOD/LHC11b2/130795/AOD136a/1/AliAOD.root"); //pp 7 TeV
        chain->Add("/Users/jaime/Lc/AOD/LHC10d/AOD137/1/AliAOD.root"); //pp 7 TeV
				//chain->Add("/Users/jaime/Lc/AOD/LHC16l/1/AliAOD.root"); // pp 13 TeV
        //chain->Add("~/Lc/AOD/LHC11h_2/170593/AOD145/1/AliAOD.root"); //Pb-Pb
        //chain->Add("/Users/jaime/Lc/AOD/LHC13d3/195389/AOD159/1/AliAOD.root"); // p-Pb 5 TeV
        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chain);
    } else {
        // if we want to run on grid, we create and configure the plugin
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        // make sure your source files get copied to grid
        alienHandler->SetAdditionalLibs("AliAnalysisTaskSounds.cxx AliAnalysisTaskSounds.h");
        alienHandler->SetAnalysisSource("AliAnalysisTaskSounds.cxx");
        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion("vAN-20160330-2");
        // select the input data
        alienHandler->SetGridDataDir("/alice/data/2011/LHC11h_2");
        alienHandler->SetDataPattern("*ESDs/pass2/AOD145/*AOD.root");
        // MC has no prefix, data has prefix 000
        alienHandler->SetRunPrefix("000");
        // runnumber
        alienHandler->AddRunNumber(167813);
        // number of files per subjob
        alienHandler->SetSplitMaxInputFileNumber(40);
        alienHandler->SetExecutable("myTask.sh");
        // specify how many seconds your job may take
        alienHandler->SetTTL(10000);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);
        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate") 
        // (see below) mode, set SetMergeViaJDL(kFALSE) 
        // to collect final results
        alienHandler->SetMaxMergeStages(1);
        alienHandler->SetMergeViaJDL(kTRUE);

        // define the output folders
        alienHandler->SetGridWorkingDir("myWorkingDir");
        alienHandler->SetGridOutputDir("myOutputDir");

        // connect the alien plugin to the manager
        mgr->SetGridHandler(alienHandler);
        if(gridTest) {
            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(1);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            mgr->StartAnalysis("grid");
        } else {
            // else launch the full grid analysis
            alienHandler->SetRunMode("full");
            mgr->StartAnalysis("grid");
        }
    }
}
