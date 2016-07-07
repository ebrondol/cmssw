#include "RecoTracker/MeasurementDet/interface/SeedingOTEDProducer.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "RecoTracker/Record/interface/CkfComponentsRecord.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimator.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

SeedingOTEDProducer::SeedingOTEDProducer(edm::ParameterSet const& conf):
  tkMeasEvent( consumes<MeasurementTrackerEvent>(conf.getParameter<edm::InputTag>("trackerEvent")) )
{
  std::cout << "SeedingOT::init" << std::endl;
  cpe = conf.getParameter<std::string>("phase2OTCPE");
  vhProducer = consumes< VectorHitCollectionNew >(edm::InputTag(conf.getParameter<edm::InputTag>("src")));
  produces< VectorHitCollectionNew >();
}
    
SeedingOTEDProducer::~SeedingOTEDProducer() {
}

void SeedingOTEDProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPhase2VectorHits","vectorHitsAccepted"));
  desc.add<edm::InputTag>("trackerEvent", edm::InputTag("MeasurementTrackerEvent"));
  desc.add<std::string>("phase2OTCPE", std::string("Phase2StripCPEGeometric"));
  descriptions.add("SeedingOTEDProducer", desc);
}
    
void SeedingOTEDProducer::produce(edm::Event& event, const edm::EventSetup& es)
{
  std::cout << "SeedingOT::produce() begin" << std::endl;

  edm::ESHandle<TrackerTopology> tTopoHandle;
  es.get<TrackerTopologyRcd>().get(tTopoHandle);
  tkTopo = tTopoHandle.product();

  edm::ESHandle<MeasurementTracker> measurementTrackerHandle;
  es.get<CkfComponentsRecord>().get(measurementTrackerHandle);
  edm::Handle<MeasurementTrackerEvent> measurementTrackerEvent;
  event.getByToken(tkMeasEvent,measurementTrackerEvent);

  const DetLayer* barrelOTLayer2 = measurementTrackerHandle->geometricSearchTracker()->tobLayers().at(1);
  layerMeasurements = new LayerMeasurements(*measurementTrackerHandle, *measurementTrackerEvent);
 
  edm::ESHandle<Chi2MeasurementEstimatorBase> est;
  es.get<TrackingComponentsRecord>().get("Chi2",est);
  estimator = est.product();

  edm::ESHandle<Propagator> prop;
  es.get<TrackingComponentsRecord>().get("PropagatorWithMaterial",prop);
  propagator = prop.product();

  edm::ESHandle<MagneticField> magFieldHandle;
  es.get<IdealMagneticFieldRecord>().get(magFieldHandle);
  magField = magFieldHandle.product();

  // Get the vector hits
  edm::Handle< VectorHitCollectionNew > vhs;
  event.getByToken(vhProducer, vhs);

  edm::ESHandle< Phase2StripCPEGeometric > parameterestimator;
  es.get<TkStripCPERecord>().get(cpe, parameterestimator); 
  const Phase2StripCPEGeometric & cpeOT(*parameterestimator);

  run(vhs, cpeOT, barrelOTLayer2);

  std::cout << "SeedingOT::produce() end" << std::endl;

}

void SeedingOTEDProducer::run(edm::Handle< VectorHitCollectionNew > VHs, 
                              const Phase2StripCPEGeometric & cpe,
                              const DetLayer* layerSearch){

  const VectorHitCollectionNew& input = *VHs;

  //seeds are built in the L3 of the OT
  std::vector<VectorHit> VHseeds;
  if (input.size() > 0 ) {  
    std::cout << "initial VH collection size = " << input.size() << std::endl;
    for (auto DSViter : input) {
      std::cout << "the ID from the DSViter = " << DSViter.id() << std::endl; 
      std::cout << "the Layer is " << checkLayer(DSViter.id()) << std::endl; 
      if(checkLayer(DSViter.id()) == 3){
        for(auto vh : DSViter)
          VHseeds.push_back(vh);
      }
    }
  }


  for(auto seeds : VHseeds){

  }

  return;
}

unsigned int SeedingOTEDProducer::checkLayer( unsigned int iidd ){

  StripSubdetector strip = StripSubdetector(iidd);
  unsigned int subid = strip.subdetId();
  if (subid ==  StripSubdetector::TIB || subid ==  StripSubdetector::TOB) { 
    return tkTopo->layer(iidd);
  }

  return 0;

}
