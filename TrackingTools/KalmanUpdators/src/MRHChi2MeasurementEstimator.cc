#include "RecoTracker/TransientTrackingRecHit/interface/TSiTrackerMultiRecHit.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/KalmanUpdators/interface/MRHChi2MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/MeasurementExtractor.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Math/interface/invertPosDefMatrix.h"

/*ERICA: old version of the code
std::pair<bool,double>
MRHChi2MeasurementEstimator::estimate(const TrajectoryStateOnSurface& tsos,
				   const TrackingRecHit& aRecHit) const {
  if (!aRecHit.isValid()) {
	throw cms::Exception("MRHChi2MeasurementEstimator") << "Invalid RecHit passed to the MRHChi2MeasurementEstimator ";
  }

  typedef AlgebraicROOTObject<2>::Vector Vec;
  typedef AlgebraicROOTObject<2>::SymMatrix Mat;

  //better be a multihit...
  TSiTrackerMultiRecHit const & mHit = dynamic_cast<TSiTrackerMultiRecHit const &>(aRecHit);  
  MeasurementExtractor me(tsos);
  double est=0;
  double annealing = mHit.getAnnealingFactor();
  LogDebug("MRHChi2MeasurementEstimator") << "Current annealing factor is " << annealing; 		
  TransientTrackingRecHit::ConstRecHitContainer components = mHit.transientHits();
  LogDebug("MRHChi2MeasurementEstimator") << "this hit has " << components.size() << " components";	
  for (TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter = components.begin(); iter != components.end(); iter++){		
  	Vec r = asSVector<2>((*iter)->parameters()) - me.measuredParameters<2>(**iter);
  	Mat R = asSMatrix<2>((*iter)->parametersError())*annealing + me.measuredError<2>(**iter);
  	//int ierr = ! R.Invert(); // if (ierr != 0) throw exception; // 
	invertPosDefMatrix(R);
	LogDebug("MRHChi2MeasurementEstimator") << "Hit with weight " << (*iter)->weight(); 
  	est += ROOT::Math::Similarity(r, R)*((*iter)->weight());
  }	
  return returnIt(est);
}
*/

std::pair<bool, double> MRHChi2MeasurementEstimator::estimate(const TrajectoryStateOnSurface& tsos,
                                   const TrackingRecHit& aRecHit) const {

//std::cout << "Calling MRHChi2MeasurementEstimator::estimate" << std::endl;
  switch (aRecHit.dimension()) {
    case 2:       return estimate<2>(tsos,aRecHit);
    //ERICA: avoid the not-2D  hit due to the final sum  
    case ( 1 || 3 || 4 || 5 ):{
      std::cout << "WARNING:The hit is not 2D: does not count in the MRH Chi2 estimation." <<  std::endl;
      double est = 0.0; 
      return  HitReturnType(false, est);
      }
    }
    throw cms::Exception("Rec hit of invalid dimension (not 1,2,3,4,5)") <<
      "The value was " << aRecHit.dimension() <<
      ", type is " << typeid(aRecHit).name() << "\n";
}

//---------------------------------------------------------------------------------------------------------------
template <unsigned int N>
std::pair<bool, double> MRHChi2MeasurementEstimator::estimate(const TrajectoryStateOnSurface& tsos,
                                                const TrackingRecHit& aRecHit) const {
  //ERICA: tsos parameters or not?? (Look MeasurementExtractor me(tsos) )

  TSiTrackerMultiRecHit const & mHit = dynamic_cast<TSiTrackerMultiRecHit const &>(aRecHit);  
  double est=0;

  double annealing = mHit.getAnnealingFactor();
//  std::cout << "Current annealing factor is " << annealing << std::endl;               
  LogDebug("MRHChi2MeasurementEstimator") << "Current annealing factor is " << annealing;               

  TransientTrackingRecHit::ConstRecHitContainer components = mHit.transientHits();
//  std::cout << " and this hit has " << components.size() << " components" << std::endl;     
  LogDebug("MRHChi2MeasurementEstimator") << "this hit has " << components.size() << " components";     

  for (TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter = components.begin(); iter != components.end(); iter++){              

    // define variables that will be used to setup the KfComponentsHolder
    ProjectMatrix<double,5,N>  pf;
    typename AlgebraicROOTObject<N,5>::Matrix H;
    typename AlgebraicROOTObject<N>::Vector r, rMeas;
    typename AlgebraicROOTObject<N,N>::SymMatrix V, VMeas;
    AlgebraicVector5 x = tsos.localParameters().vector();
    const AlgebraicSymMatrix55 &C = (tsos.localError().matrix());

    // setup the holder with the correct dimensions and get the values
    KfComponentsHolder holder;
    holder.template setup<N>(&r, &V, &H, &pf, &rMeas, &VMeas, x, C);
    (**iter).getKfComponents(holder);

    r -= rMeas;
    V = V*annealing + VMeas;
    bool ierr = invertPosDefMatrix(V);
    if( !ierr ) {
      std::cout <<"MRHChi2MeasurementEstimator::estimate: Vinv not valid!"<<std::endl;
      edm::LogError("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: W not valid!"<<std::endl;
    }

//    std::cout << "Hit with weight " << (*iter)->weight() << std::endl; 
    LogDebug("MRHChi2MeasurementEstimator") << "Hit with weight " << (*iter)->weight(); 
    est += ROOT::Math::Similarity(r, V)*((*iter)->weight());
  }     

  return returnIt(est);

}

