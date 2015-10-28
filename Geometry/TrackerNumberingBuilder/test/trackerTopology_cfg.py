import FWCore.ParameterSet.Config as cms

process = cms.Process("NumberingTest")
# empty input service, fire 10 events

process.load("FWCore.MessageLogger.MessageLogger_cfi")

upgradePhaseII = True

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
if (upgradePhaseII == False):
  process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
if (upgradePhaseII == True):
  process.load('Configuration.Geometry.GeometryExtended2023MuondevReco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#if (upgradePhaseII == False):
#  from Configuration.AlCa.GlobalTag import GlobalTag
#  process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')
#if (upgradePhaseII == True):
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgradePLS3', '')

process.load("Geometry.TrackerNumberingBuilder.trackerTopologyConstants_cfi")

# Choose Tracker Geometry
#process.load("Configuration.Geometry.GeometryReco_cff")

#process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
#process.load("Geometry.TrackerGeometryBuilder.idealForDigiTrackerGeometry_cff")
#process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cfi")

#process.load("Geometry.TrackerGeometryBuilder.trackerGeometry_cfi")

if (upgradePhaseII == True):
  process.es_prefer_trackerTopology = cms.ESPrefer("TrackerTopologyEP","trackerTopology2023Constants")

process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.prod = cms.EDAnalyzer("TrackerTopologyAnalyzer");


process.p1 = cms.Path(process.prod)


