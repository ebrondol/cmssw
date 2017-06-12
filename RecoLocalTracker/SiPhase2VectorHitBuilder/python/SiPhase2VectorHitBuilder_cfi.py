import FWCore.ParameterSet.Config as cms

siPhase2VectorHits = cms.EDProducer("VectorHitBuilderEDProducer",
     Clusters = cms.string('siPhase2Clusters'),
     offlinestubs = cms.string('vectorHits'),
     maxVectorHits = cms.int32(999999999),
     maxVectorHitsinaStack = cms.int32(999),
     Algorithm = cms.string('VectorHitBuilderAlgorithm'),
     BarrelCut = cms.vdouble( 0., 0.5, 0.6, 0.8, 0.9, 1.2, 2.), #layers are 6
     EndcapCut = cms.vdouble( 0., 1., 1., 1., 1., 1.), #disks are 5
     CPE = cms.ESInputTag("phase2StripCPEESProducer", "Phase2StripCPE")
)
