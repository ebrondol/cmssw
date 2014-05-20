import FWCore.ParameterSet.Config as cms

process = cms.Process("MULTITRACKVALIDATOR")

# message logger
process.MessageLogger = cms.Service("MessageLogger",
     default = cms.untracked.PSet( limit = cms.untracked.int32(10) )
)

# source
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-RECO/POSTLS171_V1-v1/00000/980D6268-26B6-E311-AA4D-0025905A6136.root' 
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-RECO/POSTLS171_V1-v1/00000/3E806F9A-4BB6-E311-A4D2-002618943935.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-RECO/POSTLS171_V1-v1/00000/66797485-44B6-E311-9924-002618943939.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-RECO/POSTLS171_V1-v1/00000/B4F97AB1-25B6-E311-A16B-003048FFD760.root' 
       ] );

secFiles.extend( [
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/0AD53A78-F5B5-E311-BF6A-00248C55CC9D.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/1CB3FB16-F6B5-E311-842C-0025905A613C.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/AA4515DD-F5B5-E311-834B-0025905A6076.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/AE114155-F5B5-E311-9837-002618943874.root',
       '/store/relval/CMSSW_7_1_0_pre5/RelValSingleMuPt10_UP15/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/F85DDD54-F5B5-E311-98E0-002354EF3BCE.root' 
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/0466F34F-2FB6-E311-B125-0026189438EA.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/40408552-2FB6-E311-B773-0025905A60A0.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/48164C0F-00B6-E311-8EEB-0025905A60CE.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/680935B9-FFB5-E311-9750-003048FFD71A.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/80154D86-32B6-E311-A72A-002618943858.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/AAFCD423-01B6-E311-A08C-003048FFD75C.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/E03BC81A-01B6-E311-9608-00261894385A.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/F2FC75BE-04B6-E311-BBAC-0025905A48BA.root',
#       '/store/relval/CMSSW_7_1_0_pre5/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/POSTLS171_V1-v1/00000/F44896B7-06B6-E311-96AC-0026189438D7.root'
       ] );

process.source = source
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

### conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

### standard includes
process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration.StandardSequences.Geometry_cff')
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

## includes for DAF
import RecoTracker.CkfPattern.CkfTrajectoryBuilderESProducer_cfi
import TrackingTools.TrajectoryCleaning.TrajectoryCleanerBySharedSeeds_cfi

process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
process.load("RecoTracker.TrackProducer.CTFFinalFitWithMaterialDAF_cff")
process.load("SimGeneral.MixingModule.mixNoPU_cfi")
# replace this with whatever track type you want to look at
process.TrackRefitter.TrajectoryInEvent = True
process.RKFittingSmoother.MinNumberOfHits=3
process.TrackRefitter.Fitter = "RKFittingSmoother"
process.load("RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilderWithoutRefit_cfi")

from RecoTracker.FinalTrackSelectors.TracksWithQuality_cff import *
process.ctfWithMaterialTracksDAF.TrajectoryInEvent = True
process.ctfWithMaterialTracksDAF.src = 'TrackRefitter'
process.MRHFittingSmoother.EstimateCut = -1
process.MRHFittingSmoother.MinNumberOfHits = 3

import copy
from TrackingTools.KalmanUpdators.Chi2MeasurementEstimatorESProducer_cfi import *
# Chi2MeasurementEstimatorESProducer
RelaxedChi2 = copy.deepcopy(Chi2MeasurementEstimator)
RelaxedChi2.ComponentName = 'RelaxedChi2'
RelaxedChi2.MaxChi2 = 100.

### validation-specific includes
#process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")
process.load("SimTracker.TrackAssociation.quickTrackAssociatorByHits_cfi")
process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")
process.load("Validation.RecoTrack.cuts_cff")
process.load("Validation.RecoTrack.MultiTrackValidator_cff")
process.load("DQMServices.Components.EDMtoMEConverter_cff")
process.load("Validation.Configuration.postValidation_cff")
process.quickTrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')

########### configuration MultiTrackValidator ########
process.multiTrackValidator.outputFile = 'multiprova.root'#multitrackvalidator_DAF_mupt10.root'
process.multiTrackValidator.associators = ['quickTrackAssociatorByHits']
process.multiTrackValidator.skipHistoFit=cms.untracked.bool(False)
#process.cutsRecoTracks.quality = cms.vstring('','highPurity')
#process.cutsRecoTracks.quality = cms.vstring('')
#process.multiTrackValidator.label = ['cutsRecoTracks']
process.multiTrackValidator.label = ['ctfWithMaterialTracksDAF']
#process.multiTrackValidator.useLogPt=cms.untracked.bool(True)
#process.multiTrackValidator.minpT = cms.double(0.1)
#process.multiTrackValidator.maxpT = cms.double(100.0)
#process.multiTrackValidator.nintpT = cms.int32(40)
#process.multiTrackValidator.UseAssociators = cms.bool(True)
#process.multiTrackValidator.runStandalone = cms.bool(True)

#process.quickTrackAssociatorByHits.useClusterTPAssociation = cms.bool(True)
#process.load("SimTracker.TrackerHitAssociation.clusterTpAssociationProducer_cfi")
process.load('SimTracker.TrackAssociation.TrackAssociatorByHits_cfi')
process.load('SimGeneral.TrackingAnalysis.trackingParticles_cfi')

process.demo = cms.EDAnalyzer('DAFValidator',
                tracks = cms.untracked.InputTag('ctfWithMaterialTracksDAF'),
                trackingParticleLabel = cms.InputTag("mix","MergedTrackTruth"),
		associator = cms.InputTag("TrackAssociatorByHits"),
		associatePixel = cms.bool(True),
		associateStrip = cms.bool(True),
		associateRecoTracks = cms.bool(True)
)

process.TFileService = cms.Service("TFileService",
               fileName = cms.string('multiprova_2_100evts.root')
)

process.validation = cms.Sequence(
#    process.tpClusterProducer *
    process.multiTrackValidator
)

# paths
process.p = cms.Path(
    process.MeasurementTrackerEvent * process.TrackRefitter
    * process.ctfWithMaterialTracksDAF
    * process.validation * process.demo
)
process.schedule = cms.Schedule(
      process.p
)


