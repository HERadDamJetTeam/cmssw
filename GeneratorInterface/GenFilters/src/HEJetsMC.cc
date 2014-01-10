// -*- C++ -*-
//
// Package:   HEJetsMC
// Class:     HEJetsMC
// 
/**\class HEJetsMC HEJetsMC.cc

 Description: Filter for HE Radiation Damage MC generation.

 Implementation:
     [Notes on implementation]
*/
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"
#include <iostream>

using namespace edm;
using namespace std;
using namespace reco;

//
// class declaration
//

class HEJetsMC : public edm::EDFilter 
{
public:
  explicit HEJetsMC(const edm::ParameterSet&);
  ~HEJetsMC();
  
private:
  virtual void beginJob() ;
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // ----------member data ---------------------------
  
  edm::InputTag GenHandle_;
  double minpt_;

};

HEJetsMC::HEJetsMC(const edm::ParameterSet& iConfig):
  GenHandle_(iConfig.getUntrackedParameter<InputTag>("GenTag")),
  minpt_(iConfig.getParameter<double>("MinPt"))
{
}


HEJetsMC::~HEJetsMC()
{
}

bool HEJetsMC::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   Handle<reco::GenJetCollection> GenJets;
   iEvent.getByLabel(GenHandle_, GenJets);

   bool result = false;
   bool accept_eta = false;
   bool accept_pt = false;
   double etajet[2] = {0.,0.};
   double ptjet[2] = {0.,0.};
   int n_gen = 0;

   for(GenJetCollection::const_iterator iJet = GenJets->begin(); iJet != GenJets->end() && n_gen<2; ++iJet)
     {
       reco::GenJet myJet = reco::GenJet(*iJet);
       etajet[n_gen] = myJet.eta();
       ptjet[n_gen] = myJet.pt();
       n_gen++;	
     }

   accept_eta = (fabs(etajet[0])<1.8 && etajet[1]>1.8 && etajet[1]<3.0) || (fabs(etajet[1])<1.8 && etajet[0]>1.8 && etajet[0]<3.0);
   accept_pt = ptjet[0] > minpt_ && ptjet[1] > minpt_; 
	
   if(n_gen == 2 && accept_eta && accept_pt)
      result = true;

   return result;
}

void HEJetsMC::beginJob()
{
}

void HEJetsMC::endJob()
{
}

//define this as a plug-in
DEFINE_FWK_MODULE(HEJetsMC);
