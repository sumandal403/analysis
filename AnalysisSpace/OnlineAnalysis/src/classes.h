#ifndef AnalysisSpace_OnlineAnalysis_classes_h
#define AnalysisSpace_OnlineAnalysis_classes_h
#include "AnalysisSpace/OnlineAnalysis/interface/OnlineObjects.h"
#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include <vector>
#include<map>
#include<string>
namespace {
  struct dictionary {

    std::map<std::string,double> sd;

    std::pair<pat::Electron,std::vector<pat::PackedCandidate> >  patepc;
    std::vector<std::pair<pat::Electron,std::vector<pat::PackedCandidate> > >  vpatepc;
    edm::Wrapper<std::pair<pat::Electron,std::vector<pat::PackedCandidate>>>  wpatepc;
    edm::Wrapper<std::vector<std::pair<pat::Electron,std::vector<pat::PackedCandidate> > > >  wvpatepc;

    std::pair<pat::Muon,std::vector<pat::PackedCandidate> >  patmpc;
    std::vector<std::pair<pat::Muon,std::vector<pat::PackedCandidate> > > vpatmpc;
    edm::Wrapper<std::pair<pat::Muon,std::vector<pat::PackedCandidate> > >  wpatmpc;
    edm::Wrapper<std::vector<std::pair<pat::Muon,std::vector<pat::PackedCandidate> > > >  wvpatmpc;

    vhtm::IsoElectron isoele;
    std::vector<vhtm::IsoElectron> visoele;
    edm::Wrapper<vhtm::IsoElectron> wisoele;
    edm::Wrapper<std::vector<vhtm::IsoElectron>> vwisoele;

    vhtm::IsoMuon isomu;
    std::vector<vhtm::IsoMuon> visomu;
    edm::Wrapper<vhtm::IsoMuon> wisomu;
    edm::Wrapper<std::vector<vhtm::IsoMuon>> wvisomu;

    vhtm::Zee zee;
    std::vector<vhtm::Zee> vzee;
    edm::Wrapper<vhtm::Zee> wzee;
    edm::Wrapper<std::vector<vhtm::Zee>> wvzee;

    vhtm::Zmumu zmumu;
    std::vector<vhtm::Zmumu> vzmumu;
    edm::Wrapper<vhtm::Zmumu> wzmumu;
    edm::Wrapper<std::vector<vhtm::Zmumu>> wvzmumu;

    vhtm::ZZcandidate zzcand;
    std::vector<vhtm::ZZcandidate> vzzcand;
    edm::Wrapper<vhtm::ZZcandidate> wzzcand;
    edm::Wrapper<std::vector<vhtm::ZZcandidate>> wvzzcand;
   
    vhtm::SelectedEvent  vselev;
     
    vhtm::ZtnP           ztnp;
    std::vector<vhtm::ZtnP>    vztmp;
    //edm::Wrapper<vhtm::SelectedEvent>    wvselev; 
  };
}
#endif
