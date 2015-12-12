#include <iostream>
#include <algorithm>

#include "TTree.h"
#include "TPRegexp.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "AnalysisSpace/TreeMaker/plugins/TriggerBlock.h"
#include "AnalysisSpace/TreeMaker/interface/Utility.h"

static const unsigned int NmaxL1AlgoBit = 128;
static const unsigned int NmaxL1TechBit = 64;

// Constructor
TriggerBlock::TriggerBlock(const edm::ParameterSet& iConfig) :
  verbosity_(iConfig.getUntrackedParameter<int>("verbosity", 0)),
  l1Tag_(iConfig.getUntrackedParameter<edm::InputTag>("l1InputTag", edm::InputTag("gtDigis"))),
  hltTag_(iConfig.getUntrackedParameter<edm::InputTag>("hltInputTag", edm::InputTag("TriggerResults","","HLT"))),
  hltPathsOfInterest_(iConfig.getParameter<std::vector<std::string> >("hltPathsOfInterest")),
  l1Token_(consumes<L1GlobalTriggerReadoutRecord>(l1Tag_)),
  hltToken_(consumes<edm::TriggerResults>(hltTag_))
{
  produces<std::vector<vector<int>>>().setBranchAlias("vhtmTriggerResults");
}
TriggerBlock::~TriggerBlock() {
  delete l1physbits_;
  delete l1techbits_;
  delete hltpaths_;
  delete hltresults_;
  delete hltprescales_;
}
void TriggerBlock::beginJob()
{
  std::string tree_name = "vhtree";
  TTree* tree = vhtm::Utility::getTree(tree_name);

  l1physbits_ = new std::vector<int>();
  tree->Branch("l1physbits", "vector<int>", &l1physbits_);

  l1techbits_ = new std::vector<int>();
  tree->Branch("l1techbits", "vector<int>", &l1techbits_);

  hltpaths_ = new std::vector<std::string>();
  tree->Branch("hltpaths", "vector<string>", &hltpaths_);

  hltresults_ = new std::vector<int>();
  tree->Branch("hltresults", "vector<int>", &hltresults_);

  hltprescales_ = new std::vector<int>();
  tree->Branch("hltprescales", "vector<int>", &hltprescales_);
}
void TriggerBlock::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {
  bool changed = true;
  if (hltConfig_.init(iRun, iSetup, hltTag_.process(), changed)) {
    // if init returns TRUE, initialisation has succeeded!
    edm::LogInfo("TriggerBlock") << "HLT config with process name "
				 << hltTag_.process() << " successfully extracted";
    matchedPathList_.clear();
    const std::vector<std::string>& pathList = hltConfig_.triggerNames();
    for (const std::string& path: pathList) {
      if (hltPathsOfInterest_.size()) {
        int nmatch = 0;
        for (const std::string& kt: hltPathsOfInterest_)
          nmatch += TPRegexp(kt).Match(path);
        if (!nmatch) continue;
      }
      matchedPathList_.push_back(path);
    }
  }
  else {
    // if init returns FALSE, initialisation has NOT succeeded, which indicates a problem
    // with the file and/or code and needs to be investigated!
    edm::LogError("TriggerBlock") << "Error! HLT config extraction with process name "
				  << hltTag_.process() << " failed";
    // In this case, all access methods will return empty values!
  }
}
void TriggerBlock::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // Reset the vectors
  l1physbits_->clear();
  l1techbits_->clear();
  hltpaths_->clear();
  hltresults_->clear();
  hltprescales_->clear();

  edm::Handle<L1GlobalTriggerReadoutRecord> l1GtReadoutRecord;
  bool found = iEvent.getByToken(l1Token_, l1GtReadoutRecord);
  if (found && l1GtReadoutRecord.isValid()) {
    edm::LogInfo("TriggerBlock") << "Successfully obtained L1GlobalTriggerReadoutRecord for label: "
                                 << l1Tag_;

    for (unsigned int i = 0; i < NmaxL1AlgoBit; ++i) 
      l1physbits_->push_back(l1GtReadoutRecord->decisionWord()[i] ? 1 : 0);

    for (unsigned int i = 0; i < NmaxL1TechBit; ++i) 
      l1techbits_->push_back(l1GtReadoutRecord->technicalTriggerWord()[i] ? 1 : 0 );
  }
  else 
    edm::LogError("TriggerBlock") << "Error >> Failed to get L1GlobalTriggerReadoutRecord for label: "
                                  << l1Tag_;

  edm::Handle<edm::TriggerResults> triggerResults;
  found = iEvent.getByToken(hltToken_, triggerResults);
  if (found && triggerResults.isValid()) {
    edm::LogInfo("TriggerBlock") << "Successfully obtained " << hltTag_;
    for (auto path: matchedPathList_) {
      hltpaths_->push_back(path);

      int fired = -1;
      unsigned int index = hltConfig_.triggerIndex(path);
      if (index < triggerResults->size()) {
        fired = (triggerResults->accept(index)) ? 1 : 0;
      }
      else {
	edm::LogInfo("TriggerBlock") << "Requested HLT path \"" << path << "\" does not exist";
      }
      hltresults_->push_back(fired);

      int prescale = -1;
      if (hltConfig_.prescaleSet(iEvent, iSetup) < 0) {
	edm::LogError("TriggerBlock") << "The prescale set index number could not be obtained for HLT path: "
                                      << path;
      }
      else {
        prescale = hltConfig_.prescaleValue(iEvent, iSetup, path);
      }
      hltprescales_->push_back(prescale);

      if (verbosity_) {

        auto a = hltConfig_.prescaleValues(iEvent, iSetup, path);

	edm::LogInfo("TriggerBlock") 
	  << ">>> Path: " << (path) 
	  << ", prescale: " << prescale 
	  << ", fired: " << fired
	  << ", PrescaleValues L1: " << a.first 
	  << ", PrescaleValues HLT: " << a.second;
    
        auto d = hltConfig_.moduleLabels(path);
        for (auto v: d)
	  edm::LogInfo("TriggerBlock") << "\tModule Labels: " << v;
      }    
    }      
    if (verbosity_) {
      const std::vector<std::string>& b = hltConfig_.prescaleLabels();
      for (auto v: b)
	edm::LogInfo("TriggerBlock") << "\tPrescale Labels: " << v;

      const std::map<std::string, std::vector<unsigned int> >& c = hltConfig_.prescaleTable();
      for (auto ptr: c) {
	edm::LogInfo("TriggerBlock") << "Key  : " << ptr.first << ": ";
        for (auto v: ptr.second)
	  edm::LogInfo("TriggerBlock") << "value: " << v;
      }
    } 
    //put the vhtm collections in edm
    std::auto_ptr<std::vector<int>> pv1(new std::vector<int>(*l1physbits_));
    iEvent.put(pv1,"l1physbits");
    //put the vhtm collections in edm
    std::auto_ptr<std::vector<int>> pv2(new std::vector<int>(*l1techbits_));
    iEvent.put(pv2,"l1techbits");
    //put the vhtm collections in edm
    std::auto_ptr<std::vector<int>> pv3(new std::vector<int>(*hltpaths_));
    iEvent.put(pv3,"hltpaths");
    //put the vhtm collections in edm
    std::auto_ptr<std::vector<int>> pv4(new std::vector<int>(*hltresults_));
    iEvent.put(pv4,"hltresults");
    //put the vhtm collections in edm
    std::auto_ptr<std::vector<int>> pv5(new std::vector<int>(*hltprescales));
    iEvent.put(pv5,"hltprescales");
  } 
  else {
    edm::LogError("TriggerBlock") << "Failed to get TriggerResults for label: "
                                  << hltTag_;
  }
}
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TriggerBlock);
