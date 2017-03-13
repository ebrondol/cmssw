#include "RecoTracker/MeasurementDet/interface/SeedingOTEDProducer.h"
#include "FWCore/Framework/interface/Event.h"

#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "RecoTracker/Record/interface/CkfComponentsRecord.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/TrajectoryStateUpdator.h"

#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/MeasurementDet/interface/TrajectoryMeasurementGroup.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

SeedingOTEDProducer::SeedingOTEDProducer(edm::ParameterSet const& conf):
  theUpdator(nullptr),
  tkMeasEventToken( consumes<MeasurementTrackerEvent>(conf.getParameter<edm::InputTag>("trackerEvent")) )
{
  vhProducerToken = consumes< VectorHitCollectionNew >(edm::InputTag(conf.getParameter<edm::InputTag>("src")));
  beamSpotToken = consumes< reco::BeamSpot >(conf.getParameter<edm::InputTag>("beamSpotLabel"));
  updatorName = conf.getParameter<std::string>("updator");
  produces<TrajectorySeedCollection>();
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
  std::auto_ptr<TrajectorySeedCollection> seedsWithVHs(new TrajectorySeedCollection());

  edm::ESHandle<TrackerTopology> tTopoHandle;
  es.get<TrackerTopologyRcd>().get(tTopoHandle);
  tkTopo = tTopoHandle.product();

  edm::ESHandle<MeasurementTracker> measurementTrackerHandle;
  es.get<CkfComponentsRecord>().get(measurementTrackerHandle);
  measurementTracker = measurementTrackerHandle.product();
  edm::Handle<MeasurementTrackerEvent> measurementTrackerEvent;
  event.getByToken(tkMeasEventToken,measurementTrackerEvent);

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
  event.getByToken(beamSpotToken, beamSpotH);
  if (beamSpotH.isValid()) {
    std::cout << "BeamSpot Position: " << *(beamSpotH.product());
    beamSpot = beamSpotH.product();
  }

  // Get the vector hits
  edm::Handle< VectorHitCollectionNew > vhs;
  event.getByToken(vhProducerToken, vhs);
/*
  edm::ESHandle< ClusterParameterEstimator<Phase2TrackerCluster1D> > parameterestimator;
  es.get<TkStripCPERecord>().get(cpe, parameterestimator); 
  const Phase2StripCPEGeometric & cpeOT(*parameterestimator);
*/
  TrajectorySeedCollection const& tempSeeds = run(vhs);
  for( TrajectorySeedCollection::const_iterator qIt = tempSeeds.begin(); qIt < tempSeeds.end(); ++qIt ) {
    seedsWithVHs->push_back( *qIt );
  }

  seedsWithVHs->shrink_to_fit();
  event.put(seedsWithVHs);

  std::cout << "SeedingOT::produce() end" << std::endl;

}

TrajectorySeedCollection SeedingOTEDProducer::run( edm::Handle< VectorHitCollectionNew > VHs ){

  TrajectorySeedCollection result;

  std::cout << "-----------------------------" << std::endl;
  printVHsOnLayer(VHs,3);
  printVHsOnLayer(VHs,2);
  printVHsOnLayer(VHs,1);
  std::cout << "-----------------------------" << std::endl;
  //seeds are built in the L3 of the OT
  const BarrelDetLayer* barrelOTLayer2 = measurementTracker->geometricSearchTracker()->tobLayers().at(1);
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
    std::vector<TrajectoryMeasurement> measurementsL2 = layerMeasurements->measurements(*barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    std::cout << "\tvh compatibles: " << measurementsL2.size() << std::endl;

    //other options
    //LayerMeasurements::SimpleHitContainer hits;
    //layerMeasurements->recHits(hits, *barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    //std::cout << "\tvhits compatibles: " << hits.size() << std::endl;
    //auto && measurementsL2G = layerMeasurements->groupedMeasurements(*barrelOTLayer2, initialTSOS, *theTmpPropagator, *estimator);
    //std::cout << "\tvh grouped compatibles: " << measurementsL2G.size() << std::endl;

    std::vector<TrajectoryMeasurement>::iterator measurementsL2end = std::remove_if(measurementsL2.begin(), measurementsL2.end(), isInvalid());
    measurementsL2.erase(measurementsL2end, measurementsL2.end());
    std::cout << "\tvh compatibles(without invalidHit): " << measurementsL2.size() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    if(!measurementsL2.empty()){
      //not sure if building it everytime takes time/memory
      const DetLayer* barrelOTLayer1 = measurementTracker->geometricSearchTracker()->tobLayers().at(0);
  
      for(auto mL2 : measurementsL2){
 
        const TrackingRecHit* hit = mL2.recHit().get();
        const VectorHit* vhit = dynamic_cast<const VectorHit*>(hit);
        std::cout << "\t VH valid >> " << (*vhit) << std::endl;
  
        //propagate to the L2 and update the TSOS
        std::pair<bool, TrajectoryStateOnSurface> updatedTSOS = propagateAndUpdate(initialTSOS, *theTmpPropagator, *hit);
        if(!updatedTSOS.first) continue;
        std::cout << "updatedTSOS  : " << updatedTSOS.second << std::endl;
        std::cout << "chi2 VH/updatedTSOS  : " << estimator->estimate(updatedTSOS.second, *hit).second << std::endl;
  
        std::vector<TrajectoryMeasurement> measurementsL1 = layerMeasurements->measurements(*barrelOTLayer1, updatedTSOS.second, *theTmpPropagator, *estimator);
        std::cout << "\tvh compatibles on L1: " << measurementsL1.size() << std::endl;
        std::vector<TrajectoryMeasurement>::iterator measurementsL1end = std::remove_if(measurementsL1.begin(), measurementsL1.end(), isInvalid());
        measurementsL1.erase(measurementsL1end, measurementsL1.end());
        std::cout << "\tvh compatibles on L1(without invalidHit): " << measurementsL1.size() << std::endl;

        if(!measurementsL1.empty()){

          for(auto mL1 : measurementsL1){
            const TrackingRecHit* hitL1 = mL1.recHit().get();
            const VectorHit* vhitL1 = dynamic_cast<const VectorHit*>(hitL1);
            std::cout << "\t VH valid >> " << (*vhitL1) << std::endl;
            std::pair<bool, TrajectoryStateOnSurface> updatedTSOSL1 = propagateAndUpdate(updatedTSOS.second, *theTmpPropagator, *hitL1);
            std::cout << "updatedTSOS  on L1   : " << updatedTSOSL1.second << std::endl;
            std::cout << "chi2 VH/updatedTSOS  : " << estimator->estimate(updatedTSOSL1.second, *hitL1).second << std::endl;


            if( updatedTSOSL1.first ){
            // passSelection(updatedTSOS) :
            // http://cmslxr.fnal.gov/lxr/source/FastSimulation/Muons/plugins/FastTSGFromPropagation.cc?v=CMSSW_8_1_X_2016-09-04-2300#0474
              edm::OwnVector<TrackingRecHit> container;
              container.push_back(seed.clone());
              container.push_back(hit->clone());
              container.push_back(hitL1->clone());
              std::cout << "-------> hits found in this seed: " << container.size() << std::endl;
              TrajectorySeed ts = createSeed(updatedTSOS.second, container, hitL1->geographicalId());
              result.push_back(ts);
            }
          }

        }
  
        std::cout << "-----" << std::endl;
      }
    }

  }
  std::cout << "-----------------------------" << std::endl;
  std::cout << "------- seeds found: " << result.size() << " ------" << std::endl;
  std::cout << "-----------------------------" << std::endl;

  return result;
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

  //FIXME::charge is fine 1 every two times!!
  int charge = 1;
  //float pT = vHit.transverseMomentum(magField);
  float p = vHit.momentum(magField);
  float x = vHit.localPosition().x();
  float y = vHit.localPosition().y();
  float dx = vHit.localDirection().x();
  float dy = vHit.localDirection().y(); 

  // having fun with theta
  Global3DVector gv(vHit.globalPosition().x(), vHit.globalPosition().y(), vHit.globalPosition().z());
  float theta = gv.theta();
  //std::cout << "\tgv : " << gv << std::endl;
  std::cout << "\tgv theta : " << theta << std::endl;
  std::cout << "\tgv theta error : " << computeGlobalThetaError(vHit, beamSpot->sigmaZ()) << std::endl;
  //std::cout << "\tgv eta : " << gv.eta() << std::endl;
  // gv transform to local (lv)
  const Local3DVector lv( vHit.det()->surface().toLocal( gv ) );
  //std::cout << "\tlv : " << lv << std::endl;
  // replace dy with second component of the lv renormalized to the z component
  dy = lv.y()/lv.z(); 

  LocalTrajectoryParameters ltpar2(1./p, dx, dy, x, y, charge);
  AlgebraicSymMatrix mat = assign44To55(vHit.parametersError());
  //FIXME::set the error on 1/p
  mat[0][0] = pow(computeInverseMomentumError(vHit, theta,magField,beamSpot->sigmaZ()),2);
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

std::pair<bool, TrajectoryStateOnSurface> SeedingOTEDProducer::propagateAndUpdate(const TrajectoryStateOnSurface initialTSOS, const Propagator& prop, const TrackingRecHit& hit){
  TrajectoryStateOnSurface propTSOS = prop.propagate( initialTSOS, hit.det()->surface());
  TrajectoryStateOnSurface updatedTSOS = theUpdator->update(propTSOS,hit);
  if unlikely(!updatedTSOS.isValid()) return std::make_pair( false, updatedTSOS);
  return std::make_pair( true, updatedTSOS);
}

float SeedingOTEDProducer::computeGlobalThetaError(const VectorHit& vh, const double sigmaZ_beamSpot){

  double derivative = vh.globalPosition().perp()/(pow(vh.globalPosition().z(),2)+pow(vh.globalPosition().perp(),2));
  double derivative2 = pow(derivative,2);
  return pow( derivative2*vh.lowerGlobalPosErr().czz()+derivative2*pow(sigmaZ_beamSpot,2), 0.5);

}

float SeedingOTEDProducer::computeInverseMomentumError(VectorHit& vh, const float globalTheta, const MagneticField* magField, const double sigmaZ_beamSpot){

  //for pT > 2GeV, 1/pT has sigma = 1/sqrt(12)
  float varianceInverseTransvMomentum = 1./12;
  double derivativeTheta2 = pow(sin(globalTheta)/vh.transverseMomentum(magField),2);
  double derivativeInverseTransvMomentum2 = pow(cos(globalTheta),2);
  float thetaError = computeGlobalThetaError(vh, sigmaZ_beamSpot);
  return pow(derivativeTheta2*pow(thetaError,2)+derivativeInverseTransvMomentum2*varianceInverseTransvMomentum,0.5);

}

TrajectorySeed SeedingOTEDProducer::createSeed(const TrajectoryStateOnSurface& tsos, const edm::OwnVector<TrackingRecHit>& container, const DetId& id) const {

  PTrajectoryStateOnDet seedTSOS = trajectoryStateTransform::persistentState(tsos,id.rawId());
  return TrajectorySeed(seedTSOS,container,oppositeToMomentum);

}
