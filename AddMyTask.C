class AliAnalysisDataContainer;

AliAnalysisTaskSounds* AddMyTask(TString name = "name")
{
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }
    TString fileName = AliAnalysisManager::GetCommonFileName();
    fileName += ":MyTask";      
    AliAnalysisTaskSounds* task = new AliAnalysisTaskSounds(name.Data());   
    if(!task) return 0x0;
    mgr->AddTask(task);
    TString contname = name;
    contname += "_output";
    mgr->ConnectInput(task,0,mgr->GetCommonInputContainer());
    mgr->ConnectOutput(task,1,mgr->CreateContainer(contname.Data(), TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    return task;
}
