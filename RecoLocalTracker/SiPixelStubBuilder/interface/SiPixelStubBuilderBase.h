#ifndef RecoLocalTracker_SiPixelClusterizer_SiPixelStubBuilderBase_H
#define RecoLocalTracker_SiPixelClusterizer_SiPixelStubBuilderBase_H

#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/SiPixelStub/interface/SiPixelStub.h"
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

class SiPixelStubBuilderBase {

 public:
  typedef edmNew::DetSetVector<SiPixelStub> output_t;
  typedef std::pair< StackGeomDet, std::vector<Phase2TrackerCluster1D> > StackClusters;

  SiPixelStubBuilderBase(const edm::ParameterSet&);
  virtual ~SiPixelStubBuilderBase() {}
  void initialize(const edm::EventSetup&);

  //FIXME::ERICA::this should be template, return different collection for different algo used!!
  virtual VectorHitCollectionNew run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters) = 0;

  // Build stubs in a DetUnit
  virtual void buildDetUnit( const edm::DetSetVector<Phase2TrackerCluster1D> & input,
			     output_t& output) = 0;

  unsigned int getLayerNumber(const DetId& detid);
  unsigned int getModuleNumber(const DetId& detid);
  void printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters);

  const TrackerGeometry* theTkGeom;
  const TrackerTopology* theTkTopo;
  edm::ESHandle< StripClusterParameterEstimator > parameterestimator;

private:
  edm::ESInputTag cpeTag;

//  typedef SiStripRecHit2DCollection::FastFiller Collector;

};

#endif
