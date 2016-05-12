#include "TkPhase2MeasurementDet.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
//#include "RecoTracker/TransientTrackingRecHit/interface/TSiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/Phase2TrackerRecHit1D.h"
#include "TrackingTools/DetLayers/interface/MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/TrajMeasLessEstim.h"


namespace {
  const float theRocWidth  = 8.1;
  const float theRocHeight = 8.1;
}

TkPhase2MeasurementDet::TkPhase2MeasurementDet( const GeomDet* gdet,
					      Phase2MeasurementConditionSet & conditions ) : 
    MeasurementDet (gdet),
    theDetConditions(&conditions)
  {
    if ( dynamic_cast<const PixelGeomDetUnit*>(gdet) == 0) {
      throw MeasurementDetException( "TkPhase2MeasurementDet constructed with a GeomDet which is not a PixelGeomDetUnit");
    }
  }

bool TkPhase2MeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
					  const MeasurementEstimator& est, const MeasurementTrackerEvent & data,
					  TempMeasurements & result) const {


  LogDebug("MeasurementTracker") << "TkPhase2MeasurementDet::measurements" ;
  if (!isActive(data)) {
    result.add(theInactiveHit, 0.F);
    return true;
  }
  
  auto oldSize = result.size();
  MeasurementDet::RecHitContainer && allHits = recHits(stateOnThisDet, data);
  for (auto && hit : allHits) {
    LogTrace("MeasurementTracker") << "adding one hit" ;
    LogTrace("MeasurementTracker") << " local position: " << (*hit).localPosition() ;
    LogTrace("MeasurementTracker") << " local error: " << (*hit).localPositionError() ;
    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    LogTrace("MeasurementTracker") << "with estimator result " << diffEst.second ;
    if ( diffEst.first)
      result.add(std::move(hit), diffEst.second);
  }

  if (result.size()>oldSize) return true;

  // create a TrajectoryMeasurement with an invalid RecHit and zero estimate
  bool inac = hasBadComponents(stateOnThisDet, data);
  result.add(inac ? theInactiveHit : theMissingHit, 0.F);
  return inac;

}

TrackingRecHit::RecHitPointer
TkPhase2MeasurementDet::buildRecHit( const Phase2TrackerCluster1DRef & cluster,
				    const LocalTrajectoryParameters & ltp) const
{
  const GeomDetUnit& gdu( specificGeomDet() );
  MeasurementPoint mpClu(cluster->center(), cluster->column() + 0.5);
  LocalPoint lp = gdu.topology().localPosition(mpClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError le = gdu.topology().localError(mpClu,meClu);
  return std::make_shared<Phase2TrackerRecHit1D>( lp, le, fastGeomDet(), cluster);

//  auto && params = cpe()->getParameters( * cluster, gdu, ltp );
//  return std::make_shared<SiPixelRecHit>( std::get<0>(params), std::get<1>(params), std::get<2>(params), fastGeomDet(), cluster);
}

TkPhase2MeasurementDet::RecHitContainer 
TkPhase2MeasurementDet::recHits( const TrajectoryStateOnSurface& ts, const MeasurementTrackerEvent & data ) const
{
  LogDebug("MeasurementTracker") << "TkPhase2MeasurementDet::recHits" ;
  RecHitContainer result;
  if (isEmpty(data.phase2OTData())== true ) return result;
  if (isActive(data) == false) return result;
  const Phase2TrackerCluster1D* begin=0;
  if (0 != data.phase2OTData().handle()->data().size()) {
     begin = &(data.phase2OTData().handle()->data().front());
  }
  const detset & detSet = data.phase2OTData().detSet(index());
  result.reserve(detSet.size());
  for ( const_iterator ci = detSet.begin(); ci != detSet.end(); ++ ci ) {
    
    if (ci < begin){
      edm::LogError("IndexMisMatch")<<"TkPhase2MeasurementDet cannot create hit because of index mismatch.";
      return result;
    }
     unsigned int index = ci-begin;
     if (!data.pixelClustersToSkip().empty() &&  index>=data.pixelClustersToSkip().size()){
       edm::LogError("IndexMisMatch")<<"TkPhase2MeasurementDet cannot create hit because of index mismatch. i.e "<<index<<" >= "<<data.pixelClustersToSkip().size();
       return result;
     }
     if(data.pixelClustersToSkip().empty() or (not data.pixelClustersToSkip()[index]) ) {
       Phase2TrackerCluster1DRef cluster = detSet.makeRefTo( data.phase2OTData().handle(), ci );
       result.push_back( buildRecHit( cluster, ts.localParameters() ) );
     }else{   
       LogDebug("TkPhase2MeasurementDet")<<"skipping this cluster from last iteration on "<<fastGeomDet().geographicalId().rawId()<<" key: "<<index;
     }
  }
  return result;
}

bool
TkPhase2MeasurementDet::hasBadComponents( const TrajectoryStateOnSurface &tsos, const MeasurementTrackerEvent & data ) const {
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
