//---------------------------------------------------------------------------
// class SeedingOTEDProducer
// author: ebrondol
// date: July, 2016
//---------------------------------------------------------------------------

#ifndef RecoTracker_MeasurementDet_SeedingOTEDProducer_h
#define RecoTracker_MeasurementDet_SeedingOTEDProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "TrackingTools/MeasurementDet/interface/LayerMeasurements.h"
#include "RecoLocalTracker/Phase2TrackerRecHits/interface/Phase2StripCPEGeometric.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"

class SeedingOTEDProducer : public edm::EDProducer
{

 public:

  explicit SeedingOTEDProducer(const edm::ParameterSet&);
  virtual ~SeedingOTEDProducer();
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  static void fillDescriptions(edm::ConfigurationDescriptions&);

  void run(edm::Handle< VectorHitCollectionNew > VHs, 
           const Phase2StripCPEGeometric & cpe,
           const DetLayer* layerSearch);
  unsigned int checkLayer( unsigned int iidd );

 private:

  edm::EDGetTokenT< VectorHitCollectionNew > vhProducer;
  const TrackerTopology* tkTopo;
  const LayerMeasurements* layerMeasurements;
  const MeasurementEstimator* estimator;
  const Propagator* propagator;
  const MagneticField* magField;
  const edm::EDGetTokenT<MeasurementTrackerEvent> tkMeasEvent;
  std::string cpe;
};

#endif
