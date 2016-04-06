import FWCore.ParameterSet.Config as cms

tpClusterProducer = cms.EDProducer("ClusterTPAssociationProducer",
  verbose = cms.bool(False),
  simTrackSrc       = cms.InputTag("g4SimHits"),
  pixelSimLinkSrc   = cms.InputTag("simSiPixelDigis", "Pixel"),
  stripSimLinkSrc   = cms.InputTag("simSiStripDigis"),
  phase2SimLinkSrc  = cms.InputTag("simSiPixelDigis","Tracker"),
  pixelClusterSrc   = cms.InputTag("siPixelClusters"),
  stripClusterSrc   = cms.InputTag("siStripClusters"),
  phase2ClusterSrc  = cms.InputTag("siPhase2Clusters"),
  trackingParticleSrc = cms.InputTag('mix', 'MergedTrackTruth')
)
