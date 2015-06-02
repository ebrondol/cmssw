//---------------------------------------------------------------------------
// class VectorHitBuilderAlgorithm
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiStubBuilder_VectorHitBuilderAlgorithm_H
#define RecoLocalTracker_SiStubBuilder_VectorHitBuilderAlgorithm_H

#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderAlgorithmBase.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"
#include "CommonTools/Statistics/interface/LinearFit.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class VectorHitBuilderAlgorithm : public SiPixelStubBuilderAlgorithmBase {
 public:

  VectorHitBuilderAlgorithm(const edm::ParameterSet& conf) :
    SiPixelStubBuilderAlgorithmBase(conf), theFitter(new LinearFit()) {};
  ~VectorHitBuilderAlgorithm() { delete theFitter; };

  void run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters, VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej,
    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej );
  bool checkModuleCompatibility(DetId detId1, DetId detId2);
  //not implemented yet
  bool checkClustersCompatibility(Local3DPoint& posinner, Local3DPoint& posouter, LocalError& errinner, LocalError& errouter);
  StackGeomDet createNewStack(DetId detId1, DetId detId2);
  std::vector<VectorHit> buildVectorHits(StackGeomDet stack,
                                         std::vector<Phase2TrackerCluster1D> innerClus,
                                         std::vector<Phase2TrackerCluster1D> outerClus);
  VectorHit buildVectorHit(StackGeomDet stack, const Phase2TrackerCluster1D& inner, const Phase2TrackerCluster1D& outer);

  // Full I/O in DetSet
  //void buildDetUnit( const edm::DetSetVector<Phase2TrackerCluster1D> & input,
  //                   output_t& output);

  void fit2Dzx(const Local3DPoint lpCI, const Local3DPoint lpCO,
               const LocalError leCI, const LocalError leCO,
               Local3DPoint& pos, Local3DVector& dir,
               AlgebraicSymMatrix22& covMatrix, double& chi2);
  void fit2Dzy(const Local3DPoint lpCI, const Local3DPoint lpCO,
               const LocalError leCI, const LocalError leCO,
               Local3DPoint& pos, Local3DVector& dir,
               AlgebraicSymMatrix22& covMatrix, double& chi2);

  void fit(const std::vector<float>& x,
           const std::vector<float>& y,
           const std::vector<float>& sigy,
           Local3DPoint& pos, Local3DVector& dir,
           AlgebraicSymMatrix22& covMatrix, double& chi2);

//  void build( const edm::DetSet<Phase2TrackerCluster1D> & input,
//                     output_t::FastFiller& output);


 private:
   LinearFit* theFitter;

};

#endif
