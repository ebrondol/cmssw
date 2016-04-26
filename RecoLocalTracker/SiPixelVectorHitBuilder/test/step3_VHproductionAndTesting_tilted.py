import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2023TiltedTKReco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.Validation_cff')
process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
#adding only recolocalreco
process.load('RecoLocalTracker.Configuration.RecoLocalTracker_cff')

# import VectorHitBuilder                                                                                                                                                      
process.load('RecoLocalTracker.SiPixelVectorHitBuilder.SiPixelVectorHitBuilder_cfi')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(500)
)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('file:step2_tilted_newDigis_500events.root'),
    skipEvents = cms.untracked.uint32(0)
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('step3 nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring( ('keep *_*_*_*') ),
    fileName = cms.untracked.string('file:step3_tilted_VHs.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    )
)

# debug
process.MessageLogger = cms.Service("MessageLogger",
                                    destinations = cms.untracked.vstring("debugVH_tilted"),
                                    debugModules = cms.untracked.vstring("*"),
                                    categories = cms.untracked.vstring("SiPixelVectorHitBuilder","VectorHitBuilderAlgorithm","VectorHitsBuilderValidation"),
                                    debugVH_tilted = cms.untracked.PSet(threshold = cms.untracked.string("DEBUG"),
                                                                       DEBUG = cms.untracked.PSet(limit = cms.untracked.int32(0)),
                                                                       default = cms.untracked.PSet(limit = cms.untracked.int32(0)),
                                                                       SiPixelVectorHitBuilder = cms.untracked.PSet(limit = cms.untracked.int32(-1)),
                                                                       VectorHitBuilderAlgorithm = cms.untracked.PSet(limit = cms.untracked.int32(-1)),
                                                                       VectorHitsBuilderValidation = cms.untracked.PSet(limit = cms.untracked.int32(-1))
                                                                       )
                                    )

# Analyzer
process.analysis = cms.EDAnalyzer('VectorHitsBuilderValidation',
    src = cms.InputTag("siPhase2Clusters"),
    src2 = cms.InputTag("siPixelVectorHits", "vectorHitsAccepted"),
    links = cms.InputTag("simSiPixelDigis")
)

# Output
process.TFileService = cms.Service('TFileService',
    fileName = cms.string('file:vh_validation_tilted.root')
)


# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgradePLS3', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.trackerlocalreco_step  = cms.Path(process.trackerlocalreco+process.siPixelVectorHits)
process.analysis_step = cms.Path(process.analysis)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.trackerlocalreco_step,process.analysis_step,process.FEVTDEBUGHLToutput_step)
#process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.prevalidation_step,process.validation_step,process.dqmoffline_step,process.FEVTDEBUGHLToutput_step,process.DQMoutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.combinedCustoms
from SLHCUpgradeSimulations.Configuration.combinedCustoms import cust_2023TiltedTK,cust_removeTTI 

#call to customisation function cust_2023TiltedTK imported from SLHCUpgradeSimulations.Configuration.combinedCustoms
process = cust_2023TiltedTK(process)

#call to customisation function cust_removeTTI imported from SLHCUpgradeSimulations.Configuration.combinedCustoms
process = cust_removeTTI(process)

# Automatic addition of the customisation function from SimGeneral.MixingModule.fullMixCustomize_cff
from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn 

#call to customisation function setCrossingFrameOn imported from SimGeneral.MixingModule.fullMixCustomize_cff
process = setCrossingFrameOn(process)

# End of customisation functions
