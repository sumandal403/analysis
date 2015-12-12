import FWCore.ParameterSet.Config as cms

photonBlock = cms.EDProducer("PhotonBlock",
  verbosity = cms.untracked.int32(1),
  photonSrc = cms.untracked.InputTag('slimmedPhotons')
)
