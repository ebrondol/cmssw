#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TVectorHit.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/SiPixelVectorHitBuilderAlgorithmBase.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const SiPixelVectorHitBuilder* matcher,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  theMatcher(matcher),
  thePixelCPE(cpe),
  theLowerDet(nullptr), theUpperDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theLowerDet = dynamic_cast<const TkPixelMeasurementDet *>(lowerDet);
  theUpperDet = dynamic_cast<const TkPixelMeasurementDet *>(upperDet);

  if ((theLowerDet == 0) || (theUpperDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPixelMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts) const
{
  std::cout << "\tTkStackMeasurementDet::rechits ok" << std::endl;
  RecHitContainer result;
  if (Empty == true ) return result;
  if (isActive() == false) return result;

  result.reserve(theLowerDetSet.size());

  SiPixelVectorHitBuilderAlgorithmBase * algobase = theMatcher->algo() ;
  VectorHitBuilderAlgorithm* vhalgo = dynamic_cast<VectorHitBuilderAlgorithm *>(algobase);
  std::vector<VectorHit> vhs;
  if(vhalgo){
    vhs = vhalgo->buildVectorHits(&specificGeomDet(), theHandle, theLowerDetSet, theUpperDetSet, specificGeomDet().lowerDet(), specificGeomDet().upperDet());
  }
  else
    std::cout << "algo not valid" << std::endl;

  for ( auto vh : vhs ){
    LogTrace("MeasurementTracker") << "TkStackMeasurementDet::rechits adding VectorHits!" << std::endl;
    result.push_back( TVectorHit::build( &fastGeomDet(), &vh, thePixelCPE) );
  }

  return result;

}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est,
                                          TempMeasurements & result) const {
  LogTrace("MeasurementTracker") << "TkStackMeasurementDet::measurements" << std::endl;

  //here there is the possibility to add if(isActive)

  auto oldSize = result.size();
  MeasurementDet::RecHitContainer && allHits = recHits(stateOnThisDet);

  for (auto && hit : allHits) {
    //std::cout << "\tTkStackMeasurementDet::newhit" << hit->globalPosition() << std::endl;

    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    if ( diffEst.first){
      LogTrace("MeasurementTracker") << "The measurement has been added with chi2 = " << diffEst.second << std::endl;
      result.add(std::move(hit), diffEst.second);
    }
  
    LogTrace("MeasurementTracker") << std::endl;

  }
  if (result.size()>oldSize) return true;

  //here there is the possibility to add hasBadComponents
  return true;
}

