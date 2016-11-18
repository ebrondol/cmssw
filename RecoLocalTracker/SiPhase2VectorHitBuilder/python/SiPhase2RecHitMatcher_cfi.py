import FWCore.ParameterSet.Config as cms

SiPhase2RecHitMatcherESProducer = cms.ESProducer("SiPhase2RecHitMatcherESProducer",
     ComponentName = cms.string('SiPhase2VectorHitMatcher'),
     Clusters = cms.string('siPhase2Clusters'),
     offlinestubs = cms.string('vectorHits'),
     maxVectorHits = cms.int32(999999999),
     maxVectorHitsinaStack = cms.int32(999),
     Algorithm = cms.string('VectorHitBuilderAlgorithm'),
     CPE = cms.ESInputTag('StripCPEfromTrackAngleESProducer:StripCPEfromTrackAngle'),
#    NSigmaInside = cms.double(3.0)
)

