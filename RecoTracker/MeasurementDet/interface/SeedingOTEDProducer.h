//---------------------------------------------------------------------------
// class SeedingOTEDProducer
// author: ebrondol
// date: July, 2016
//---------------------------------------------------------------------------

#ifndef RecoTracker_MeasurementDet_SeedingOTEDProducer_h
#define RecoTracker_MeasurementDet_SeedingOTEDProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "TrackingTools/MeasurementDet/interface/LayerMeasurements.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"

class TrajectoryStateUpdator;

class SeedingOTEDProducer : public edm::EDProducer
{

 public:

  explicit SeedingOTEDProducer(const edm::ParameterSet&);
  virtual ~SeedingOTEDProducer();
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  static void fillDescriptions(edm::ConfigurationDescriptions&);

  void run( edm::Handle< VectorHitCollectionNew > );
  unsigned int checkLayer( unsigned int iidd );
  std::vector<VectorHit> collectVHsOnLayer( edm::Handle< VectorHitCollectionNew >, unsigned int );
  void printVHsOnLayer( edm::Handle< VectorHitCollectionNew >, unsigned int );
  const TrajectoryStateOnSurface buildInitialTSOS( VectorHit& );
  AlgebraicSymMatrix assign44To55( AlgebraicSymMatrix );
  std::pair<bool, TrajectoryStateOnSurface> propagateAndUpdate(const TrajectoryStateOnSurface initialTSOS, const Propagator&, const TrackingRecHit& hit);
  float computeGlobalThetaError(const VectorHit& vh, const double sigmaZ_beamSpot);
  float computeInverseMomentumError(VectorHit& vh, const float globalTheta, const MagneticField* magField, const double sigmaZ_beamSpot);

  struct isInvalid {
    bool operator()(const TrajectoryMeasurement& measurement) {
      return ( ((measurement).recHit() == 0) || !((measurement).recHit()->isValid()) || !((measurement).updatedState().isValid()) ); 
    }
  };

 private:

  edm::EDGetTokenT< VectorHitCollectionNew > vhProducerToken;
  const TrackerTopology* tkTopo;
  const MeasurementTracker* measurementTracker;
  const LayerMeasurements* layerMeasurements;
  const MeasurementEstimator* estimator;
  const Propagator* propagator;
  const MagneticField* magField;
  const TrajectoryStateUpdator* theUpdator;
  const edm::EDGetTokenT<MeasurementTrackerEvent> tkMeasEventToken;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken;
  const reco::BeamSpot* beamSpot;
  std::string updatorName;
};

#endif
