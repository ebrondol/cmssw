import FWCore.ParameterSet.Config as cms

siPixelVectorHits = cms.EDProducer("SiPixelVectorHitBuilder",
#    Clusters = cms.InputTag('siPixelClusters'),
     Clusters = cms.InputTag('siPhase2Clusters'),
     offlinestubs = cms.string('vectorHits'),
     maxVectorHits = cms.int32(999999999),
     maxVectorHitsinaStack = cms.int32(999),
     Algorithm = cms.string('VectorHitBuilderAlgorithm'),
     CPE = cms.ESInputTag('StripCPEfromTrackAngleESProducer:StripCPEfromTrackAngle'),
)
