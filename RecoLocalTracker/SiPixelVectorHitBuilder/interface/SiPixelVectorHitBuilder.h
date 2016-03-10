//---------------------------------------------------------------------------
// class SiPixelVectorHitBuilder
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiPixelVectorHitBuilder_SiPixelVectorHitBuilder_h
#define RecoLocalTracker_SiPixelVectorHitBuilder_SiPixelVectorHitBuilder_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/SiPixelVectorHitBuilderAlgorithmBase.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"

class SiPixelVectorHitBuilder : public edm::EDProducer
{

 public:

  explicit SiPixelVectorHitBuilder(const edm::ParameterSet&);
  virtual ~SiPixelVectorHitBuilder();
  virtual void produce(edm::Event&, const edm::EventSetup&);
  void setupAlgorithm(edm::ParameterSet const& conf);
  void run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
           edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej,
           VectorHitCollectionNew& outputAcc, VectorHitCollectionNew& outputRej);
 SiPixelVectorHitBuilderAlgorithmBase * algo() const { return stubsBuilder; };

 private:

  SiPixelVectorHitBuilderAlgorithmBase * stubsBuilder;
  std::string offlinestubsTag;
  unsigned int maxOfflinestubs;
  std::string algoTag;
  edm::InputTag clusterProducer;
  bool readytobuild;

};

#endif
