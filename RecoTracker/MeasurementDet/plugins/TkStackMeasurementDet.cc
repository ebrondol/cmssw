#include "TkStackMeasurementDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  theCPE(cpe),
  theInnerDet(nullptr), theOuterDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* innerDet,
                                 const MeasurementDet* outerDet) {
  theInnerDet = dynamic_cast<const TkPixelMeasurementDet *>(innerDet);
  theOuterDet = dynamic_cast<const TkPixelMeasurementDet *>(outerDet);

  if ((theInnerDet == 0) || (theOuterDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPixelMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts) const
{

  RecHitContainer result;
/*
  HitCollectorForRecHits collector( &fastGeomDet(), theMatcher, theCPE, result );
  collectRecHits(ts, collector);
*/
  return result;
}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est,
                                          TempMeasurements & result) const {
  return true;
}
