#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  thePixelCPE(cpe),
  theLowerDet(nullptr), theUpperDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theLowerDet = dynamic_cast<const TkPhase2OTMeasurementDet *>(lowerDet);
  theUpperDet = dynamic_cast<const TkPhase2OTMeasurementDet *>(upperDet);

  if ((theLowerDet == 0) || (theUpperDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPhase2OTMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts, const MeasurementTrackerEvent & data) const
{
  LogDebug("MeasurementTracker")<<"TkStackMeasurementDet::recHits";
  RecHitContainer result;
/*
  HitCollectorForRecHits collector( &fastGeomDet(), theMatcher, theCPE, result );
  collectRecHits(ts, collector);
*/
  return result;
}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est, const MeasurementTrackerEvent & data,
                                          TempMeasurements & result) const {
  LogDebug("MeasurementTracker")<<"TkStackMeasurementDet::measurements";
  return true;
}
