import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	    #/RelValJpsiMM_Pt_0_20/CMSSW_2_2_0_pre1_STARTUP_V7_v1/GEN-SIM-RECO 
#'/store/relval/CMSSW_2_2_0_pre1/RelValJpsiMM_Pt_0_20/GEN-SIM-RECO/STARTUP_V7_v1/0037/609089DC-00B0-DD11-A137-001A928116BA.root',
#'/store/relval/CMSSW_2_2_0_pre1/RelValJpsiMM_Pt_0_20/GEN-SIM-RECO/STARTUP_V7_v1/0038/B09568B3-04B0-DD11-9238-003048679180.root',
#'/store/relval/CMSSW_2_2_0_pre1/RelValJpsiMM_Pt_0_20/GEN-SIM-RECO/STARTUP_V7_v1/0041/14EECFF0-CFB0-DD11-A1A2-003048679162.root'

#/RelValUpsMM/CMSSW_2_2_0_pre1_STARTUP_V7_v1/GEN-SIM-RECO 
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/204CCBD6-25B0-DD11-8E3B-0018F3D096BE.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/30364C5B-26B0-DD11-AB61-0030486792C4.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/44222D70-2BB0-DD11-AF2E-0030486792C8.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/766D2BD6-2BB0-DD11-8010-00304867902E.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/9EE708F4-2EB0-DD11-AFBB-003048678FE4.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0038/C647A07B-2DB0-DD11-855C-0030486638E2.root',
'/store/relval/CMSSW_2_2_0_pre1/RelValUpsMM/GEN-SIM-RECO/STARTUP_V7_v1/0041/6EB3247C-D0B0-DD11-947F-0018F3D09614.root'
			     
        )
)

process.MessageLogger = cms.Service("MessageLogger")

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('hltVal220pre1STARTUP_V7_v1RelValUpsMM_Pt_0_20.root')
)

process.demo = cms.EDAnalyzer("TriggerValidation",
    OutfileName = cms.untracked.string('hltVal220pre1STARTUP_V7_v1RelValUpsMM_Pt_0_20.out'),
    muon = cms.InputTag('muons'),
    genEvt = cms.InputTag('generator'),
    hlt = cms.InputTag("TriggerResults","","HLT"),
    theHLTPath_1 = cms.string('HLT_DoubleIsoMu3'),
    theHLTPath_2 = cms.string('HLT_DoubleMu3'),
    theHLTPath_3 = cms.string('HLT_DoubleMu3JPsi'),
    theHLTPath_4 = cms.string('HLT_DoubleMu3Vtx2mm'),
    PSFileName = cms.untracked.string('hltVal220pre1STARTUP_V7_v1RelValUpsMM_Pt_0_20.ps')
)

process.p = cms.Path(process.demo)

