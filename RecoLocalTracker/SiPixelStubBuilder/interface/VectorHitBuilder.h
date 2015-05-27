#ifndef RecoLocalTracker_SiStubBuilder_VectorHitBuilder_H
#define RecoLocalTracker_SiStubBuilder_VectorHitBuilder_H

//-----------------------------------------------------------------------
//! \class VectorHitBuilder
//! \brief An dummy stub building algorithm
//!

//-----------------------------------------------------------------------

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderBase.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"

#include <vector>
#include <map>


class VectorHitBuilder : public SiPixelStubBuilderBase {
 public:

  VectorHitBuilder(edm::ParameterSet const& conf, const TrackerGeometry& geom, const TrackerTopology& topo);
  ~VectorHitBuilder();

  // group clusters in stack modules
  std::vector< StackClusters > groupinginStackModules(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters);
  std::vector<VectorHit> buildVectorHits(StackGeomDet stack,
                                         std::vector<Phase2TrackerCluster1D> innerClus,
                                         std::vector<Phase2TrackerCluster1D> outerClus);

  // Full I/O in DetSet
  void buildDetUnit( const edm::DetSetVector<Phase2TrackerCluster1D> & input,
                     output_t& output);

  void fit(const std::vector<float>& x,
           const std::vector<float>& y,
           const std::vector<float>& sigy,
           LocalPoint& pos,
           LocalVector& dir,
           AlgebraicSymMatrix& covMatrix,
           double& chi2);

//  void build( const edm::DetSet<Phase2TrackerCluster1D> & input,
//                     output_t::FastFiller& output);


 private:
  edm::ParameterSet conf_;
  const TrackerGeometry& theTkGeom;
  const TrackerTopology& theTkTopo;


 template<class T> void buildDetUnit_(const T& input, output_t& output) {
   for(typename T::const_iterator it = input.begin(); it!=input.end(); it++) {
     output_t::FastFiller ff(output, it->detId());
//     build(*it, ff);
     if(ff.empty()) ff.abort();
   }
 }

/*
  //! Data storage
  SiPixelArrayBuffer               theBuffer;         // internal nrow * ncol matrix
  bool                             bufferAlreadySet;  // status of the buffer array
  std::vector<SiPixelCluster::PixelPos>  theSeeds;          // cached seed pixels
  std::vector<SiPixelCluster>            theClusters;       // resulting clusters

  //! Clustering-related quantities:
  float thePixelThresholdInNoiseUnits;    // Pixel threshold in units of noise
  float theSeedThresholdInNoiseUnits;     // Pixel cluster seed in units of noise
  float theClusterThresholdInNoiseUnits;  // Cluster threshold in units of noise

  int   thePixelThreshold;  // Pixel threshold in electrons
  int   theSeedThreshold;   // Seed threshold in electrons
  float theClusterThreshold;  // Cluster threshold in electrons
  int   theConversionFactor;  // adc to electron conversion factor
  int   theOffset;            // adc to electron conversion offset

  //! Geometry-related information
  int  theNumOfRows;
  int  theNumOfCols;
  uint32_t detid_;
  bool dead_flag;
  bool doMissCalibrate; // Use calibration or not
  bool doSplitClusters;
  //! Private helper methods:
  bool setup(const PixelGeomDetUnit * pixDet);

  // Calibrate the ADC charge to electrons
  int   theStackADC_;          // The maximum ADC count for the stack layers
  int   theFirstStack_;        // The index of the first stack layer
*/

};

#endif
