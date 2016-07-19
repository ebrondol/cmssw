#include "RecoTracker/MeasurementDet/interface/SeedingOTEDProducer.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "RecoTracker/Record/interface/CkfComponentsRecord.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimator.h"

#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

SeedingOTEDProducer::SeedingOTEDProducer(edm::ParameterSet const& conf):
  tkMeasEvent( consumes<MeasurementTrackerEvent>(conf.getParameter<edm::InputTag>("trackerEvent")) )
{
  vhProducer = consumes< VectorHitCollectionNew >(edm::InputTag(conf.getParameter<edm::InputTag>("src")));
  produces< VectorHitCollectionNew >();
}
    
SeedingOTEDProducer::~SeedingOTEDProducer() {
}

void SeedingOTEDProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPhase2VectorHits","vectorHitsAccepted"));
  desc.add<edm::InputTag>("trackerEvent", edm::InputTag("MeasurementTrackerEvent"));
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
/*
  edm::ESHandle< ClusterParameterEstimator<Phase2TrackerCluster1D> > parameterestimator;
  es.get<TkStripCPERecord>().get(cpe, parameterestimator); 
  const Phase2StripCPEGeometric & cpeOT(*parameterestimator);
*/
  run(vhs, barrelOTLayer2);

  std::cout << "SeedingOT::produce() end" << std::endl;

}

void SeedingOTEDProducer::run(edm::Handle< VectorHitCollectionNew > VHs, 
                              const DetLayer* layerSearch){

  std::cout << "-----------------------------" << std::endl;
  std::vector<VectorHit> VHseeds = collectVHsInput(VHs);
  std::cout << "-----------------------------" << std::endl;
  std::cout << "VH seeds = " << VHseeds.size() << std::endl;

  for(auto seed : VHseeds){

    std::cout << "\tbuilding a seed for the VH: " << seed << std::endl;
    int charge = 1;
    LocalTrajectoryParameters ltpar(seed.localPosition(), seed.localDirection(), charge);
    //float pT = seed.transverseMomentum(magField);
    float p = seed.momentum(magField);
    float x = seed.localPosition().x();
    float y = seed.localPosition().y();
    float dx = seed.localDirection().x();
    float dy = seed.localDirection().y(); 
    LocalTrajectoryParameters ltpar2(1./p, dx, dy, x, y, charge);
    AlgebraicSymMatrix mat = assign44To55(seed.parametersError());
    //FIXME::set the error on 1/p
    mat[4][4] = 1000;
    std::cout << "\tltraj : " << 1./p <<","<< dx <<","<< dy <<","<< x <<","<< y <<","<< charge << std::endl;
    std::cout << "\tmat   : " << mat << std::endl;
    LocalTrajectoryError lterr(asSMatrix<5>(mat));
    const TrajectoryStateOnSurface tsos(ltpar, lterr, seed.det()->surface(), magField);
    std::vector<TrajectoryMeasurement> tmp = layerMeasurements->measurements(*layerSearch, tsos, *propagator, *estimator);
    std::cout << "\tvh compatibles: " << tmp.size() << std::endl;
    LayerMeasurements::SimpleHitContainer hits;
    layerMeasurements->recHits(hits, *layerSearch, tsos, *propagator, *estimator);
    std::cout << "\tvhits compatibles: " << hits.size() << std::endl;
    //std::vector<TrajectoryMeasurementGroup> tmpG = layerMeasurements->groupedMeasurements(*layerSearch, tsos, *propagator, *estimator);
    //std::cout << "\tvh grouped compatibles: " << tmpG.size() << std::endl;
    for(auto tm : tmp){
      auto hitPossible = tm.recHit();
      std::cout << "\t VH >> " << hitPossible << std::endl;
    }
    std::cout << "-----" << std::endl;


  }
  std::cout << "-----------------------------" << std::endl;

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

std::vector<VectorHit> SeedingOTEDProducer::collectVHsInput( edm::Handle< VectorHitCollectionNew > VHs ){

  const VectorHitCollectionNew& input = *VHs;
  //seeds are built in the L3 of the OT
  std::vector<VectorHit> VHs_layer3;
  if (input.size() > 0 ) {  
    std::cout << "initial VH collection size = " << input.size() << std::endl;
    for (auto DSViter : input) {
      if(checkLayer(DSViter.id()) == 3){
        for(auto vh : DSViter){
          std::cout << " VH in layer 3 >> " << vh << std::endl;
          VHs_layer3.push_back(vh);
        }
      }

      //just debugging for layer 2
      if(checkLayer(DSViter.id()) == 2){
        for(auto vh : DSViter)
          std::cout << " VH in layer 2 >> " << vh << std::endl;
      }
    }
  }

  std::cout << "VH in layer 3 collection size = " << VHs_layer3.size() << std::endl;

  return VHs_layer3;
}

AlgebraicSymMatrix SeedingOTEDProducer::assign44To55(AlgebraicSymMatrix mat44){
  if(mat44.num_row() != 4 || mat44.num_col() != 4)
    assert("Wrong dimension! This should be a 4x4 matrix!");

  AlgebraicSymMatrix result(5,0);
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      result[i][j] = mat44[i][j];
    }
  }
  return result;
}
