// -*- C++ -*-
//
// Package:    TriggerValidation
// Class:      TriggerValidation
// 
/**\class TriggerValidation TriggerValidation.cc quarkoniumAnalysis/TriggerValidation/src/TriggerValidation.cc
   
Description: <one line class summary>

Implementation:
<Notes on implementation>

     To calculate invariant mass of two leptons.
     Storing variables:
     0 runno
     1 evtno
     2 itag: 0 opposite signed dimuon
           : 1 same signed dimuon
           : 2 opposite signed dielectron
           : 3 same signed dielectron
           : 4 opposite signed electron and muon
	   : 5 same signed electron and muon                       
	     3-5 +ve lepton's momentum (including sign of charge), theta, phi
	     6,7 dR, dZ of tracks for the same
	     8-10 -ve lepton's momentum (including sign of charge), theta, phi
	     11-12 dR, dZ of tracks for the same
	     13 invariant mass
	     14 3D distance between two tracks. In future to be replaced by 
	        vertex fit confidence level or chiSquare/ndf
		      
*/
//
// Original Author:  Devdatta Majumder
//         Created:  Sun Jan  6 07:34:59 CET 2008
// $Id$
//
//
  
  
// system include files
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

  // user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
  
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
  
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/TriggerNames.h"
  
#include "FWCore/ParameterSet/interface/ParameterSet.h"
  
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/Particle.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"  //also included in 
#include "DataFormats/GeometryVector/interface/GlobalVector.h" //TrackingTools below

#include "DataFormats/L1Trigger/interface/L1ParticleMap.h"
#include "DataFormats/L1Trigger/interface/L1ParticleMapFwd.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMap.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistanceHelixHelix.h"

#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetMatchInfo.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TStyle.h"
#include "TGraphErrors.h"

using namespace std ;
using namespace reco ;
using namespace edm ;
using namespace trigger ;

// class decleration
//

class TriggerValidation : public edm::EDAnalyzer {
public:
  explicit TriggerValidation(const edm::ParameterSet&);
  ~TriggerValidation();
  
  
private:
  virtual void beginJob(const edm::EventSetup&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  float highestPtMuon(vector<Muon>) ;
  
  ofstream outfile ;
  
//  std::string theRootFileName ;
//  TFile* theFile ;
//  std::string theOutputPSFileName ;
  std::string theOutfileName ;
  edm::Service<TFileService> fs; // or should it be under protected?

  TH1F *moth_pT_, *moth_eta_, *moth_phi_, *moth_vtx_;
  TH1F *jpsi_pT_, *jpsi_eta_, *jpsi_phi_, *jpsi_vtx_;
  TH1F *jpsiMuP_pT_, *jpsiMuP_eta_, *jpsiMuP_phi_,              // mu+ daughters of JPsi
       *jpsiMuM_pT_, *jpsiMuM_eta_, *jpsiMuM_phi_;              // mu- daughters of JPsi
  TH1F *dau1_pT_, *dau1_eta_, *dau1_phi_, *dau1_vtx_;
  TH1F *dau2_pT_, *dau2_eta_, *dau2_phi_, *dau2_vtx_;
  TH1F *track_pT_, *track_eta_, *track_phi_ ;
  TH1F *recoMu_hist_, *recoMu_pT_, *recoMu_eta_ ;
  TH1F *l1passed_hist_, *mu_L1Passed_pT_, *mu_L1Passed_eta_ ;   // all L1
  TH1F *mu_L1_DoubleMu3_pT_, *mu_L1_DoubleMu3_eta_,             // L1 bit 51
       *mu_L1_SingleMu7_pT_, *mu_L1_SingleMu7_eta_;             // L1 bit 46
  TH1F *hltPassed_hist_, *mu_HLTPassed_pT_, *mu_HLTPassed_eta_ ;// all hlt
  TH1F *mu_HLT2MuonJPsi_pT_, *mu_HLT2MuonJPsi_eta_,             // hlt bit 59
       *mu_HLT2MuonNonIso_pT_, *mu_HLT2MuonNonIso_eta_,         // hlt bit 58
       *mu_HLT2MuonIso_pT_, *mu_HLT2MuonIso_eta_,               // hlt bit 57
       *mu_HLT1MuonNonIso_pT_, *mu_HLT1MuonNonIso_eta_,         // hlt bit 56
       *mu_HLT1MuonIso_pT_, *mu_HLT1MuonIso_eta_,               // hlt bit 55
       *mu_HLT2MuonUpsilon_pT_, *mu_HLT2MuonUpsilon_eta_,       // hlt bit 60
       *mu_HLTBJPsiMuMu_pT_, *mu_HLTBJPsiMuMu_eta_;             // hlt bit 83
  TH1F *maxMuon_pT_ ;       
  TH2F *l1_hltCorrelation_ ;

  TTree* T1 ;
  
  // ----------member data ---------------------------

  edm::InputTag l_HLT ;
  string l_genEvt ;
  string l_muon ;

  bool l1PathFlag, hltPathFlag ;
  unsigned int sizeL1, sizeHLT ;
  unsigned int nEvt, iEvt, iRun;
  unsigned int l1Accepts, hltAccepts ; // total events accepted by L1 trigger and by HLT
  unsigned int nL1AndHLT, 
  	       nL1_DoubleMu3,   // bit 48
	       nL1_SingleMu7,   // bit 2
	       nHLT2MuonJPsi,   // bit 50
	       nHLT2MuonNonIso, // bit 49
	       nCandHLT2MuonIso;// bit 48 
  float moth_pT, moth_eta, moth_vtx; // vtx funtion not executed yet 
  float dau1_pT, dau1_eta, dau1_vtx; // vtx function not executed yet
  float dau2_pT, dau2_eta, dau2_vtx; // vtx function not executed yet 
  float jpsiMuP_pT, jpsiMuP_eta, jpsiMuP_phi; 
  float jpsiMuM_pT, jpsiMuM_eta, jpsiMuM_phi;
  float track_pT, track_eta, track_phi;
  float muMuHelixPathDist ;
  float maxMu_pT, mu_maxPt_eta ; // highest pT of mu in an evt and its eta
  bool l1BitFlag, hltBitFlag ;
  std::map<int,std::string> l1NameMap;
  std::map<std::string, bool> fired ;
  std::vector<std::string> l1Names_;
  
  TrackDetectorAssociator  trackDetectorAssociator_;
  TrackAssociatorParameters trackAssociatorParameters_;
  

};

//
// constants, enums and typedefs
//
const float muMass = 0.106 ;

//
// static data member definitions
//

//
// constructors and destructor
//
TriggerValidation::TriggerValidation(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  
  l_HLT = iConfig.getParameter<edm::InputTag>("hlt") ;
  l_genEvt = iConfig.getParameter<string>("genEvt") ;
  l_muon   = iConfig.getParameter<string>  ("muon") ;
  
//  theRootFileName = iConfig.getUntrackedParameter<string>("RootFileName") ;
//  theFile = new TFile(theRootFileName.c_str(), "RECREATE") ;
//  theOutputPSFileName = iConfig.getUntrackedParameter<string>("PSFileName") ;
  theOutfileName = iConfig.getUntrackedParameter<string>("OutfileName") ;
  outfile.open(theOutfileName.c_str()) ;

  edm::Service<TFileService> fs;
      
  moth_pT_ = fs->make<TH1F>("mother_pT_", "Mother p_{T} distribution", 
		      100, 0., 100.) ;
  moth_eta_ = fs->make<TH1F>("mother_eta_", "Mother #eta distribution", 
		      50, -3.5, 3.5) ;
  moth_phi_ = fs->make<TH1F>("mother_phi_", "Mother #phi distribution", 
		       50, 0., 6.28) ;
  jpsi_pT_ = fs->make<TH1F>("jpsi_pT_", "JPsi p_{T} distribution", 
		      100, 0., 100.) ;
  jpsi_eta_ = fs->make<TH1F>("jpsi_eta_", "JPsi #eta distribution", 
		      50, -3.5, 3.5) ;
  jpsi_phi_ = fs->make<TH1F>("jpsi_phi_", "JPsi #phi distribution", 
		       50, 0., 6.28) ;
  dau1_pT_ = fs->make<TH1F>("daughter1_pT_", "Daughter1 p_{T} distribution", 
		      100, 0., 100.) ;
  dau1_eta_ = fs->make<TH1F>("daughter1_eta_", "Daughter1 #eta distribution",
		      50, -3.5, 3.5) ;
  dau1_phi_ = fs->make<TH1F>("daughter1_phi_", "Daughter1 #phi distribution",
		       50, 0., 6.28) ;
  dau2_pT_ = fs->make<TH1F>("daughter2_pT_", "Daughter2 p_{T} distribution", 
		      100, 0., 100.) ;
  dau2_eta_ = fs->make<TH1F>("daughter2_eta_", "Daughter2 #eta distribution",
		      50, -3.5, 3.5) ;
  dau2_phi_ = fs->make<TH1F>("daughter2_phi_", "Daughter2 #phi distribution",
		       50, 0., 6.28) ;
  jpsiMuP_pT_ = fs->make<TH1F>("jpsiMuP_pT_", "Mu+ frm JPsi p_{T} distribution", 
			 100, 0., 100.) ;
  jpsiMuP_eta_ = fs->make<TH1F>("jpsiMuP_eta_", "Mu+ frm JPsi #eta distribution",
			  50, -3.5, 3.5) ;
  jpsiMuP_phi_ = fs->make<TH1F>("jpsiMuP_phi_", "Mu+ frm JPsi #phi distribution",
		       50, 0., 6.28) ;
  jpsiMuM_pT_ = fs->make<TH1F>("jpsiMuM_pT_", "Mu- frm JPsi p_{T} distribution", 
		      100, 0., 100.) ;
  jpsiMuM_eta_ = fs->make<TH1F>("jpsiMuM_eta_", "Mu- frm JPsi #eta distribution",
			  50, -3.5, 3.5) ;
  jpsiMuM_phi_ = fs->make<TH1F>("jpsiMuM_phi_", "Mu- frm JPsi #phi distribution",
		       50, 0., 6.28) ;
  track_pT_ = fs->make<TH1F>("track_pT_", "Track p_{T} distribution", 
		       100, 0., 100.) ;
  track_eta_ = fs->make<TH1F>("track_eta_", "Track #eta distribution", 
			50, -3.5, 3.5) ;
  track_phi_ = fs->make<TH1F>("track_phi_", "Track #phi distribution", 
			50, 0., 6.28) ;
  recoMu_hist_ = fs->make<TH1F>("recoMu_hist_", "Muon counts per event", 
		      20, 0., 20.) ;
  recoMu_pT_ = fs->make<TH1F>("recoMu_pT_", "Muon p_{T} spectrum",
		      100, 0., 100.) ;
  recoMu_eta_ = fs->make<TH1F>("recoMu_eta_", "Muon #eta spectrum",
		     100, -3., 3.) ;
  l1passed_hist_ = fs->make<TH1F>("L1Passed_hist_", "Muon counts per event having passed L1 ", 
			     200, 0., 200.) ;
  mu_L1Passed_pT_ = fs->make<TH1F>("mu_L1Passed_pT_", "Muon p_{T} spectrum having passed L1",
			     100, 0., 100.) ;
  mu_L1Passed_eta_ = fs->make<TH1F>("mu_L1Passed_eta_", "Muon #eta spectrum having paassed L1",
			      100, -3., 3.) ;
  mu_L1_SingleMu7_pT_ = fs->make<TH1F>("mu_L1_SingleMu7_pT_", 
                                 "Muon p_{T} spectrum having passed L1_SingleMu7 bit", 
				  100, 0., 100.) ;
  mu_L1_SingleMu7_eta_ = fs->make<TH1F>("mu_L1_SingleMu7_eta_", 
                                  "Muon #eta spectrum having paassed L1_SingleMu7 bit",
			           100, -3., 3.) ;
  mu_L1_DoubleMu3_pT_ = fs->make<TH1F>("mu_L1_DoubleMu3_pT_", 
                                 "Muon p_{T} spectrum having passed L_DoubleMu3 bit",
			          100, 0., 100.) ;
  mu_L1_DoubleMu3_eta_ = fs->make<TH1F>("mu_L1DoubleMu3_eta_", 
  				  "Muon #eta spectrum having paassed L1_DoubleMu3 bit",
			           100, -3., 3.) ;
  hltPassed_hist_ = fs->make<TH1F>("HLTPassed_hist_", "Muo n counts per event having passed HLT", 
			      200, 0., 200.) ;
  mu_HLTPassed_pT_ = fs->make<TH1F>("mu_HLTPassed_pT_", "Muon p_{T} spectrum having passed HLT",
			      100, 0., 100.) ;
  mu_HLTPassed_eta_ = fs->make<TH1F>("mu_HLTPassed_eta_", "Muon #eta spectrum having passed HLT",
			       100, -3., 3.) ;
  mu_HLT2MuonJPsi_pT_ = fs->make<TH1F>("mu_HLT2MuonJPsi_pT_", 
				 "Muon p_{T} spectrum having passed HLT2muonJPsi bit",
				 100, 0., 100.) ;
  mu_HLT2MuonJPsi_eta_ = fs->make<TH1F>("mu_HLT2MuonJPsi_eta_", 
				  "Muon #eta spectrum having passed HLT2MuonJPsi bit",
				  100, -3., 3.) ;
  mu_HLT2MuonNonIso_pT_ = fs->make<TH1F>("mu_HLT2MuonNonIso_pT_", 
				 "Muon p_{T} spectrum having passed HLT2muonNonIso bit",
				 100, 0., 100.) ;
  mu_HLT2MuonNonIso_eta_ = fs->make<TH1F>("mu_HLT2MuonNonIso_eta_", 
				  "Muon #eta spectrum having passed HLT2MuonNonIso bit",
				  100, -3., 3.) ;
  mu_HLT2MuonIso_pT_ = fs->make<TH1F>("mu_HLT2MuonIso_pT_", 
				 "Muon p_{T} spectrum having passed HLT2MuonIso bit",
				 100, 0., 100.) ;
  mu_HLT2MuonIso_eta_ = fs->make<TH1F>("mu_HLT2Muonso_eta_", 
				  "Muon #eta spectrum having passed HLT2MuonIso bit",
				  100, -3., 3.) ;
  mu_HLT1MuonIso_pT_ = fs->make<TH1F>("mu_HLT1MuonIso_pT_", 
				 "Muon p_{T} spectrum having passed HLT1MuonIso bit",
				 100, 0., 100.) ;
  mu_HLT1MuonIso_eta_ = fs->make<TH1F>("mu_HLT1MuonIso_eta_", 
				  "Muon #eta spectrum having passed HLT1MuonIso bit",
				  100, -3., 3.) ;
  mu_HLT1MuonNonIso_pT_ = fs->make<TH1F>("mu_HLT1MuonNonIso_pT_", 
				 "Muon p_{T} spectrum having passed HLT1MuonNonIso bit",
				 100, 0., 100.) ;
  mu_HLT1MuonNonIso_eta_ = fs->make<TH1F>("mu_HLT1MuonNonIso_eta_", 
				  "Muon #eta spectrum having passed HLT1MuonNonIso bit",
				  100, -3., 3.) ;
  mu_HLT2MuonUpsilon_pT_ = fs->make<TH1F>("mu_HLT2MuonUpsilon_pT_", 
				 "Muon p_{T} spectrum having passed HLT2MuonUpsilon bit",
				 100, 0., 100.) ;
  mu_HLT2MuonUpsilon_eta_ = fs->make<TH1F>("mu_HLT2MuonUpsilon_eta_", 
				  "Muon #eta spectrum having passed HLT2MuonUpsilon bit",
				  100, -3., 3.) ;
  mu_HLTBJPsiMuMu_pT_ = fs->make<TH1F>("mu_HLTBJPsiMuMu_pT_", 
				       "Muon p_{T} spectrum having passed HLTBJPsiMuMu it",
				       100, 0., 100.) ;
  mu_HLTBJPsiMuMu_eta_ = fs->make<TH1F>("mu_HLTBJPsiMuMu_eta_", 
					"Muon #eta spectrum having passed HLTBJPsiMuMu bit",
				        100, -3., 3.) ;
  maxMuon_pT_ = fs->make<TH1F>("maxMuon_pT_",
			       "p_{T} spectrum of highest energy mouns in each event",
			       100, 0., 100.) ;
  l1_hltCorrelation_ = fs->make<TH2F>("l1_hltCorrelation_", "Correlation between L1 and HLT bits",
                                      130, 0, 130, 100, 0, 100) ;				  
  
  l1PathFlag = hltPathFlag = false ;
  sizeL1 = sizeHLT =0 ;
  nEvt = l1Accepts = hltAccepts = nL1AndHLT = 0;
  l1Names_.clear() ;

}

TriggerValidation::~TriggerValidation()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
   //     cout << " Here Comes the Destructor " << endl ;
  
  outfile.close() ;
  
  /*
  theFile->cd() ;
  theFile->Write() ;
  theFile->Close() ;
  */

}


//
// member functions
//

//Function to calculate and return the highest pT value of recoMuons
//  float TriggerValidation::highestPtMuon(const MuonCollection &muons){ 
  float TriggerValidation::highestPtMuon(vector<Muon>muons){ 
  	                                
//  float _maxMu_pT = 0. ;
//  float _mu_maxPt_eta ;
  vector<Muon>::iterator it_muons ;
  for(it_muons = muons.begin(); it_muons != muons.end(); it_muons++){
    TrackRef combinedMuTrack = it_muons->combinedMuon() ;
    CLHEP::HepLorentzVector combinedMuTrack4v(combinedMuTrack->px(),
                                       combinedMuTrack->py(),
                                       combinedMuTrack->pz(),
                                       sqrt((combinedMuTrack->p()*combinedMuTrack->p())
                                            + (muMass*muMass))) ;

    float mu_p = combinedMuTrack->p() ;
    float mu_theta = combinedMuTrack4v.theta() ;

    float mu_pT = mu_p*sin(mu_theta) ;
    if(mu_pT>maxMu_pT){
     maxMu_pT = mu_pT ;
     mu_maxPt_eta = -log(tan(mu_theta/2)) ;
   }

   return 0. ;

   }

 }

// ------------ method called to for each event  ------------
void
TriggerValidation::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  
//  ESHandle<MagneticField> theMagneticField ;
//  iSetup.get<IdealMagneticFieldRecord>().get(theMagneticField) ;
  
  maxMu_pT = mu_maxPt_eta = 0. ;

  nEvt++ ;
  
  iRun = iEvent.id().run() ;
  iEvt = iEvent.id().event() ;


  //get Generator Level info
  Handle<HepMCProduct> EvtHandle ;
  iEvent.getByLabel(l_genEvt, EvtHandle) ;
  const HepMC::GenEvent* Evt = EvtHandle->GetEvent() ;
   for(HepMC::GenEvent::particle_const_iterator // looping over all generated particles
	 it_gen = Evt->particles_begin(); 
       it_gen != Evt->particles_end(); 
       it_gen++){
     
     CLHEP::HepLorentzVector moth ;
     CLHEP::HepLorentzVector dau1 ;
     CLHEP::HepLorentzVector dau2 ;
     
     if((*it_gen)->status() != 3) continue ; // rejecting unstable generated particles
     
     HepMC::FourVector gen4v = (*it_gen)->momentum() ;
     moth = HepLorentzVector(gen4v.px(), gen4v.py(), gen4v.pz(), 
			     gen4v.e()) ;
     moth_pT_->Fill(moth.perp()) ; 
     moth_eta_->Fill(moth.eta()) ;
     
     if((*it_gen)->pdg_id()==443) { // loping over all generated J/psi

       jpsi_pT_->Fill(moth.perp()) ; jpsi_eta_->Fill(moth.eta()) ;
       
       unsigned int jpsi_noOfMuDaughters = 0 ;
     
       for(HepMC::GenVertex::particle_iterator it_child = // 
             (*it_gen)->end_vertex()->particles_begin(HepMC::descendants);
           it_child != (*it_gen)->end_vertex()->particles_end(HepMC::descendants);
           it_child++){
       
         if((*it_child)->pdg_id() == 13){ // mu+
	   HepMC::FourVector cld4v = (*it_child)->momentum() ;
           dau1 = HepLorentzVector(cld4v.px(), cld4v.py(), cld4v.pz(), cld4v.e()) ;
           dau1_pT_->Fill(dau1.perp()) ;
           dau1_eta_->Fill(dau1.eta()) ;
         }
         if((*it_child)->pdg_id() == -13){ //mu-
   	   HepMC::FourVector _cld4v = (*it_child)->momentum() ;
           dau2 = HepLorentzVector(_cld4v.px(), _cld4v.py(), _cld4v.pz(), _cld4v.e()) ;
           dau2_pT_->Fill(dau2.perp()) ;
           dau2_eta_->Fill(dau2.eta()) ;
         }
       
       }
  
       if((*it_gen)->pdg_id() != 443       // J/psi(1S)
  	  || (*it_gen)->pdg_id() != 553    // Y(2S)
	  || (*it_gen)->pdg_id() != 100443 // Psi(2S)
	  || (*it_gen)->pdg_id() != 10441  // Chi_c0(1P)
	  || (*it_gen)->pdg_id() != 20443  // Chi_c1(1P)
	  || (*it_gen)->pdg_id() != 445    // Chi_c2(1P)
       ) continue ;
     }

   }
   //get Generator Level info end


/*
   // get Track info
   Handle<TrackCollection> tracks;
   iEvent.getByLabel( "ctfWithMaterialTracks", tracks );
   for( TrackCollection::const_iterator it_tracks = tracks->begin(); 
	it_tracks != tracks->end(); ++ it_tracks ) {
     double track_pT = it_tracks->pt(), 
            track_eta = it_tracks->eta(), 
            track_phi = it_tracks->phi();
     track_pT_->Fill(track_pT);
     track_eta_->Fill(track_eta);
     track_phi_->Fill(track_phi);
   }
   //get Track info end
   
   //get Primary Vertices info
   Handle<VertexCollection> primaryVertices ;
   iEvent.getByLabel("offlinePrimaryVertices", primaryVertices) ;
   for(reco::VertexCollection::const_iterator it_vtces = primaryVertices->begin() ;
       it_vtces != primaryVertices->end(); it_vtces++ ){
       Hep3Vector primaryVtx(it_vtces->x(), it_vtces->y(), it_vtces->z()) ;
//       outfile << " 1ry Vertex dist frm interaction pt. " << primaryVtx.perp() << endl ;
   }
   //get PrimaryVertices info end
*/


   //get Muon info
   Handle<MuonCollection> muons ;
   iEvent.getByLabel("muons", muons) ;
   
   recoMu_hist_->Fill(muons->size()) ;
   
   const MuonCollection &muColl = (*(muons.product())) ;

   vector<Muon> muVect ;
   muVect.clear()  ;
   for(unsigned short it_muons = 0; it_muons < muColl.size(); it_muons++){
     muVect.push_back(muColl[it_muons]);
   }

   highestPtMuon(muVect) ; 

   if(muColl.size() > 0){
     maxMuon_pT_->Fill(maxMu_pT) ;
   }else maxMuon_pT_->Fill(0.) ;


   reco::MuonCollection::const_iterator it_muons ;
   float mu_p, mu_theta, mu_phi ;

   for(it_muons = muons->begin(); it_muons != muons->end(); it_muons++)
     { // looping over recoMuons
       // mu track info
       TrackRef combinedMuTrack = it_muons->combinedMuon() ; 
       HepLorentzVector combinedMuTrack4v(combinedMuTrack->px(), 
					  combinedMuTrack->py(), 
					  combinedMuTrack->pz(),
					  sqrt((combinedMuTrack->p()*combinedMuTrack->p())
					       + (muMass*muMass))) ;
       
       mu_p = combinedMuTrack->p() ;
//     mu_p = combinedMuTrack->charge()*combinedMuTrack->p() ;
       mu_theta = combinedMuTrack4v.theta() ;
       mu_phi = combinedMuTrack4v.phi() ;
       recoMu_pT_->Fill(mu_p*sin(mu_theta)) ;
       recoMu_eta_->Fill(-log(tan(mu_theta/2))) ;

/*
       GlobalPoint mu_position(combinedMuTrack->vertex().x(),
			    combinedMuTrack->vertex().y(),
			    combinedMuTrack->vertex().z());

       GlobalVector mu_momentum(combinedMuTrack->momentum().x(),
			     combinedMuTrack->momentum().y(),
			     combinedMuTrack->momentum().z());

       GlobalTrajectoryParameters mu_glbTrjPrm(mu_position, mu_momentum, 
					       combinedMuTrack->charge(), 
					       &(*theMagneticField)) ;

       TransientTrack muTranTrk(*combinedMuTrack, theMagneticField.product()) ;

//       iteration of all recoMuons again to get dist betwn 2 muon tracks
       reco::MuonCollection::const_iterator it2_muons ;
       for(it2_muons = it_muons+1; it2_muons != muons->end(); it2_muons++){

	 // mu2 track info
	 TrackRef combinedMu2Track = it2_muons->combinedMuon() ; 
	 HepLorentzVector 
	 combinedMu2Track4v(combinedMu2Track->px(), 
			    combinedMu2Track->py(), 
			    combinedMu2Track->pz(),
			    sqrt((combinedMu2Track->p()*combinedMu2Track->p())
				 + (muMass*muMass))) ;
	 
	 float mu2_p = combinedMu2Track->charge()*combinedMu2Track->p() ;
	 float mu2_theta = combinedMu2Track4v.theta() ;
	 float mu2_phi = combinedMu2Track4v.phi() ;
	 
	 GlobalPoint mu2_position(combinedMu2Track->vertex().x(),
				  combinedMu2Track->vertex().y(),
				  combinedMu2Track->vertex().z());
	 
	 GlobalVector mu2_momentum(combinedMu2Track->momentum().x(),
				   combinedMu2Track->momentum().y(),
				   combinedMu2Track->momentum().z());
	 
	 GlobalTrajectoryParameters mu2_glbTrjPrm(mu2_position, mu2_momentum, 
						  combinedMu2Track->charge(), 
						  &(*theMagneticField)) ;
	 
	 TransientTrack mu2TranTrk(*combinedMu2Track, theMagneticField.product()) ;
	 
	 // now to calculate separation between two muon tracks
	 TwoTrackMinimumDistanceHelixHelix _muMuHelixPathDist ;
	 bool merge = _muMuHelixPathDist.calculate(mu_glbTrjPrm, mu2_glbTrjPrm, .001) ;
	 pair<GlobalPoint, GlobalPoint> _muMuGlobalPointPair ;
	 muMuHelixPathDist = -10. ;
	 if(!merge){
	   _muMuGlobalPointPair = _muMuHelixPathDist.points() ;
	   muMuHelixPathDist = 
	     (_muMuGlobalPointPair.first - _muMuGlobalPointPair.second).mag() ;
	 }
	 
	 vector<TransientTrack> trnsTrkVect ;
	 trnsTrkVect.push_back(muTranTrk) ;
	 trnsTrkVect.push_back(mu2TranTrk) ;
	 
	 KalmanVertexFitter klmnVrtxFttr ;
	 TransientVertex muMuTransientVertex = klmnVrtxFttr.vertex(trnsTrkVect) ;
	 if(muMuTransientVertex.isValid()){
	 VertexState muMuVtx = muMuTransientVertex.vertexState() ;
	 }
	 

	 for(reco::VertexCollection::const_iterator it_vtces = primaryVertices->begin() ;
	     it_vtces != primaryVertices->end(); it_vtces++ ){
	   Hep3Vector muVtx1ryVtxDiff = Hep3Vector(muMuVtx.position().x(), 
						   muMuVtx.position().y(),
						   muMuVtx.position().z()) -
	     Hep3Vector(it_vtces->x(), it_vtces->y(), it_vtces->z()) ;
	 }
 
	 
       }
     
       //mu energy deposit info
       MuonEnergy muEnergy = it_muons->calEnergy() ; 
       
       //mu isolation criteria
       const MuonIsolation& muIsolationR03 = it_muons->isolationR03() ; 
       const MuonIsolation& muIsolationR05 = it_muons->isolationR05() ; 
*/     

     }
   //get Muon info end


   //get L1 Trigger info
   Handle<L1GlobalTriggerReadoutRecord> l1gtrr ;
   
   try{ iEvent.getByLabel("hltGtDigis",l1gtrr) ; }catch(...){ outfile << " Invalid L1 " << endl ; } 
   
   //       try{ iEvent.getByLabel("l1extraparticleMap", l1gtrr) ; } 
   //       catch(...){ outfile << " Invalid L1 " << endl ; }
   
   //       try{ iEvent.getByLabel("l1GtEmulDigis", l1gtrr) ; } 
   //       catch(...){ outfile << " Invalid L1 " << endl ; }
   
   // All three of the above method valid: which to use? 

   int nL1size = l1gtrr->decisionWord().size();
   
   const bool l1accept(l1gtrr->decision()) ;
   
   if(l1accept){
     
     l1Accepts++ ;
//     mu_L1Passed_pT_->Fill(mu_p*sin(mu_theta)) ; // muons that pass L1 Trigger
//     mu_L1Passed_pT_->Fill(maxMu_pT) ; // muons that pass L1 Trigger

   }
   
   sizeL1 = l1gtrr->decisionWord().size() ;
   

   // getting names of L1 Trigger bits: 
   edm::Handle<L1GlobalTriggerObjectMapRecord> gtObjectMapRecord;
   iEvent.getByLabel("hltL1GtObjectMap", gtObjectMapRecord);
   
   const std::vector<L1GlobalTriggerObjectMap>& objMapVec = gtObjectMapRecord->gtObjectMap();

   if(!l1PathFlag){
     l1PathFlag = true ;
     for (std::vector<L1GlobalTriggerObjectMap>::const_iterator itMap = objMapVec.begin();
       itMap != objMapVec.end(); ++itMap) {
       int algoBit = (*itMap).algoBitNumber();
       std::string algoNameStr = (*itMap).algoName();
       l1NameMap[algoBit] = algoNameStr;
       if(nEvt==1) outfile << " L1 Bit " << algoBit << " L1 name " << algoNameStr << endl ;
     }
   } 
   //get L1 Trigger info end

   //get HLT info
   Handle<edm::TriggerResults> trigRes ;
   iEvent.getByLabel(l_HLT, trigRes) ;
   
   Handle<trigger::TriggerEvent> trgEvt ;
   iEvent.getByLabel("hltTriggerSummaryAOD", trgEvt) ;

   const TriggerObjectCollection& trgObj = trgEvt->getObjects() ;
   vector<TriggerObject>::const_iterator it_trgObj ;
   if(nEvt==1) outfile << " trgObj size " << trgObj.size() << endl ;
   for(it_trgObj = trgObj.begin(); it_trgObj != trgObj.end(); ++it_trgObj){
     int id = it_trgObj->id() ;
     float pt = it_trgObj->pt() ;
     float mass = it_trgObj->mass() ;
     if(nEvt==1) outfile << " trg obj id " << id << " trg obj pt " << pt  << " mass " << mass << endl ;
   }
   
   sizeHLT = trigRes->size() ;
   
   if(trigRes->accept()) {
     hltAccepts++ ;
//     mu_HLTPassed_pT_->Fill(maxMu_pT) ; // HLT-passed mu
   }
   
   edm::TriggerNames triggerNames(*trigRes) ;
   if(!hltPathFlag){
     hltPathFlag = true ;
     for(unsigned short ii = 0; ii != sizeHLT; ii++){
       if(nEvt==1) outfile << " Trigger bit " << ii << " name " << triggerNames.triggerName(ii) << endl ;
     }
   }

   //get HLT info end
  
   // Combined L1 and HLT info

   if( l1accept && trigRes->accept() ) nL1AndHLT++ ;   

   unsigned int hltPassed_hist_Marker = 0 ;
   
   for(unsigned int ii = 0; ii != sizeL1; ii++){

     l1BitFlag = 0 ; 

     l1Names_.push_back(l1NameMap[ii]) ;
     if(l1gtrr->decisionWord()[ii]) {
       l1BitFlag = 1 ;
       l1passed_hist_->Fill(ii) ; // event has passed ii'th L1-bit
//       outfile << " #@ l1 bit " << ii << " has passed @# " << endl ;
     
       if(ii == 44){
          mu_L1_SingleMu7_pT_->Fill(maxMu_pT) ;
          mu_L1_SingleMu7_eta_->Fill(mu_maxPt_eta) ;
       }
       
       if(ii == 51){
          mu_L1_DoubleMu3_pT_->Fill(maxMu_pT) ;
	  mu_L1_DoubleMu3_eta_->Fill(mu_maxPt_eta) ;
       }
     
     }

     for(unsigned int jj = 0; jj != sizeHLT; jj++){

       hltBitFlag = 0 ;

       std::string name = triggerNames.triggerName(jj) ;
       if(trigRes->accept(jj)){

         hltBitFlag = 1 ;
	 
	 if(l1gtrr->decisionWord()[ii] ) l1_hltCorrelation_->Fill(ii, jj) ;
				    
          if(hltPassed_hist_Marker == 0){
	  
	    hltPassed_hist_->Fill(jj); 

            fired[name] = trigRes->accept(jj) ;

            if( jj == 181 ){ // HLT bit HLT2MuonJPsi
               mu_HLT2MuonJPsi_pT_->Fill(maxMu_pT);
               mu_HLT2MuonJPsi_eta_->Fill(mu_maxPt_eta) ;
            }
	 
            if( jj == 58 ){ // HLT bit HLT2MuonNonIso
               mu_HLT2MuonNonIso_pT_->Fill(maxMu_pT);
               mu_HLT2MuonNonIso_eta_->Fill(mu_maxPt_eta) ;
            }

            if( jj == 99 ){ // HLT bit HLT2MuonIso
               mu_HLT2MuonIso_pT_->Fill(maxMu_pT);
               mu_HLT2MuonIso_eta_->Fill(mu_maxPt_eta) ;
            }
	 
            if( jj == 56 ){ // HLT bit HLT1MuonNonIso
               mu_HLT1MuonNonIso_pT_->Fill(maxMu_pT);
               mu_HLT1MuonNonIso_eta_->Fill(mu_maxPt_eta) ;
            }
	   
            if( jj == 55 ){ // HLT bit HLT1MuonIso
               mu_HLT1MuonIso_pT_->Fill(maxMu_pT);
               mu_HLT1MuonIso_eta_->Fill(mu_maxPt_eta) ;
            }

            if( jj == 169 ){ // HLT bit HLT2MuonUpsilon
               mu_HLT2MuonUpsilon_pT_->Fill(maxMu_pT);
               mu_HLT2MuonUpsilon_eta_->Fill(mu_maxPt_eta) ;
            }
	 
            if( jj == 55 ){ // HLT bit HLTBJPsiMuMU
               mu_HLTBJPsiMuMu_pT_->Fill(maxMu_pT);
               mu_HLTBJPsiMuMu_eta_->Fill(mu_maxPt_eta) ;
            }
	    
	 }
	 
       }
     	
     }
       
     hltPassed_hist_Marker = 1 ;
 
   }

} // ****end of analyze function****

// ------------ method called once each job just before starting event loop  ------------
void 
TriggerValidation::beginJob(const edm::EventSetup& iSetup)
{
  
/*	
  //get IdealMagneticFieldRecord
  edm::ESHandle<MagneticField> bField ;
  iSetup.get<IdealMagneticFieldRecord>().get(bField) ;

  SteppingHelixPropagator *helixProp = 
    new SteppingHelixPropagator(&*bField, anyDirection) ;
  helixProp->setMaterialMode(false) ;
  helixProp->applyRadX0Correction(true) ;
  Propagator *defProp = helixProp ;
  trackDetectorAssociator_.setPropagator(defProp) ;
*/  
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TriggerValidation::endJob() {
  //     cout << " End Job " << endl ;

 if (nEvt==1000){
   outfile << " ## L1 size = " << sizeL1 << " ## " << endl ;
   outfile << " ** L1Accepts = " << l1Accepts << " ** " << endl ;
   outfile << " @@ L1 Efficiency = " << float(l1Accepts*1./nEvt) << " @@ " << endl ;
   outfile << " ## HLT size = " << sizeHLT << " ## " << endl ;
   outfile << " ** HLTAccepts = " << hltAccepts << " ** " << endl ;
   outfile << " @@ HLT Efficiency = " << float(hltAccepts*1./nEvt) << " @@ " << endl ;
   outfile << " ** # of Events passing both L1 and HLT = " << nL1AndHLT << " ** " << endl ;
 }

  outfile << " *** NEvents =  " << nEvt << " *** " << endl ; 

//  mu_HLT2MuonJPsi_pT_->Divide(recoMu_pT_) ;

/*  
  theFile->cd() ;
  
  int ips = 111 ;
  TPostScript ps(theOutputPSFileName.c_str(), ips) ;

  int xsiz = 600;
  int ysiz = 800;
  ps.NewPage() ;

  TCanvas *c0 = new TCanvas("c0", " HLT turn-on curve as function of p_{T} ", xsiz, ysiz);

  c0->Divide(1,3) ;

  c0->cd(1) ;
  maxMuon_pT_->Draw() ;
  maxMuon_pT_->GetXaxis()->SetTitle("highest muon p_{T} in an event") ;

  c0->cd(2) ;
  mu_HLT1MuonIso_pT_->Divide(recoMu_pT_) ;
  mu_HLT1MuonIso_pT_->Draw() ;
  mu_HLT1MuonIso_pT_->GetXaxis()->SetTitle("highest muon p_{T} in an event") ;

  c0->cd(3) ;
  mu_HLT1MuonNonIso_pT_->Divide(recoMu_pT_) ;
  mu_HLT1MuonNonIso_pT_->Draw() ;
  mu_HLT1MuonNonIso_pT_->GetXaxis()->SetTitle("highest muon p_{T} in an event") ;

  c0->Update() ;

  ps.Close() ;
*/  

}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerValidation);

