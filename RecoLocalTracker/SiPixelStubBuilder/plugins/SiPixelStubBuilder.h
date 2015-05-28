//---------------------------------------------------------------------------
// class SiPixelStubBuilder
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiPixelStubBuilder_SiPixelStubBuilder_h
#define RecoLocalTracker_SiPixelStubBuilder_SiPixelStubBuilder_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderBase.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"

class SiPixelStubBuilder : public edm::EDProducer
{

 public:

  explicit SiPixelStubBuilder(const edm::ParameterSet&);
  virtual ~SiPixelStubBuilder();
  virtual void produce(edm::Event&, const edm::EventSetup&);
  void setupAlgorithm(edm::ParameterSet const& conf);
  void run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& , VectorHitCollectionNew );


 private:

  SiPixelStubBuilderBase * stubsBuilder;
  std::string offlinestubsTag;
  unsigned int maxOfflinestubs;
  std::string algoTag;
  edm::InputTag clusterProducer;
  bool readytobuild;

};

#endif
