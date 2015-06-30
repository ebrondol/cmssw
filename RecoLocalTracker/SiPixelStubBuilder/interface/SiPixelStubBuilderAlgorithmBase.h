#ifndef RecoLocalTracker_SiPixelClusterizer_SiPixelStubBuilderAlgorithmBase_H
#define RecoLocalTracker_SiPixelClusterizer_SiPixelStubBuilderAlgorithmBase_H

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

namespace edm {
   class ParameterSet;
   template<typename T> class RefGetter;
   class EventSetup;
}

class SiPixelStubBuilderAlgorithmBase {

 public:
  typedef edmNew::DetSetVector<VectorHit> output_t;
  typedef std::pair< StackGeomDet, std::vector<Phase2TrackerCluster1D> > StackClusters;

  SiPixelStubBuilderAlgorithmBase(const edm::ParameterSet&);
  virtual ~SiPixelStubBuilderAlgorithmBase() {}
  void initialize(const edm::EventSetup&);

  //FIXME::ERICA::this should be template, return different collection for different algo used!!
  virtual void run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters, VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej, 
    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej) = 0;

  unsigned int getLayerNumber(const DetId& detid);
  unsigned int getSubLayerNumber(const DetId& detid);
  unsigned int getModuleNumber(const DetId& detid);
  void printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters);
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
