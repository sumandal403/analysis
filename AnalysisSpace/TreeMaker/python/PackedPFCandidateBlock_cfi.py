import FWCore.ParameterSet.Config as cms

packedPFCandidateBlock = cms.EDAnalyzer("PackedPFCandidateBlock",
  verbosity = cms.untracked.int32(1),
  pfCands   = cms.untracked.InputTag('packedPFCandidates')
)