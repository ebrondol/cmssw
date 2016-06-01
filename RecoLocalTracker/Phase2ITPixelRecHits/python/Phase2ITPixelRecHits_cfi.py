import FWCore.ParameterSet.Config as cms

phase2ITPixelRecHits = cms.EDProducer("Phase2ITPixelRecHitConverter",
    src = cms.InputTag("phase2ITPixelClusters"),
    CPE = cms.string('Phase2PixelCPEGeneric'),
    VerboseLevel = cms.untracked.int32(0),

)

phase2ITPixelRecHitsPreSplitting = phase2ITPixelRecHits.clone(
    src = 'phase2ITPixelClustersPreSplitting'
)
