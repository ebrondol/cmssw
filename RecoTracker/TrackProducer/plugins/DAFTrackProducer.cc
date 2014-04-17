#include "RecoTracker/TrackProducer/plugins/DAFTrackProducer.h"
// system include files
#include <memory>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"

#include "RecoTracker/Record/interface/MultiRecHitRecord.h"
#include "RecoTracker/SiTrackerMRHTools/interface/MultiRecHitCollector.h"
#include "RecoTracker/SiTrackerMRHTools/interface/SiTrackerMultiRecHitUpdator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "RecoTracker/Record/interface/MultiRecHitRecord.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"


DAFTrackProducer::DAFTrackProducer(const edm::ParameterSet& iConfig):
  KfTrackProducerBase(iConfig.getParameter<bool>("TrajectoryInEvent"),false),
  theAlgo(iConfig)
{
  setConf(iConfig);
  setSrc( consumes<TrackCandidateCollection>(iConfig.getParameter<edm::InputTag>( "src" )),
          consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>( "beamSpot" )),
          consumes<MeasurementTrackerEvent>(iConfig.getParameter<edm::InputTag>( "MeasurementTrackerEvent") ));
  setAlias( iConfig.getParameter<std::string>( "@module_label" ) );
  //register your products
  produces<reco::TrackCollection>().setBranchAlias( alias_ + "Tracks" );
  produces<reco::TrackExtraCollection>().setBranchAlias( alias_ + "TrackExtras" );
  produces<TrackingRecHitCollection>().setBranchAlias( alias_ + "RecHits" );
  produces<std::vector<Trajectory> >();
  produces<TrajTrackAssociationCollection>();
}


void DAFTrackProducer::produce(edm::Event& theEvent, const edm::EventSetup& setup)
{
  edm::LogInfo("DAFTrackProducer") << "Analyzing event number: " << theEvent.id() << "\n";
  
  // create empty output collections
  //
  std::auto_ptr<TrackingRecHitCollection>    outputRHColl (new TrackingRecHitCollection);
  std::auto_ptr<reco::TrackCollection>       outputTColl(new reco::TrackCollection);
  std::auto_ptr<reco::TrackExtraCollection>  outputTEColl(new reco::TrackExtraCollection);
  std::auto_ptr<std::vector<Trajectory> >    outputTrajectoryColl(new std::vector<Trajectory>);

  //
  //declare and get stuff to be retrieved from ES
  //
  edm::ESHandle<TrackerGeometry> theG;
  edm::ESHandle<MagneticField> theMF;
  edm::ESHandle<TrajectoryFitter> theFitter;
  edm::ESHandle<Propagator> thePropagator;
  edm::ESHandle<MeasurementTracker>  theMeasTk;
  edm::ESHandle<TransientTrackingRecHitBuilder> theBuilder;
  getFromES(setup,theG,theMF,theFitter,thePropagator,theMeasTk,theBuilder);
  //get additional es_modules needed by the DAF	
  edm::ESHandle<MultiRecHitCollector> measurementCollectorHandle;
  std::string measurementCollectorName = getConf().getParameter<std::string>("MeasurementCollector");
  setup.get<MultiRecHitRecord>().get(measurementCollectorName, measurementCollectorHandle);
  edm::ESHandle<SiTrackerMultiRecHitUpdator> updatorHandle;
  std::string updatorName = getConf().getParameter<std::string>("UpdatorName");	
  setup.get<MultiRecHitRecord>().get(updatorName, updatorHandle);	 

  //ERICA: look into RecoTracker/TrackProducer/src/GsfTrackProducerBase.cc also for &*mte
  edm::Handle<MeasurementTrackerEvent> mte;
  theEvent.getByToken(mteSrc_, mte);

  //
  //declare and get TrackColection to be retrieved from the event
  //
  AlgoProductCollection algoResults;
  reco::BeamSpot bs;
  try{
    edm::Handle<std::vector<Trajectory> > theTrajectoryCollection;
    getFromEvt(theEvent,theTrajectoryCollection,bs);
//    measurementCollectorHandle->updateEvent(theEvent);

    //
    //run the algorithm  
    //
    LogDebug("DAFTrackProducer") << "run the algorithm" << "\n";
    theAlgo.runWithCandidate(theG.product(), theMF.product(), *theTrajectoryCollection, &*mte,
                             theFitter.product(), theBuilder.product(), measurementCollectorHandle.product(), updatorHandle.product(),bs,algoResults);
  } catch (cms::Exception &e){ edm::LogInfo("DAFTrackProducer") << "cms::Exception caught!!!" << "\n" << e << "\n"; throw; }
  //
  //put everything in the event
  putInEvt(theEvent, thePropagator.product(), theMeasTk.product(), outputRHColl, outputTColl, outputTEColl, 
           outputTrajectoryColl, algoResults);
  LogDebug("DAFTrackProducer") << "end" << "\n";

}

void DAFTrackProducer::getFromEvt(edm::Event& theEvent,edm::Handle<TrajectoryCollection>& theTrajectoryCollection, reco::BeamSpot& bs)
{
  //
  //get the TrajectoryCollection from the event
  //

  edm::InputTag src_=getConf().getParameter<edm::InputTag>( "src" );
  theEvent.getByLabel(src_,theTrajectoryCollection );

  //get the BeamSpot
  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  theEvent.getByToken(bsSrc_,recoBeamSpotHandle);
  bs = *recoBeamSpotHandle;
}

