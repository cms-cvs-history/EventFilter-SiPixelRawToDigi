import FWCore.ParameterSet.Config as cms

siPixelDigis = cms.EDFilter("SiPixelRawToDigi",
    Timing = cms.untracked.bool(False),
    IncludeErrors = cms.untracked.bool(False),
    InputLabel = cms.untracked.InputTag("siPixelRawData"),
    CheckPixelOrder = cms.untracked.bool(False),
    UseCablingTree = cms.untracked.bool(True)
)



