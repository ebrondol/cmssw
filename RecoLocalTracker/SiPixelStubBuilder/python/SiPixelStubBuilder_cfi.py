import FWCore.ParameterSet.Config as cms
 
siPixelStubs = cms.EDProducer("SiPixelStubBuilder",
#    Clusters = cms.InputTag('siPixelClusters'),
     Clusters = cms.InputTag('siPhase2Clusters'), 
     phase2stripCPE = cms.ESInputTag('StripCPEfromTrackAngleESProducer:StripCPEfromTrackAngle'),
)

