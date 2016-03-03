//---------------------------------------------------------------------------
// class VectorHitBuilderAlgorithm
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiVectorHitBuilder_VectorHitBuilderAlgorithm_H
#define RecoLocalTracker_SiVectorHitBuilder_VectorHitBuilderAlgorithm_H

#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/SiPixelVectorHitBuilderAlgorithmBase.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit.h"
#include "CommonTools/Statistics/interface/LinearFit.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class VectorHitBuilderAlgorithm : public SiPixelVectorHitBuilderAlgorithmBase {
 public:
  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;

  VectorHitBuilderAlgorithm(const edm::ParameterSet& conf) :
    SiPixelVectorHitBuilderAlgorithmBase(conf), theFitter(new LinearFit()) {};
  ~VectorHitBuilderAlgorithm() { delete theFitter; };

  void run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
           VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej,
           edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej );

  //not implemented yet
  bool checkClustersCompatibility(Local3DPoint& posinner, Local3DPoint& posouter, LocalError& errinner, LocalError& errouter);

  std::vector<VectorHit> buildVectorHits(const StackGeomDet& stack,
                                         edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
                                         const detset & DSVinner, 
                                         const detset & DSVouter);

  VectorHit buildVectorHit(const StackGeomDet& stack, Phase2TrackerCluster1DRef inner, Phase2TrackerCluster1DRef outer);

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
