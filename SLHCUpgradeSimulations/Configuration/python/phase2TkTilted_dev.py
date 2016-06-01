import FWCore.ParameterSet.Config as cms
#import SLHCUpgradeSimulations.Configuration.customise_PFlow as customise_PFlow

#GEN-SIM so far...
def customise(process):
    print "!!!You are using the SUPPORTED Tilted version of the Phase2 Tracker !!!"
    if hasattr(process,'DigiToRaw'):
        process=customise_DigiToRaw(process)
    if hasattr(process,'RawToDigi'):
        process=customise_RawToDigi(process)
    n=0
    if hasattr(process,'reconstruction') or hasattr(process,'dqmoffline_step'):
        if hasattr(process,'mix'):
            if hasattr(process.mix,'input'):
                n=process.mix.input.nbPileupEvents.averageNumber.value()
        else:
            print 'phase1TkCustoms requires a --pileup option to cmsDriver to run the reconstruction/dqm'
            print 'Please provide one!'
            sys.exit(1)
    if hasattr(process,'reconstruction'):
        process=customise_Reco(process,float(n))
    if hasattr(process,'digitisation_step'):
        process=customise_Digi(process)
    process=customise_condOverRides(process)

    return process

def customise_Digi(process):
    process.digitisation_step.remove(process.mix.digitizers.pixel)
    process.load('SimTracker.SiPhase2Digitizer.phase2TrackerDigitizer_cfi')
    process.mix.digitizers.pixel=process.phase2TrackerDigitizer
    process.mix.digitizers.pixel.PixelDigitizerAlgorithm.AddNoisyPixels = cms.bool(False) 
    process.mix.digitizers.pixel.PixelDigitizerAlgorithm.AddThresholdSmearing = cms.bool(False)
    process.mix.digitizers.pixel.PixelDigitizerAlgorithm.AddNoise = cms.bool(False)
    process.mix.digitizers.pixel.PixelDigitizerAlgorithm.AddXTalk = cms.bool(False) 
    process.mix.digitizers.strip.ROUList = cms.vstring("g4SimHitsTrackerHitsPixelBarrelLowTof",
                         'g4SimHitsTrackerHitsPixelEndcapLowTof')
    #Check if mergedtruth is in the sequence first, could be taken out depending on cmsDriver options
    if hasattr(process.mix.digitizers,"mergedtruth") :
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTIBLowTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTIBHighTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTOBLowTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTOBHighTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTECLowTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTECHighTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTIDLowTof"))
        process.mix.digitizers.mergedtruth.simHitCollections.tracker.remove( cms.InputTag("g4SimHits","TrackerHitsTIDHighTof"))

    # keep new digis
    alist=['FEVTDEBUG','FEVTDEBUGHLT','FEVT','RECOSIM']
    for a in alist:
        b=a+'output'
        if hasattr(process,b):
            getattr(process,b).outputCommands.append('keep Phase2TrackerDigiedmDetSetVector_*_*_*')
            getattr(process,b).outputCommands.append('keep *_simSiPixelDigis_*_*')
    return process


def customise_DigiToRaw(process):
    process.digi2raw_step.remove(process.siPixelRawData)
    process.digi2raw_step.remove(process.rpcpacker)
    return process

def customise_RawToDigi(process):
    process.raw2digi_step.remove(process.siPixelDigis)
    return process

def customise_Reco(process,pileup):
    # insert the new clusterizer
    process.load('SimTracker.SiPhase2Digitizer.phase2TrackerClusterizer_cfi')

    # added to produce phase2 DataFormats
    process.load("RecoLocalTracker.Phase2ITPixelClusterizer.Phase2ITPixelClusterizerPreSplitting_cfi")
    process.phase2ITPixelClustersPreSplitting.src = cms.InputTag('simSiPixelDigis', "Pixel")
    process.phase2ITPixelClustersPreSplitting.MissCalibrate = cms.untracked.bool(False)
    process.load("RecoLocalTracker.Phase2ITPixelRecHits.Phase2ITPixelRecHits_cfi")
    process.phase2ITPixelRecHitsPreSplitting.src = cms.InputTag("phase2ITPixelClustersPreSplitting")


    # PixelCPEGeneric #
    process.PixelCPEGenericESProducer.Upgrade = cms.bool(True)
    process.PixelCPEGenericESProducer.UseErrorsFromTemplates = cms.bool(False)
    process.PixelCPEGenericESProducer.LoadTemplatesFromDB = cms.bool(False)
    process.PixelCPEGenericESProducer.TruncatePixelCharge = cms.bool(False)
    process.PixelCPEGenericESProducer.IrradiationBiasCorrection = False
    process.PixelCPEGenericESProducer.DoCosmics = False
    process.PixelCPEGenericESProducer.useLAWidthFromDB = cms.bool(False)
    # CPE for other steps
    process.siPixelRecHits.CPE = cms.string('PixelCPEGeneric')

    # Phase2PixelCPEGeneric #
    process.load("RecoLocalTracker.Phase2ITPixelRecHits.Phase2PixelCPEESProducers_cff")
    process.Phase2PixelCPEGenericESProducer.useLAWidthFromDB = cms.bool(False)
    process.Phase2PixelCPEGenericESProducer.DoCosmics = cms.bool(False)
    # CPE for other steps
    process.phase2ITPixelRecHits.CPE = cms.string('Phase2PixelCPEGeneric')


    #process.load('RecoLocalTracker.SubCollectionProducers.jetCoreClusterSplitter_cfi')	
    #clustersTmp = 'siPixelClustersPreSplitting'
     # 0. Produce tmp clusters in the first place.
    #process.siPixelClustersPreSplitting = process.siPixelClusters.clone()
    #process.siPixelRecHitsPreSplitting = process.siPixelRecHits.clone()
    #process.siPixelRecHitsPreSplitting.src = clustersTmp
    #process.pixeltrackerlocalreco.replace(process.siPixelClusters, process.siPixelClustersPreSplitting)
    #process.pixeltrackerlocalreco.replace(process.siPixelRecHits, process.siPixelRecHitsPreSplitting)
    #process.clusterSummaryProducer.pixelClusters = clustersTmp

    #itIndex = process.pixeltrackerlocalreco.index(process.siPixelClustersPreSplitting)
    #process.pixeltrackerlocalreco.insert(itIndex, process.siPhase2Clusters)
    #process.pixeltrackerlocalreco.replace(process.siPixelClustersPreSplitting,process.phase2ITPixelClustersPreSplitting)
    #process.pixeltrackerlocalreco.replace(process.siPixelRecHitsPreSplitting,process.phase2ITPixelRecHitPreSplitting)


    process.pixeltrackerlocalreco = cms.Sequence(
        process.siPhase2Clusters 
      + process.phase2ITPixelClustersPreSplitting 
      + process.phase2ITPixelRecHitsPreSplitting 
        )
    process.trackerlocalreco.remove(process.clusterSummaryProducer)
    process.reconstruction_trackingOnly.remove(process.globalreco_tracking)

    # keep digis and new clusters
    alist=['RAWSIM','FEVTDEBUG','FEVTDEBUGHLT','GENRAW','RAWSIMHLT','FEVT','RECOSIM']
    for a in alist:
        b=a+'output'
        if hasattr(process,b):
            getattr(process,b).outputCommands.append('keep *_siPhase2Clusters_*_*')
            getattr(process,b).outputCommands.append('keep *_phase2ITPixelClusters_*_*')
            getattr(process,b).outputCommands.append('keep *_phase2ITPixelClustersPreSplitting_*_*')
            getattr(process,b).outputCommands.append('keep *_phase2ITPixelRecHits_*_*')               
            getattr(process,b).outputCommands.append('keep *_phase2ITPixelRecHitsPreSplitting_*_*')               
            getattr(process,b).outputCommands.append('keep *_simSiPixelDigis_*_*')
 
    return process

def customise_condOverRides(process):
    process.load('SLHCUpgradeSimulations.Geometry.fakeConditions_phase2TkTilted_cff')
    process.siPixelFakeLorentzAngleESSource.file = cms.FileInPath('SLHCUpgradeSimulations/Geometry/data/PhaseII/Tilted/PixelSkimmedGeometry_GeometryExtended2023Tilted.txt')
    return process



