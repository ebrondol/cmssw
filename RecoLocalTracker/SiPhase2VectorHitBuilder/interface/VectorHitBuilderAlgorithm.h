//---------------------------------------------------------------------------
// class VectorHitBuilderAlgorithm
// author: ebrondol,nathera
// date: May, 2015
//---------------------------------------------------------------------------

#ifndef RecoLocalTracker_SiPhase2VectorHitBuilder_VectorHitBuilderAlgorithm_H
#define RecoLocalTracker_SiPhase2VectorHitBuilder_VectorHitBuilderAlgorithm_H

#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithmBase.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"
#include "CommonTools/Statistics/interface/LinearFit.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class VectorHitBuilderAlgorithm : public VectorHitBuilderAlgorithmBase {
 public:

  VectorHitBuilderAlgorithm(const edm::ParameterSet& conf) :
    VectorHitBuilderAlgorithmBase(conf), theFitter(new LinearFit()) {};
  ~VectorHitBuilderAlgorithm() { delete theFitter; };

  void run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
           VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej,
           edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej );

  //not implemented yet
  bool checkClustersCompatibility(Local3DPoint& posinner, Local3DPoint& posouter, LocalError& errinner, LocalError& errouter);

  std::vector<VectorHit> buildVectorHits(const StackGeomDet * stack,
                                         edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
                                         const detset & DSVinner, const detset & DSVouter,
                                         const GeomDetUnit * theLowerGeomDetUnit,
                                         const GeomDetUnit * theUpperGeomDetUnit);

  VectorHit buildVectorHit(const StackGeomDet* stack, Phase2TrackerCluster1DRef lower, Phase2TrackerCluster1DRef upper, 
                           const GeomDetUnit * theLowerGeomDetUnit, const GeomDetUnit * theUpperGeomDetUnit);

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
