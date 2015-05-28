//---------------------------------------------------------------------------
// class VectorHitBuilder
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiStubBuilder_VectorHitBuilder_H
#define RecoLocalTracker_SiStubBuilder_VectorHitBuilder_H

#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderBase.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class VectorHitBuilder : public SiPixelStubBuilderBase {
 public:

  VectorHitBuilder(const edm::ParameterSet& conf) : SiPixelStubBuilderBase(conf) {};
  ~VectorHitBuilder() {};

  VectorHitCollectionNew run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters);
  bool checkModuleCompatibility(DetId detId1, DetId detId2);
  StackGeomDet createNewStack(DetId detId1, DetId detId2);

  std::vector< VectorHit > buildVectorHits(StackGeomDet stack,
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
  template<class T> void buildDetUnit_(const T& input, output_t& output) {
   for(typename T::const_iterator it = input.begin(); it!=input.end(); it++) {
     output_t::FastFiller ff(output, it->detId());
//     build(*it, ff);
     if(ff.empty()) ff.abort();
   }
 }

};

#endif
