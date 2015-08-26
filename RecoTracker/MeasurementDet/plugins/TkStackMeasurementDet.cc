#include "TkStackMeasurementDet.h"
#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const GluedGeomDet* gdet,
                                              const SiStripRecHitMatcher* matcher,
                                              const StripClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  theMatcher(matcher),  theCPE(cpe),
  theMonoDet(nullptr), theStereoDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* monoDet,
                                 const MeasurementDet* stereoDet) {
  theMonoDet = dynamic_cast<const TkStripMeasurementDet *>(monoDet);
  theStereoDet = dynamic_cast<const TkStripMeasurementDet *>(stereoDet);

  if ((theMonoDet == 0) || (theStereoDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkStripMeasurementDet");
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
