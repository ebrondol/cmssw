# DPG tool to run with:
# - Tilted + Phase1 Pixel Geometry
#   cmsRun DPGAnalysis/SiStripTools/test/OccupancyPlotsTest_phase2_cfg.py print globalTag=auto:phase2_realistic inputFiles=file:step3.root triggerPath="*" geometry="tilted" 
# - Flat + Phase1 Pixel Geometry
#   cmsRun DPGAnalysis/SiStripTools/test/OccupancyPlotsTest_phase2_cfg.py print globalTag=auto:phase2_realistic inputFiles=file:step3.root triggerPath="*" geometry="flat"
# - Tilted + Phase2 Pixel Geometry
#   cmsRun DPGAnalysis/SiStripTools/test/OccupancyPlotsTest_phase2_cfg.py print globalTag=auto:phase2_realistic inputFiles=file:step3.root triggerPath="*" geometry="tilted4021"
# - Tilted + Phase2 Pixel Geometry (12 disks)
#   cmsRun DPGAnalysis/SiStripTools/test/OccupancyPlotsTest_phase2_cfg.py print globalTag=auto:phase2_realistic inputFiles=file:step3.root triggerPath="*" geometry="tilted4025"

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

process = cms.Process('OccupancyPlotsTest',eras.Phase2C1)

#prepare options

options = VarParsing.VarParsing("analysis")

options.register ('globalTag',
                  "DONOTEXIST",
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "GlobalTag")
options.register ('HLTprocess',
                  "HLT",
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "HLTProcess")
options.register ('triggerPath',
                  "HLT_*",
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "list of HLT paths")
options.register ('geometry',
                  "tilted",
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "Geometry")
options.parseArguments()

#

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    fileMode = cms.untracked.string("FULLMERGE")
    )

process.load("FWCore.MessageService.MessageLogger_cfi")

process.MessageLogger.destinations.extend(cms.vstring("detids"))
process.MessageLogger.categories.extend(cms.vstring("GeometricDetBuilding","DuplicateHitFinder","BuildingTrackerDetId","AlignmentTrackSelector",
                                                    "SubDetectorGeometricDetType","BuildingGeomDetUnits","LookingForFirstStrip",
                                                    "BuildingSubDetTypeMap","SubDetTypeMapContent","NumberOfLayers","IsThereTest","OccupancyPlots"))
process.MessageLogger.cout.placeholder = cms.untracked.bool(False)
process.MessageLogger.cout.threshold = cms.untracked.string("INFO")
process.MessageLogger.debugModules = cms.untracked.vstring("*")
process.MessageLogger.cout.default = cms.untracked.PSet(
    limit = cms.untracked.int32(0)
    )
process.MessageLogger.detids = cms.untracked.PSet(
    default = cms.untracked.PSet(
        limit = cms.untracked.int32(0)
        ),
    BuildingTrackerDetId = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    AlignmentTrackSelector = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    GeometricDetBuilding = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    SubDetectorGeometricDetType = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    BuildingGeomDetUnits = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    LookingForFirstStrip = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    BuildingSubDetTypeMap = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    SubDetTypeMapContent = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    NumberOfLayers = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    IsThereTest = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    OccupancyPlots = cms.untracked.PSet(
        limit = cms.untracked.int32(100000000)
        ),
    threshold = cms.untracked.string("DEBUG")
    )    
process.MessageLogger.cout.DuplicateHitFinder = cms.untracked.PSet(
    limit = cms.untracked.int32(100000000)
    )
process.MessageLogger.cout.FwkSummary = cms.untracked.PSet(
    limit = cms.untracked.int32(100000000)
    )
process.MessageLogger.cout.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(10000)
    )

process.MessageLogger.cerr.placeholder = cms.untracked.bool(False)
process.MessageLogger.cerr.threshold = cms.untracked.string("WARNING")
process.MessageLogger.cerr.default = cms.untracked.PSet(
    limit = cms.untracked.int32(10000000)
    )
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100000)
    )

#------------------------------------------------------------------

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source("PoolSource",
#                    fileNames = cms.untracked.vstring(options.inputFiles),
                    fileNames =  cms.untracked.vstring("/store/relval/CMSSW_9_1_0_pre1/RelValTTbar_14TeV/GEN-SIM-RECO/PU25ns_90X_upgrade2023_realistic_v9_D11PU200-v1/00000/001D34A8-711A-E711-B2EA-0025905A6090.root"),
#                    skipBadFiles = cms.untracked.bool(True),
                    inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*")
                    )

# HLT Selection ------------------------------------------------------------
process.load("HLTrigger.HLTfilters.triggerResultsFilter_cfi")
process.triggerResultsFilter.triggerConditions = cms.vstring(options.triggerPath)
process.triggerResultsFilter.hltResults = cms.InputTag( "TriggerResults", "", options.HLTprocess )
process.triggerResultsFilter.l1tResults = cms.InputTag( "" )
process.triggerResultsFilter.throw = cms.bool(False)

process.seqHLTSelection = cms.Sequence(process.triggerResultsFilter)
if options.triggerPath=="*":
    process.seqHLTSelection = cms.Sequence()


#--------------------------------------
#from DPGAnalysis.SiStripTools.occupancyplotsselections_cff import *
#from DPGAnalysis.SiStripTools.occupancyplotsselections_simplified_cff import *
#from DPGAnalysis.SiStripTools.occupancyplotsselections_phase2_cff import *
if options.geometry == "flat" :
  from DPGAnalysis.SiStripTools.occupancyplotsselections_phase2_flat_cff import *
if options.geometry == "tilted" :
  from DPGAnalysis.SiStripTools.occupancyplotsselections_phase2_tilted_cff import *
if options.geometry == "tilted4021" :
  from DPGAnalysis.SiStripTools.occupancyplotsselections_phase2_tilted4021_cff import *
if options.geometry == "tilted4025" :
  from DPGAnalysis.SiStripTools.occupancyplotsselections_phase2_tilted4025_detailed_cff import *

#process.ssclusmultprod = cms.EDProducer("SiStripClusterMultiplicityProducer",
#                                        clusterdigiCollection = cms.InputTag("siStripClusters"),
#                                        wantedSubDets = cms.VPSet()
#                                        )
#process.ssclusmultprod.wantedSubDets.extend(OccupancyPlotsStripWantedSubDets)
#
#process.ssclusoccuprod = cms.EDProducer("SiStripClusterMultiplicityProducer",
#                                        clusterdigiCollection = cms.InputTag("siStripClusters"),
#                                        withClusterSize = cms.untracked.bool(True),
#                                        wantedSubDets = cms.VPSet()
#                                        )
#process.ssclusoccuprod.wantedSubDets.extend(OccupancyPlotsStripWantedSubDets)

#multiplicity pixels
process.spclusmultprod = cms.EDProducer("SiPixelClusterMultiplicityProducer",
                                        clusterdigiCollection = cms.InputTag("siPixelClusters"),
                                        wantedSubDets = cms.VPSet()
                                        )
process.spclusmultprod.wantedSubDets.extend(OccupancyPlotsPixelWantedSubDets)
process.spclusmultprodontrack=process.spclusmultprod.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

process.spclusmultprodxy = process.spclusmultprod.clone()
process.spclusmultprodxy.wantedSubDets = OccupancyPlotsPXFDetailedWantedSubDets
process.spclusmultprodxyontrack=process.spclusmultprodxy.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

#multiplicity phase2OT
process.p2clusmultprod = cms.EDProducer("SiPhase2TrackerCluster1DMultiplicityProducer",
                                        clusterdigiCollection = cms.InputTag("siPhase2Clusters"),
                                        wantedSubDets = cms.VPSet()
                                        )
process.p2clusmultprod.wantedSubDets.extend(OccupancyPlotsOuterTrackerWantedSubDets)
process.p2clusmultprodontrack=process.p2clusmultprod.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

#occupancy pixels
process.spclusoccuprod = cms.EDProducer("SiPixelClusterMultiplicityProducer",
                                        clusterdigiCollection = cms.InputTag("siPixelClusters"),
                                        withClusterSize = cms.untracked.bool(True),
                                        wantedSubDets = cms.VPSet()
                                        )
process.spclusoccuprod.wantedSubDets.extend(OccupancyPlotsPixelWantedSubDets)
process.spclusoccuprodontrack=process.spclusoccuprod.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

process.spclusoccuprodxy = process.spclusoccuprod.clone()
process.spclusoccuprodxy.wantedSubDets = OccupancyPlotsPXFDetailedWantedSubDets
process.spclusoccuprodxyontrack=process.spclusoccuprodxy.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

#occupancy phase2OT
process.p2clusoccuprod = cms.EDProducer("SiPhase2TrackerCluster1DMultiplicityProducer",
                                        clusterdigiCollection = cms.InputTag("siPhase2Clusters"),
                                        withClusterSize = cms.untracked.bool(True),
                                        wantedSubDets = cms.VPSet()
                                        )
process.p2clusoccuprod.wantedSubDets.extend(OccupancyPlotsOuterTrackerWantedSubDets)
process.p2clusoccuprodontrack=process.p2clusoccuprod.clone(clusterdigiCollection = cms.InputTag("AlignmentTrackSelector"))

process.seqMultProd = cms.Sequence(process.p2clusmultprod + process.p2clusoccuprod +
                                   process.p2clusmultprodontrack + process.p2clusoccuprodontrack +
                                   process.spclusmultprod + process.spclusoccuprod +
                                   process.spclusmultprodontrack + process.spclusoccuprodontrack +
                                   process.spclusmultprodxy + process.spclusoccuprodxy +
                                   process.spclusmultprodxyontrack + process.spclusoccuprodxyontrack 
                                   )

process.load("DPGAnalysis.SiStripTools.occupancyplots_cfi")
#process.occupancyplots.wantedSubDets = OccupancyPlotsStripWantedSubDets
if options.geometry == "flat" :
  process.occupancyplots.file = cms.untracked.FileInPath("SLHCUpgradeSimulations/Geometry/data/PhaseII/Flat/PixelSkimmedGeometry.txt")
if options.geometry == "tilted" :
  process.occupancyplots.file = cms.untracked.FileInPath("SLHCUpgradeSimulations/Geometry/data/PhaseII/Tilted/PixelSkimmedGeometry.txt")
if options.geometry == "tilted4021" :
  process.occupancyplots.file = cms.untracked.FileInPath("SLHCUpgradeSimulations/Geometry/data/PhaseII/Tilted/PixelSkimmedGeometry4021.txt")
if options.geometry == "tilted4025" :
  process.occupancyplots.file = cms.untracked.FileInPath("SLHCUpgradeSimulations/Geometry/data/PhaseII/Tilted/PixelSkimmedGeometry4025.txt")
process.occupancyplots.checkWithLabels = cms.bool(True)

process.pixeloccupancyplots = process.occupancyplots.clone()
process.pixeloccupancyplots.wantedSubDets = process.spclusmultprod.wantedSubDets
process.pixeloccupancyplots.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprod"))
process.pixeloccupancyplots.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprod"))

process.pixeloccupancyxyplots = process.occupancyplots.clone()
process.pixeloccupancyxyplots.wantedSubDets = process.spclusmultprodxy.wantedSubDets
process.pixeloccupancyxyplots.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprodxy"))
process.pixeloccupancyxyplots.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprodxy"))

process.pixeloccupancyplotsontrack = process.occupancyplots.clone()
process.pixeloccupancyplotsontrack.wantedSubDets = process.spclusmultprodontrack.wantedSubDets
process.pixeloccupancyplotsontrack.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprodontrack"))
process.pixeloccupancyplotsontrack.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprodontrack"))

process.pixeloccupancyxyplotsontrack = process.pixeloccupancyxyplots.clone()
process.pixeloccupancyxyplotsontrack.wantedSubDets = process.spclusmultprodxyontrack.wantedSubDets
process.pixeloccupancyxyplotsontrack.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprodxyontrack"))
process.pixeloccupancyxyplotsontrack.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprodxyontrack"))

process.phase2occupancyplots = process.occupancyplots.clone()
process.phase2occupancyplots.wantedSubDets = process.spclusmultprod.wantedSubDets
process.phase2occupancyplots.multiplicityMaps = cms.VInputTag(cms.InputTag("p2clusmultprod"))
process.phase2occupancyplots.occupancyMaps = cms.VInputTag(cms.InputTag("p2clusoccuprod"))

process.phase2occupancyplotsontrack = process.occupancyplots.clone()
process.phase2occupancyplotsontrack.wantedSubDets = process.spclusmultprodontrack.wantedSubDets
process.phase2occupancyplotsontrack.multiplicityMaps = cms.VInputTag(cms.InputTag("p2clusmultprodontrack"))
process.phase2occupancyplotsontrack.occupancyMaps = cms.VInputTag(cms.InputTag("p2clusoccuprodontrack"))

process.alloccupancyplots = process.occupancyplots.clone()
process.alloccupancyplots.wantedSubDets = cms.VPSet()
process.alloccupancyplots.wantedSubDets.extend(OccupancyPlotsPixelWantedSubDets)
process.alloccupancyplots.wantedSubDets.extend(OccupancyPlotsOuterTrackerWantedSubDets)
process.alloccupancyplots.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprod"),cms.InputTag("p2clusmultprod"))
process.alloccupancyplots.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprod"),cms.InputTag("p2clusoccuprod"))

process.alloccupancyplotsontrack = process.occupancyplots.clone()
process.alloccupancyplotsontrack.wantedSubDets = cms.VPSet()
process.alloccupancyplotsontrack.wantedSubDets.extend(OccupancyPlotsPixelWantedSubDets)
process.alloccupancyplotsontrack.wantedSubDets.extend(OccupancyPlotsOuterTrackerWantedSubDets)
process.alloccupancyplotsontrack.multiplicityMaps = cms.VInputTag(cms.InputTag("spclusmultprodontrack"),cms.InputTag("p2clusmultprodontrack"))
process.alloccupancyplotsontrack.occupancyMaps = cms.VInputTag(cms.InputTag("spclusoccuprodontrack"),cms.InputTag("p2clusoccuprodontrack"))


#process.load("TrackingPFG.Utilities.bxlumianalyzer_cfi")

process.goodVertices = cms.EDFilter("VertexSelector",
   src = cms.InputTag("offlinePrimaryVertices"),
   cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),  
   filter = cms.bool(False),   # otherwise it won't filter the events, just produce an empty vertex collection.
)

process.load("Validation.RecoVertex.anotherprimaryvertexanalyzer_cfi")
process.primaryvertexanalyzer.pvCollection=cms.InputTag("goodVertices")
process.primaryvertexanalyzer.vHistogramMakerPSet.runHisto=cms.untracked.bool(False)
process.primaryvertexanalyzer.vHistogramMakerPSet.runHistoProfile=cms.untracked.bool(False)
process.primaryvertexanalyzer.vHistogramMakerPSet.runHistoBXProfile=cms.untracked.bool(False)

process.seqAnalyzers = cms.Sequence(
    #process.bxlumianalyzer +
    process.goodVertices + process.primaryvertexanalyzer +
#    process.occupancyplots +
    process.pixeloccupancyplots + process.pixeloccupancyplotsontrack +
    process.pixeloccupancyxyplots + process.pixeloccupancyxyplotsontrack +
    process.phase2occupancyplots + process.phase2occupancyplotsontrack +
    process.alloccupancyplots + process.alloccupancyplotsontrack
)

#-------------------------------------------------------------------------------------------

process.load("Alignment.CommonAlignmentProducer.AlignmentTrackSelector_cfi")
process.AlignmentTrackSelector.etaMin = cms.double(-5.)
process.AlignmentTrackSelector.etaMax = cms.double(5.)

process.seqProducers = cms.Sequence(process.AlignmentTrackSelector + process.seqMultProd)

process.load("DPGAnalysis.SiStripTools.trackcount_cfi")
process.trackcount.trackCollection = cms.InputTag("generalTracks")
process.trackcount.etaMin= cms.untracked.double(-4.)
process.trackcount.etaMax= cms.untracked.double(4.)
process.trackcount.netabin1D=cms.untracked.uint32(160)
process.trackcount.netabin2D=cms.untracked.uint32(50)
process.trackcount.nchi2bin1D=cms.untracked.uint32(1000)
process.trackcount.nndofbin1D=cms.untracked.uint32(200)
process.trackcount.nchi2bin2D=cms.untracked.uint32(400)
process.trackcount.nndofbin2D=cms.untracked.uint32(100)
process.trackcount.wanted2DHistos=cms.untracked.bool(True)

process.load("DPGAnalysis.SiStripTools.duplicaterechits_cfi")

#----GlobalTag ------------------------

process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")

if options.geometry == "tilted" :
  process.load('Configuration.Geometry.GeometryExtended2023D1Reco_cff')
if options.geometry == "flat" :
  process.load('Configuration.Geometry.GeometryExtended2023D2Reco_cff')
if options.geometry == "tilted4021" :
  process.load('Configuration.Geometry.GeometryExtended2023D4Reco_cff')
if options.geometry == "tilted4025" :
  process.load('Configuration.Geometry.GeometryExtended2023D11Reco_cff')

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, options.globalTag, '')
#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgradePLS3', '')


process.siStripQualityESProducer.ListOfRecordToMerge=cms.VPSet(
#    cms.PSet( record = cms.string("SiStripDetVOffRcd"),    tag    = cms.string("") ),
    cms.PSet( record = cms.string("SiStripDetCablingRcd"), tag    = cms.string("") ),
    cms.PSet( record = cms.string("RunInfoRcd"),           tag    = cms.string("") ),
    cms.PSet( record = cms.string("SiStripBadChannelRcd"), tag    = cms.string("") ),
    cms.PSet( record = cms.string("SiStripBadFiberRcd"),   tag    = cms.string("") ),
    cms.PSet( record = cms.string("SiStripBadModuleRcd"),  tag    = cms.string("") )
)

process.SiStripDetInfoFileReader = cms.Service("SiStripDetInfoFileReader")

process.TFileService = cms.Service('TFileService',
                                   fileName = cms.string('OccupancyPlotsTest_phase2_'+options.geometry+'_'+options.tag+'.root')
                                   )

#process = customise_Reco(process,0)
#process = customise_condOverRides(process)

process.myrereco = cms.Sequence(
    process.siPixelRecHits + 
    process.trackingGlobalReco)

process.p0 = cms.Path(
#    process.myrereco +
    process.seqHLTSelection +
    process.seqProducers +
    process.seqAnalyzers +
    process.trackcount +
    process.duplicaterechits 
    )


#print process.dumpPython()
