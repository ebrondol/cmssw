#include "TrackingTools/KalmanUpdators/interface/MRHKFUpdator.h"

#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//#include "TrackingTools/PatternTools/interface/MeasurementExtractor.h"
//#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
//#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/TrackingRecHit/interface/KfComponentsHolder.h"
#include "DataFormats/Math/interface/invertPosDefMatrix.h"
#include "DataFormats/Math/interface/ProjectMatrix.h"

/*  OLD  MRH KF Updator
#include "TrackingTools/KalmanUpdators/interface/MRHKFUpdator.h"

#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Math/interface/invertPosDefMatrix.h"

TrajectoryStateOnSurface MRHKFUpdator::update(const TrajectoryStateOnSurface& tsos,
  				              const TrackingRecHit& aRecHit) const {

  LogTrace("MRHKFUpdator") << "Calling MRHKFUpdator ... ";

  typedef typename AlgebraicROOTObject<5>::Vector Vec5;
  typedef typename AlgebraicROOTObject<5,5>::SymMatrix SMat55;

  SiTrackerMultiRecHit const & mHit = dynamic_cast<SiTrackerMultiRecHit const &>(aRecHit);
  std::vector<const TrackingRecHit*> components = mHit.recHits();
  std::vector<TrajectoryStateOnSurface> updateTSOSs;
  updateTSOSs.reserve(components.size());
  LogTrace("MRHKFUpdator") << " This MRH has " << components.size() << " components";

  double weightTsos = 1.0 - mHit.getWeightsSum();
  Vec5 x_tsos = tsos.localParameters().vector();
  SMat55 C_tsos = tsos.localError().matrix();
  LogTrace("MRHKFUpdator") << "Hit with weights sum " << mHit.getWeightsSum() ;
  LogTrace("MRHKFUpdator") << "Tsos with weight" << weightTsos;
  LogTrace("MRHKFUpdator") << "Predicted TSOS local par: " << x_tsos;
  LogTrace("MRHKFUpdator") << "Predicted TSOS local err: " << C_tsos;

  Vec5 x_tot = weightTsos*x_tsos;
  SMat55 C_tot;
  C_tot = weightTsos*weightTsos*C_tsos;

  LogTrace("MRHKFUpdator") << "####### Updated TSOS local par: " << x_tot;
  LogTrace("MRHKFUpdator") << "####### Updated TSOS local err: " << C_tot;

  bool ok = invertPosDefMatrix(C_tsos);

  for(std::vector<const TrackingRecHit*>::const_iterator iter = components.begin(); iter != components.end(); iter++){

    TrajectoryStateOnSurface updateComponent;
    updateComponent = theUpdator->update(tsos,**iter);
    updateTSOSs.push_back(updateComponent);
    LogTrace("MRHKFUpdator") << "New component of the MRH::update predicted state";
    LogTrace("MRHKFUpdator") << " local par: " << updateComponent.localParameters().vector();
    LogTrace("MRHKFUpdator") << " local err: " << updateComponent.localError().matrix();

  }

  if( updateTSOSs.size() != components.size() )
    edm::LogError("MRHKFUpdator")<<"Different sizes::something wrong happened!!";

  for( unsigned int iComp = 0; iComp < components.size(); iComp++){
    Vec5 xi = updateTSOSs.at(iComp).localParameters().vector();
    SMat55 Ci = updateTSOSs.at(iComp).localError().matrix();

    x_tot += mHit.weight(iComp)*xi;
    C_tot += mHit.weight(iComp)*mHit.weight(iComp)*Ci + 2*mHit.weight(iComp)*weightTsos*Ci;

    for( unsigned int jComp = iComp + 1 ; jComp < components.size(); jComp++){
      SMat55 Cj = updateTSOSs.at(jComp).localError().matrix();
      SMat55 Ctemp;
      ROOT::Math::AssignSym::Evaluate(Ctemp,Ci*C_tsos*Cj);
      C_tot += 2*mHit.weight(iComp)*mHit.weight(jComp)*Ctemp;
    }

    LogTrace("MRHKFUpdator") << "####### Updated TSOS local par: " << x_tot;
    LogTrace("MRHKFUpdator") << "####### Updated TSOS local err: " << C_tot;

  }

  LogTrace("MRHKFUpdator") << "####### Final updated TSOS local par: " << x_tot;
  LogTrace("MRHKFUpdator") << "####### Final updated TSOS local err: " << C_tot;

  if( ok )
    return TrajectoryStateOnSurface(LocalTrajectoryParameters(x_tot, tsos.localParameters().pzSign()),
                                    LocalTrajectoryError(C_tot), tsos.surface(),
                                    &(tsos.globalParameters().magneticField()), tsos.surfaceSide() );
  else{
    edm::LogError("MRHKFUpdator")<<" could not invert the pred TSOS cov martix:\n"<< C_tsos;
    return TrajectoryStateOnSurface();
  }  
}
*/

//with this method the KFUpdator is not needed anymore!!
TrajectoryStateOnSurface MRHKFUpdator::update(const TrajectoryStateOnSurface& tsos,
                                              const TrackingRecHit& aRecHit) const {

  LogTrace("MRHKFUpdator") << "Calling MRHKFUpdator ... ";

  SiTrackerMultiRecHit const & mHit = dynamic_cast<SiTrackerMultiRecHit const &>(aRecHit);
  std::vector<const TrackingRecHit*> components = mHit.recHits();
  int iComp = 0;
  TrajectoryStateOnSurface updateTsos;
  TrajectoryStateOnSurface predTsosTemp = tsos;
  LogTrace("MRHKFUpdator") << " This MRH has " << components.size() << " components";

  for(std::vector<const TrackingRecHit*>::const_iterator iter = components.begin(); iter != components.end(); iter++, iComp++){

    updateTsos = updateMRH(predTsosTemp,**iter,mHit.weight(iComp));
    LogTrace("MRHKFUpdator") << "#" << iComp << " component of the MRH::updated state";
    LogTrace("MRHKFUpdator") << " local par: " << updateTsos.localParameters().vector();
    LogTrace("MRHKFUpdator") << " local err: " << updateTsos.localError().matrix();
    predTsosTemp = updateTsos;

  }

  return updateTsos;

}


TrajectoryStateOnSurface MRHKFUpdator::updateMRH(const TrajectoryStateOnSurface& tsos,
                                                       const TrackingRecHit& aRecHit, float weight) const {
    switch (aRecHit.dimension()) {
        case 1: return updateMRH<1>(tsos,aRecHit,weight);
        case 2: return updateMRH<2>(tsos,aRecHit,weight);
        case 3: return updateMRH<3>(tsos,aRecHit,weight);
        case 4: return updateMRH<4>(tsos,aRecHit,weight);
        case 5: return updateMRH<5>(tsos,aRecHit,weight);
    }
    throw cms::Exception("Rec hit of invalid dimension (not 1,2,3,4,5)") <<
         "The value was " << aRecHit.dimension() << 
        ", type is " << typeid(aRecHit).name() << "\n";
}


template <unsigned int D>
TrajectoryStateOnSurface MRHKFUpdator::updateMRH(const TrajectoryStateOnSurface& tsos,
				           const TrackingRecHit& aRecHit, float weight) const {

  typedef typename AlgebraicROOTObject<D,5>::Matrix MatD5;
  typedef typename AlgebraicROOTObject<5,D>::Matrix Mat5D;
  typedef typename AlgebraicROOTObject<D,D>::SymMatrix SMatDD;
  typedef typename AlgebraicROOTObject<D>::Vector VecD;
  double pzSign = tsos.localParameters().pzSign();

  //MeasurementExtractor me(tsos);

  auto && x = tsos.localParameters().vector();
  auto && C = tsos.localError().matrix();
  // Measurement matrix
  ProjectMatrix<double,5,D>  pf;
  MatD5 H; 
  VecD r, rMeas; 
  SMatDD V, VMeas;

  KfComponentsHolder holder; 
  holder.template setup<D>(&r, &V, &H, &pf, &rMeas, &VMeas, x, C);
  aRecHit.getKfComponents(holder);
  
  r -= rMeas;
  SMatDD R = V + VMeas;

  LogTrace("MRHKFUpdator") << "\tpos tsos: " << x;
  LogTrace("MRHKFUpdator") << "\tcov tsos: " << C;
  LogTrace("MRHKFUpdator") << "\tV: " << V;
  LogTrace("MRHKFUpdator") << "\tVMeas: " << VMeas;
  LogTrace("MRHKFUpdator") << "\tR: " << R;
  bool ok = invertPosDefMatrix(R);
  LogTrace("MRHKFUpdator") << "\tinverted R: " << R;

  // Compute MRH Kalman gain matrix ( K * weight )
  Mat5D K;
  AlgebraicMatrix55 M = AlgebraicMatrixID();
  K = (C * ROOT::Math::Transpose(H)) * R;
  LogTrace("MRHKFUpdator") << "\tK: " << K;
  K *= weight;
  M -= K * H;

  LogTrace("MRHKFUpdator") << "\tK*weight: " << K;

  // Compute local filtered state vector
  AlgebraicVector5 fsv = x + K * r;
  // Compute covariance matrix of local filtered state vector
  AlgebraicSymMatrix55 fse = ROOT::Math::Similarity(M, C) + ROOT::Math::Similarity(K, V);

  if (ok) {
    return TrajectoryStateOnSurface( LocalTrajectoryParameters(fsv, pzSign),
				     LocalTrajectoryError(fse), tsos.surface(),&(tsos.globalParameters().magneticField()), tsos.surfaceSide() );
  }else {
    edm::LogError("MRHKFUpdator")<<" could not invert martix:\n"<< (V+VMeas);
    return TrajectoryStateOnSurface();
  }

}

