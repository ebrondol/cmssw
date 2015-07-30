# Imports
import FWCore.ParameterSet.Config as cms

# Create a new CMS process
process = cms.Process('cluTest')

# Import all the necessary files
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2023MuondevReco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.Validation_cff')
process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Geometry.TrackerNumberingBuilder.trackerTopologyConstants_cfi')

# Number of events (-1 = all)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input file
process.source = cms.Source('PoolSource',
#    fileNames = cms.untracked.vstring('file:step3_newclusters_SLHC25_skip6.root')
    fileNames = cms.untracked.vstring('file:step3_newclusters_SLHC25.root')
)

# TAG
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgradePLS3', '')

# Output
process.TFileService = cms.Service('TFileService',
    fileName = cms.string('file:vh_validation.root')
)

# DEBUG
#process.MessageLogger = cms.Service('MessageLogger',
#	debugModules = cms.untracked.vstring('siPhase2Clusters'),
#	destinations = cms.untracked.vstring('cout'),
#	cout = cms.untracked.PSet(
#		threshold = cms.untracked.string('ERROR')
#	)
#)

# Analyzer
process.analysis = cms.EDAnalyzer('VectorHitsBuilderValidation',
    src = cms.InputTag("siPhase2Clusters"),
    src2 = cms.InputTag("siPixelStubs", "vectorHitsAccepted"),
    links = cms.InputTag("simSiPixelDigis")
)

# Processes to run
process.p = cms.Path(process.analysis)

from SLHCUpgradeSimulations.Configuration.phase2TkCustomsBE5DPixel10Ddev import customise_condOverRides
process = customise_condOverRides(process)

