#include "TkPhase2MeasurementDet.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TSiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "TrackingTools/DetLayers/interface/MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/TrajMeasLessEstim.h"


namespace {
  const float theRocWidth  = 8.1;
  const float theRocHeight = 8.1;
}

TkPhase2MeasurementDet::TkPhase2MeasurementDet( const GeomDet* gdet,
					      const PixelClusterParameterEstimator* cpe) :
    MeasurementDet (gdet),
    theCPE(cpe),
    empty(true),
    activeThisEvent_(true), activeThisPeriod_(true)
  {
    if ( dynamic_cast<const PixelGeomDetUnit*>(gdet) == 0) {
      throw MeasurementDetException( "TkPhase2MeasurementDet constructed with a GeomDet which is not a PixelGeomDetUnit");
    }
  }

bool TkPhase2MeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
					  const MeasurementEstimator& est,
					  TempMeasurements & result) const {
  LogDebug("MeasurementTracker") << "measurements collected from TkPhase2MeasurementDet ... " << std::endl;

  if (!isActive()) {
    result.add(InvalidTransientRecHit::build(&geomDet(), TrackingRecHit::inactive), 0.F);
    return true;
  }

  auto oldSize = result.size();
  MeasurementDet::RecHitContainer && allHits = recHits(stateOnThisDet);
  for (auto && hit : allHits) {
    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    if ( diffEst.first)
      result.add(std::move(hit), diffEst.second);
  }

  if (result.size()>oldSize) return true;

  // create a TrajectoryMeasurement with an invalid RecHit and zero estimate
  bool inac = hasBadComponents(stateOnThisDet);
  TrackingRecHit::Type type = inac ? TrackingRecHit::inactive : TrackingRecHit::missing;
  result.add(InvalidTransientRecHit::build(&fastGeomDet(), type), 0.F);
  return inac;

}

/*
TransientTrackingRecHit::RecHitPointer
TkPhase2MeasurementDet::buildRecHit( const Phase2TrackerCluster1DRef & cluster,
				    const LocalTrajectoryParameters & ltp) const
{
  const GeomDetUnit& gdu( specificGeomDet());
  LocalValues lv = theCPE->localParameters( * cluster, gdu, ltp );
  return TSiPixelRecHit::build( lv.first, lv.second, &fastGeomDet(), cluster, theCPE);
}
*/
TkPhase2MeasurementDet::RecHitContainer
TkPhase2MeasurementDet::recHits( const TrajectoryStateOnSurface& ts ) const
{
  LogDebug("MeasurementTracker") << "rechits collected from TkPhase2MeasurementDet ... " << std::endl;
  RecHitContainer result;
/*
  if (empty == true ) return result;
  if (isActive() == false) return result;
  const Phase2TrackerCluster1D* begin=0;
  if(0!=handle_->data().size()) {
     begin = &(handle_->data().front());
  }
  result.reserve(detSet_.size());
  for ( const_iterator ci = detSet_.begin(); ci != detSet_.end(); ++ ci ) {

    if (ci < begin){
      edm::LogError("IndexMisMatch")<<"TkPhase2MeasurementDet cannot create hit because of index mismatch.";
      return result;
    }
     unsigned int index = ci-begin;
       Phase2TrackerCluster1DRef cluster = edmNew::makeRefTo( handle_, ci );
       result.push_back( buildRecHit( cluster, ts.localParameters() ) );
  }
*/
  return result;
}

bool
TkPhase2MeasurementDet::hasBadComponents( const TrajectoryStateOnSurface &tsos ) const {
/*
    if (badRocPositions_.empty()) return false;
    LocalPoint lp = tsos.localPosition();
    LocalError le = tsos.localError().positionError();
    double dx = 3*std::sqrt(le.xx()) + theRocWidth, dy = 3*std::sqrt(le.yy()) + theRocHeight;
    for (std::vector<LocalPoint>::const_iterator it = badRocPositions_.begin(), ed = badRocPositions_.end(); it != ed; ++it) {
        if ( (std::abs(it->x() - lp.x()) < dx) &&
             (std::abs(it->y() - lp.y()) < dy) ) return true;
    }
*/
    return false;
}
