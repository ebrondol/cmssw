import FWCore.ParameterSet.Config as cms

siPixelStubs = cms.EDProducer("SiPixelStubBuilder",
#    Clusters = cms.InputTag('siPixelClusters'),
     Clusters = cms.InputTag('siPhase2Clusters'),
     offlinestubs = cms.string('vectorHits'),
     maxStubs = cms.int32(999999999),
     Algorithm = cms.string('VectorHitBuilderAlgorithm'),
     CPE = cms.ESInputTag('StripCPEfromTrackAngleESProducer:StripCPEfromTrackAngle'),
)
