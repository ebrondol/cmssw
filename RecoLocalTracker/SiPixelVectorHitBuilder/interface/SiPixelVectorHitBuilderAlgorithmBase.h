#ifndef RecoLocalTracker_SiPixelClusterizer_SiPixelVectorHitBuilderAlgorithmBase_H
#define RecoLocalTracker_SiPixelClusterizer_SiPixelVectorHitBuilderAlgorithmBase_H

#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/StripClusterParameterEstimator.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

namespace edm {
   class ParameterSet;
   template<typename T> class RefGetter;
   class EventSetup;
}

class SiPixelVectorHitBuilderAlgorithmBase {

 public:
  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;
  typedef edmNew::DetSetVector<VectorHit> output_t;
  typedef std::pair< StackGeomDet, std::vector<Phase2TrackerCluster1D> > StackClusters;

  SiPixelVectorHitBuilderAlgorithmBase(const edm::ParameterSet&);
  virtual ~SiPixelVectorHitBuilderAlgorithmBase() {}
  void initialize(const edm::EventSetup&);

  //FIXME::ERICA::this should be template, return different collection for different algo used!!
  virtual void run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> >  clusters, VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej, 
    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej) = 0;

  virtual std::vector<VectorHit> buildVectorHits (const StackGeomDet * stack,
                                         edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
                                         const detset & DSVinner, const detset & DSVouter,
                                         const PixelGeomDetUnit * theLowerGeomDetUnit,
                                         const PixelGeomDetUnit * theUpperGeomDetUnit) = 0;

  void printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters);
  void printCluster(unsigned int rawId, const Phase2TrackerCluster1D* cluster);
  void loadDetSetVector( std::map< DetId,std::vector<VectorHit> >& theMap, edmNew::DetSetVector<VectorHit>& theCollection ) const ;

  const TrackerGeometry* theTkGeom;
  const TrackerTopology* theTkTopo;
  edm::ESHandle< StripClusterParameterEstimator > parameterestimator;
  unsigned int nMaxVHforeachStack;

private:
  edm::ESInputTag cpeTag;


//  typedef SiStripRecHit2DCollection::FastFiller Collector;

};

#endif
