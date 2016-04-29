#include "TkPixelMeasurementDet.h"
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

TkPixelMeasurementDet::TkPixelMeasurementDet( const GeomDet* gdet,
					      const PixelClusterParameterEstimator* cpe) :
    MeasurementDet (gdet),
    theCPE(cpe),
    skipClusters_(0),
    empty(true),
    activeThisEvent_(true), activeThisPeriod_(true)
  {
    if ( dynamic_cast<const PixelGeomDetUnit*>(gdet) == 0) {
      throw MeasurementDetException( "TkPixelMeasurementDet constructed with a GeomDet which is not a PixelGeomDetUnit");
    }
  }

bool TkPixelMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
					  const MeasurementEstimator& est,
					  TempMeasurements & result) const {

  LogDebug("MeasurementTracker") << "measurements collected from TkPixelMeasurementDet ... " << std::endl;
  if (!isActive() && !isPhase2Active()) {
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


TransientTrackingRecHit::RecHitPointer
TkPixelMeasurementDet::buildRecHit( const SiPixelClusterRef & cluster,
				    const LocalTrajectoryParameters & ltp) const
{
  const GeomDetUnit& gdu( specificGeomDet());
  LocalValues lv = theCPE->localParameters( * cluster, gdu, ltp );
  return TSiPixelRecHit::build( lv.first, lv.second, &fastGeomDet(), cluster, theCPE);
}

TransientTrackingRecHit::RecHitPointer
TkPixelMeasurementDet::buildRecHit( const Phase2TrackerCluster1DRef & cluster,
				    const LocalTrajectoryParameters & ltp) const
{
  const GeomDetUnit& gdu( specificGeomDet() );
  MeasurementPoint mpClu(cluster->center(), cluster->column() + 0.5);
  LocalPoint lp = gdu.topology().localPosition(mpClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError le = gdu.topology().localError(mpClu,meClu);

//  float strippitch  = 0.0090; // hardcoded dummy, a la 2S
//  float striplength = 5.;     // hardcoded dummy, a la 2S
//  std::pair<float, float> barycenter = (*cluster).barycenter();
//  LocalPoint lp( barycenter.second * strippitch, striplength * (barycenter.first + 1/2), 0 );
//  LocalError le( strippitch / 12, 0, striplength / 12 );
//  LocalValues lv = theCPE->localParameters( * cluster, gdu, ltp );
  return TSiPixelRecHit::build( lp, le, &fastGeomDet(), cluster, theCPE);
}

TkPixelMeasurementDet::RecHitContainer
TkPixelMeasurementDet::recHits( const TrajectoryStateOnSurface& ts ) const
{
  LogDebug("MeasurementTracker") << "rechits collected from TkPixelMeasurementDet ... " << std::endl;
  RecHitContainer result;
  LogTrace("MeasurementTracker") << "TkPixelMeasurementDet empty:" << empty << std::endl;
  LogTrace("MeasurementTracker") << "TkPixelMeasurementDet ph2empty:" << phase2empty << std::endl;
  LogTrace("MeasurementTracker") << "TkPixelMeasurementDet active:" << isActive() << std::endl;
  LogTrace("MeasurementTracker") << "TkPixelMeasurementDet ph2active:" << isPhase2Active() << std::endl;
//  if (empty == true ) return result;
  if ( !empty && isActive() == true){
    LogDebug("MeasurementTracker") << "TkPixelMeasurementDet active and not empty ... " << std::endl;
    const SiPixelCluster* begin=0;
    if(0!=handle_->data().size()) {
       begin = &(handle_->data().front());
    }
    LogDebug("MeasurementTracker") << "rechits collected from TkPixelMeasurementDet 2 ... " << std::endl;
    result.reserve(detSet_.size());
    for ( const_iterator ci = detSet_.begin(); ci != detSet_.end(); ++ ci ) {
  
      if (ci < begin){
        edm::LogError("IndexMisMatch")<<"TkPixelMeasurementDet cannot create hit because of index mismatch.";
        return result;
      }

      unsigned int index = ci-begin;
      if (skipClusters_!=0 && skipClusters_->size()!=0 &&  index>=skipClusters_->size()){
        edm::LogError("IndexMisMatch")<<"TkPixelMeasurementDet cannot create hit because of index mismatch. i.e "<<index<<" >= "<<skipClusters_->size();
        return result;
      }
      if(0==skipClusters_ or skipClusters_->empty() or (not (*skipClusters_)[index]) ) {
        SiPixelClusterRef cluster = edmNew::makeRefTo( handle_, ci );
        result.push_back( buildRecHit( cluster, ts.localParameters() ) );
      }else{
        LogDebug("TkPixelMeasurementDet")<<"skipping this cluster from last iteration on "<<fastGeomDet().geographicalId().rawId()<<" key: "<<index;
      }
    }

  } else if ( !phase2empty && isPhase2Active() == true){
    LogDebug("MeasurementTracker") << "TkPixelMeasurementDet phase2active and not empty ... " << std::endl;
    const Phase2TrackerCluster1D* ph2begin=0;
    if(0!=phase2handle_->data().size()) {
       ph2begin = &(phase2handle_->data().front());
    }
    LogDebug("MeasurementTracker") << "rechits collected from TkPixelMeasurementDet 3 ... " << std::endl;
  
    LogDebug("MeasurementTracker") << "rechits collected from TkPixelMeasurementDet 4 ... " << std::endl;
    for ( ph2_const_iterator ci = phase2detSet_.begin(); ci != phase2detSet_.end(); ++ ci ) {
      if (ci < ph2begin){
        edm::LogError("IndexMisMatch")<<"TkPixelMeasurementDet cannot create hit because of index mismatch.";
        return result;
      }
      unsigned int index = ci-ph2begin;
      if (skipClusters_!=0 && skipClusters_->size()!=0 &&  index>=skipClusters_->size()){
        edm::LogError("IndexMisMatch")<<"TkPixelMeasurementDet cannot create hit because of index mismatch. i.e "<<index<<" >= "<<skipClusters_->size();
        return result;
      }
      if(0==skipClusters_ or skipClusters_->empty() or (not (*skipClusters_)[index]) ) {
        Phase2TrackerCluster1DRef cluster = edmNew::makeRefTo( phase2handle_, ci );
        result.push_back( buildRecHit( cluster, ts.localParameters() ) );
      }else{
        LogDebug("TkPixelMeasurementDet")<<"skipping this cluster from last iteration on "<<fastGeomDet().geographicalId().rawId()<<" key: "<<index;
      }
   }
  
  }
  return result;
}

bool
TkPixelMeasurementDet::hasBadComponents( const TrajectoryStateOnSurface &tsos ) const {
    if (badRocPositions_.empty()) return false;
    LocalPoint lp = tsos.localPosition();
    LocalError le = tsos.localError().positionError();
    double dx = 3*std::sqrt(le.xx()) + theRocWidth, dy = 3*std::sqrt(le.yy()) + theRocHeight;
    for (std::vector<LocalPoint>::const_iterator it = badRocPositions_.begin(), ed = badRocPositions_.end(); it != ed; ++it) {
        if ( (std::abs(it->x() - lp.x()) < dx) &&
             (std::abs(it->y() - lp.y()) < dy) ) return true;
    }
    return false;
}
