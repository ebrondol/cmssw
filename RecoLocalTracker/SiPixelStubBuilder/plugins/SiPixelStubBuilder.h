#ifndef RecoLocalTracker_SiPixelStubBuilder_SiPixelStubBuilder_h
#define RecoLocalTracker_SiPixelStubBuilder_SiPixelStubBuilder_h

//---------------------------------------------------------------------------
//! \class SiPixelStubBuilder
//!
//! \brief EDProducer to create SiPixelStubs from SiPixelClusters.
//!//!
//! \author Natalie Heracleous adapted from SiPixelClusterizer
//! \version February 2nd, 2015
//!
//---------------------------------------------------------------------------

#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderBase.h"

//#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
//#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/SiPixelStub/interface/SiPixelStub.h"

#include "RecoLocalTracker/ClusterParameterEstimator/interface/StripClusterParameterEstimator.h"

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"



namespace cms
{

  class SiPixelStubBuilder : public edm::EDProducer {
  public:
    //--- Constructor, virtual destructor (just in case)
    explicit SiPixelStubBuilder(const edm::ParameterSet& );
    virtual ~SiPixelStubBuilder();

    void setupStubFindingAlgorithm(const TrackerGeometry& geom, const TrackerTopology& topo);

    // Begin Job
    //virtual void beginJob( const edm::EventSetup& );
    virtual void beginJob();

    //--- The top-level event method.
    virtual void produce(edm::Event&, const edm::EventSetup& );

    //--- Check input clusters
    void check(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters,
               const TrackerGeometry& geom,
               const TrackerTopology& topo);

    //--- Execute the algorithm(s).
    void run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& ,
 	     const TrackerGeometry& ,
             const TrackerTopology& ,
             edmNew::DetSetVector<Phase2TrackerCluster1D> );

    //--- Get the layer && module given the geometry
    //unsigned int getLayerNumber(const DetId& ,
    //		 	        const TrackerTopology* );
    //unsigned int getModuleNumber(const DetId& ,
    //		 	         const TrackerTopology* );

  private:

    edm::ParameterSet Conf_;
    edm::ESInputTag CPEtag_;
    edm::InputTag ClustersInputTag_;
    std::string StubBuilderAlgo_;
    SiPixelStubBuilderBase * StubBuilder_;
    bool ReadyToBuild_;
    edm::ESHandle< StripClusterParameterEstimator > parameterestimator;

    //typedef Phase2TrackerCluster1D::FastFiller Collector;

    //! Optional limit on the total number of clusters
    //int32_t maxTotalStubs_;
  };
}


#endif
