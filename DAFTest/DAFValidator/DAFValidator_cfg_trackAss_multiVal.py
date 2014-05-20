import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

# source
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-RECO/POSTLS171_V1-v1/00000/980D6268-26B6-E311-AA4D-0025905A6136.root'
       ] );
secFiles.extend( [
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/0AD53A78-F5B5-E311-BF6A-00248C55CC9D.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/1CB3FB16-F6B5-E311-842C-0025905A613C.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/AA4515DD-F5B5-E311-834B-0025905A6076.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/AE114155-F5B5-E311-9837-002618943874.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/F85DDD54-F5B5-E311-98E0-002354EF3BCE.root'
       ] );
process.source = source


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
process.load("SimTracker.TrackAssociation.quickTrackAssociatorByHits_cfi")
process.load("DQMServices.Components.EDMtoMEConverter_cff")
process.load("Validation.Configuration.postValidation_cff")
process.quickTrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')

### Multi Validation
process.load("SimGeneral.MixingModule.mixNoPU_cfi")
process.load("Validation.RecoTrack.MultiTrackValidator_cff")
process.multiTrackValidator.associators = ['TrackAssociatorByHits']
process.multiTrackValidator.outputFile = 'multitrackvalidator_DAF.root'
process.multiTrackValidator.label = ['ctfWithMaterialTracksDAF']

### CTFFinalFitWithMaterialDAF
process.load("RecoTracker.TrackProducer.CTFFinalFitWithMaterialDAF_cff")
process.ctfWithMaterialTracksDAF.TrajectoryInEvent = True
process.ctfWithMaterialTracksDAF.src = 'TrackRefitter'
process.MRHFittingSmoother.EstimateCut = -1
process.MRHFittingSmoother.MinNumberOfHits = 3


process.demo = cms.EDAnalyzer('DAFValidator',
                tracks = cms.untracked.InputTag('ctfWithMaterialTracksDAF'),
		trackingParticleLabel = cms.InputTag('trackingParticles'),
)

process.TFileService = cms.Service("TFileService",
               fileName = cms.string('prova_multival.root')
)


process.p = cms.Path(process.MeasurementTrackerEvent*process.TrackRefitter
		     *process.ctfWithMaterialTracksDAF
		     *process.multiTrackValidator
		     *process.demo)
