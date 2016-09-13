#include "RecoTracker/MeasurementDet/interface/SeedingOTEDProducer.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "RecoTracker/Record/interface/CkfComponentsRecord.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/TrajectoryStateUpdator.h"

#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "TrackingTools/MeasurementDet/interface/TrajectoryMeasurementGroup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

SeedingOTEDProducer::SeedingOTEDProducer(edm::ParameterSet const& conf):
  theUpdator(nullptr),
  tkMeasEvent( consumes<MeasurementTrackerEvent>(conf.getParameter<edm::InputTag>("trackerEvent")) )
{
  vhProducer = consumes< VectorHitCollectionNew >(edm::InputTag(conf.getParameter<edm::InputTag>("src")));
  beamSpot = consumes< reco::BeamSpot >(conf.getParameter<edm::InputTag>("beamSpotLabel"));
  updatorName = conf.getParameter<std::string>("updator");
  produces< VectorHitCollectionNew >();
}
    
SeedingOTEDProducer::~SeedingOTEDProducer() {
}

void SeedingOTEDProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPhase2VectorHits","vectorHitsAccepted"));
  desc.add<edm::InputTag>("trackerEvent", edm::InputTag("MeasurementTrackerEvent"));
  desc.add<edm::InputTag>("beamSpotLabel", edm::InputTag("offlineBeamSpot"));
  desc.add<std::string>("updator", std::string("KFUpdator"));
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
  measurementTracker = measurementTrackerHandle.product();
  edm::Handle<MeasurementTrackerEvent> measurementTrackerEvent;
  event.getByToken(tkMeasEvent,measurementTrackerEvent);

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

  edm::ESHandle<TrajectoryStateUpdator> updatorHandle;
  es.get<TrackingComponentsRecord>().get(updatorName, updatorHandle);
  theUpdator = updatorHandle.product();

  edm::Handle<reco::BeamSpot> beamSpotH;
  event.getByToken(beamSpot, beamSpotH);
  if (beamSpotH.isValid()) {
    std::cout << "BeamSpot Position: " << *(beamSpotH.product());
  }

  // Get the vector hits
  edm::Handle< VectorHitCollectionNew > vhs;
  event.getByToken(vhProducer, vhs);
/*
  edm::ESHandle< ClusterParameterEstimator<Phase2TrackerCluster1D> > parameterestimator;
  es.get<TkStripCPERecord>().get(cpe, parameterestimator); 
  const Phase2StripCPEGeometric & cpeOT(*parameterestimator);
*/
  run(vhs);

  std::cout << "SeedingOT::produce() end" << std::endl;

}

void SeedingOTEDProducer::run( edm::Handle< VectorHitCollectionNew > VHs ){

  std::cout << "-----------------------------" << std::endl;
  printVHsOnLayer(VHs,3);
  printVHsOnLayer(VHs,2);
  printVHsOnLayer(VHs,1);
  std::cout << "-----------------------------" << std::endl;
  //seeds are built in the L3 of the OT
  const DetLayer* barrelOTLayer2 = measurementTracker->geometricSearchTracker()->tobLayers().at(1);
  std::vector<VectorHit> VHseeds = collectVHsOnLayer(VHs,3);
  std::cout << "-----------------------------" << std::endl;
  std::cout << "VH seeds = " << VHseeds.size() << std::endl;

  for(auto seed : VHseeds){

    //building a tsos out of a VectorHit
    std::cout << "\tbuilding a seed for the VH: " << seed << std::endl;
    const TrajectoryStateOnSurface initialTSOS = buildInitialTSOS(seed);
    std::cout << "initialTSOS    : " << initialTSOS << std::endl;

    //set the direction of the propagator
    Propagator* theTmpPropagator = &*propagator->clone();
    if (initialTSOS.globalPosition().y()<0 ) theTmpPropagator->setPropagationDirection(oppositeToMomentum);
    else                              theTmpPropagator->setPropagationDirection(alongMomentum);
    if(theTmpPropagator->propagationDirection() == alongMomentum)
      std::cout << "propagator along Momentum" << std::endl;
    if(theTmpPropagator->propagationDirection() == oppositeToMomentum)
      std::cout << "propagator opposite To Momentum" << std::endl;

    //find vHits in layer 2
    std::cout << "-----------------------------" << std::endl;
    std::vector<TrajectoryMeasurement> tmp = layerMeasurements->measurements(*barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    std::cout << "\tvh compatibles: " << tmp.size() << std::endl;

    //other options
    //LayerMeasurements::SimpleHitContainer hits;
    //layerMeasurements->recHits(hits, *barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    //std::cout << "\tvhits compatibles: " << hits.size() << std::endl;
    //auto && tmpG = layerMeasurements->groupedMeasurements(*barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    //std::cout << "\tvh grouped compatibles: " << tmpG.size() << std::endl;

    std::vector<TrajectoryMeasurement>::iterator tmpend = std::remove_if(tmp.begin(), tmp.end(), isInvalid());
    tmp.erase(tmpend, tmp.end());
    std::cout << "\tvh compatibles(without invalidHit): " << tmp.size() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    const DetLayer* barrelOTLayer1 = measurementTracker->geometricSearchTracker()->tobLayers().at(0);

    for(auto tm : tmp){
      const VectorHit* vhit = dynamic_cast<const VectorHit*>(tm.recHit().get());
      //if (vhit->isValid()) 
        std::cout << "\t VH valid >> " << (*vhit) << std::endl;
      //else
        //std::cout << "\t VH invalid. " << std::endl;

      TrajectoryStateOnSurface updatedTSOS = theUpdator->update(initialTSOS,*(tm.recHit().get()));
      std::cout << "updatedTSOS    : " << updatedTSOS << std::endl;

      std::vector<TrajectoryMeasurement> tmpL1 = layerMeasurements->measurements(*barrelOTLayer1, updatedTSOS, *theTmpPropagator, *estimator);
      std::cout << "\tvh compatibles: " << tmpL1.size() << std::endl;
      std::vector<TrajectoryMeasurement>::iterator tmpL1end = std::remove_if(tmpL1.begin(), tmpL1.end(), isInvalid());
      tmpL1.erase(tmpL1end, tmpL1.end());
      std::cout << "\tvh compatibles(without invalidHit): " << tmpL1.size() << std::endl;
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

std::vector<VectorHit> SeedingOTEDProducer::collectVHsOnLayer( edm::Handle< VectorHitCollectionNew > VHs, unsigned int layerNumber ){

  const VectorHitCollectionNew& input = *VHs;
  std::vector<VectorHit> VHsOnLayer;
  if (input.size() > 0 ) {  
    std::cout << "initial VH collection size = " << input.size() << std::endl;
    for (auto DSViter : input) {
      if(checkLayer(DSViter.id()) == layerNumber){
        for(auto vh : DSViter){
          VHsOnLayer.push_back(vh);
        }
      }
    }
  }

  std::cout << "VH in layer " << layerNumber << " collection size = " << VHsOnLayer.size() << std::endl;

  return VHsOnLayer;
}

void SeedingOTEDProducer::printVHsOnLayer( edm::Handle< VectorHitCollectionNew > VHs, unsigned int layerNumber ){
  const VectorHitCollectionNew& input = *VHs;
  if (input.size() > 0 ) {  
    for (auto DSViter : input) {
      for(auto vh : DSViter){
        if(checkLayer(DSViter.id()) == layerNumber)  std::cout << " VH in layer " << layerNumber << " >> " << vh << std::endl;
      }
    }
  } else {
    std::cout << " No VHs in layer " << layerNumber << "." << std::endl;
  }
}

const TrajectoryStateOnSurface SeedingOTEDProducer::buildInitialTSOS( VectorHit& vHit ){

  int charge = 1;
  LocalTrajectoryParameters ltpar(vHit.localPosition(), vHit.localDirection(), charge);
  //float pT = vHit.transverseMomentum(magField);
  float p = vHit.momentum(magField);
  float x = vHit.localPosition().x();
  float y = vHit.localPosition().y();
  float dx = vHit.localDirection().x();
  float dy = vHit.localDirection().y(); 

  // having fun with theta
  //std::cout << "\tvh theta : " << vHit.theta() << std::endl;
  //std::cout << "\tvh eta : " << vHit.globalDirection().eta() << std::endl;
  Global3DVector gv(vHit.globalPosition().x(), vHit.globalPosition().y(), vHit.globalPosition().z());
  //std::cout << "\tgv : " << gv << std::endl;
  //std::cout << "\tgv theta : " << gv.theta() << std::endl;
  //std::cout << "\tgv eta : " << gv.eta() << std::endl;
  // gv transform to local (lv)
  const Local3DVector lv( vHit.det()->surface().toLocal( gv ) );
  //std::cout << "\tlv : " << lv << std::endl;
  // replace dy with second component of the lv renormalized to the z component
  dy = lv.y()/lv.z(); 

  LocalTrajectoryParameters ltpar2(1./p, dx, dy, x, y, charge);
  AlgebraicSymMatrix mat = assign44To55(vHit.parametersError());
  //FIXME::set the error on 1/p
  mat[0][0] = 1000;
  //std::cout << "\tltraj : " << 1./p <<","<< dx <<","<< dy <<","<< x <<","<< y <<","<< charge << std::endl;
  //std::cout << "\tmat   : " << mat << std::endl;

  //building tsos
  LocalTrajectoryError lterr(asSMatrix<5>(mat));
  const TrajectoryStateOnSurface tsos(ltpar2, lterr, vHit.det()->surface(), magField);
  //std::cout << "tsos    : " << tsos << std::endl;

  return tsos;
}

AlgebraicSymMatrix SeedingOTEDProducer::assign44To55( AlgebraicSymMatrix mat44 ){
  if(mat44.num_row() != 4 || mat44.num_col() != 4)
    assert("Wrong dimension! This should be a 4x4 matrix!");

  AlgebraicSymMatrix result(5,0);
  for(int i = 1; i < 5; i++){
    for(int j = 1; j < 5; j++){
      result[i][j] = mat44[i-1][j-1];
    }
  }
  return result;
}
