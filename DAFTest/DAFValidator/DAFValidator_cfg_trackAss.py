import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

### conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

### standard includes (can't live without)
process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration.StandardSequences.Geometry_cff')
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

### Track Refitter
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")

### Track Association
process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")
process.load('SimTracker.TrackAssociation.TrackAssociatorByHits_cfi')
#process.load('SimGeneral.TrackingAnalysis.trackingParticles_cfi')

### CTFFinalFitWithMaterialDAF
process.load("RecoTracker.TrackProducer.CTFFinalFitWithMaterialDAF_cff")
process.ctfWithMaterialTracksDAF.TrajectoryInEvent = True
process.ctfWithMaterialTracksDAF.src = 'TrackRefitter'
process.MRHFittingSmoother.EstimateCut = -1
process.MRHFittingSmoother.MinNumberOfHits = 3

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#        'file:../../reco_trk_SingleMuPt10_UP15_10evts.root'
        'file:../../reco_trk_TTbar_13_1evts.root'
    )
)

process.demo = cms.EDAnalyzer('DAFValidator',
                tracks = cms.untracked.InputTag('ctfWithMaterialTracksDAF'),
		trackingParticleLabel = cms.InputTag('trackingParticles'),
)

process.TFileService = cms.Service("TFileService",
               fileName = cms.string('prova.root')
)


process.p = cms.Path(process.MeasurementTrackerEvent*process.TrackRefitter
		     *process.ctfWithMaterialTracksDAF
		     *process.demo)
