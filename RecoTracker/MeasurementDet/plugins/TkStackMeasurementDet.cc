#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  thePixelCPE(cpe),
  theLowerDet(nullptr), theUpperDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theLowerDet = dynamic_cast<const TkPhase2MeasurementDet *>(lowerDet);
  theUpperDet = dynamic_cast<const TkPhase2MeasurementDet *>(upperDet);

  if ((theLowerDet == 0) || (theUpperDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPhase2MeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts) const
{

  RecHitContainer result;
  if (Empty == true ) return result;
  if (isActive() == false) return result;

  result.reserve(theLowerDetSet.size());

  return result;

}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est,
                                          TempMeasurements & result) const {

  LogDebug("MeasurementTracker") << "measurements collected from TkStackMeasurementDet ... " << std::endl;
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

  //here there is the possibility to add hasBadComponents
  return result.size()>oldSize;
}

