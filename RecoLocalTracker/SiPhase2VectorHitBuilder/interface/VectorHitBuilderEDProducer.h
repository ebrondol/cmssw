//---------------------------------------------------------------------------
// class VectorHitBuilderEDProducer
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiPhase2VectorHitBuilder_VectorHitBuilderEDProducer_h
#define RecoLocalTracker_SiPhase2VectorHitBuilder_VectorHitBuilderEDProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithmBase.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"

class VectorHitBuilderEDProducer : public edm::EDProducer
{

 public:

  explicit VectorHitBuilderEDProducer(const edm::ParameterSet&);
  virtual ~VectorHitBuilderEDProducer();
  virtual void produce(edm::Event&, const edm::EventSetup&);
  void setupAlgorithm(edm::ParameterSet const& conf);
  void run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
           edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej,
           VectorHitCollectionNew& outputAcc, VectorHitCollectionNew& outputRej);
 VectorHitBuilderAlgorithmBase * algo() const { return stubsBuilder; };

 private:

  VectorHitBuilderAlgorithmBase * stubsBuilder;
  std::string offlinestubsTag;
  unsigned int maxOfflinestubs;
  std::string algoTag;
  edm::InputTag clusterProducer;
  bool readytobuild;

};

#endif
