#include "RecoTracker/TransientTrackingRecHit/interface/TSiPhase2RecHit.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementError.h"

TSiPhase2RecHit::RecHitPointer TSiPhase2RecHit::clone (const TrajectoryStateOnSurface& ts) const
{
  if (theCPE == 0){
    return new TSiPhase2RecHit( det(), &theHitData, 0,false);
  }else{
    const Phase2TrackerCluster1D& clust = *specificHit()->cluster();  
  MeasurementPoint mpClu(clust.center(), clust.column() + 0.5);
  LocalPoint lp = detUnit()->topology().localPosition(mpClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError le = detUnit()->topology().localError(mpClu,meClu);
//    PixelClusterParameterEstimator::LocalValues lv = 
//      theCPE->localParameters( clust, *detUnit(), ts);
    return TSiPhase2RecHit::build( lp, le, det(), specificHit()->cluster(), theCPE);
  }
}

const GeomDetUnit* TSiPhase2RecHit::detUnit() const
{
  return static_cast<const GeomDetUnit*>(det());
}



// This private constructor copies the TrackingRecHit.  It should be used when the 
// TrackingRecHit exist already in some collection.
TSiPhase2RecHit::TSiPhase2RecHit(const GeomDet * geom, const Phase2TrackerRecHit1D* rh, 
			       const PixelClusterParameterEstimator* cpe,
			       bool computeCoarseLocalPosition) : 
  TValidTrackingRecHit(geom, *rh), theCPE(cpe), theHitData(*rh)
{
  if (! (rh->hasPositionAndError() || !computeCoarseLocalPosition)) {
    const GeomDetUnit* gdu = dynamic_cast<const GeomDetUnit*>(geom);
    if (gdu){
      MeasurementPoint mpClu(rh->cluster()->center(), rh->cluster()->column() + 0.5);
      LocalPoint lp = (*gdu).topology().localPosition(mpClu);
      MeasurementError meClu(1./12,0.0,1./12);
      LocalError le = gdu->topology().localError(mpClu,meClu);
//      PixelClusterParameterEstimator::LocalValues lval= theCPE->localParameters(*rh->cluster(), *gdu);
      LogDebug("TSiPhase2RecHit")<<"calculating coarse position/error.";
      theHitData = Phase2TrackerRecHit1D(lp, le, geom->geographicalId(),rh->cluster());
    }else{
      edm::LogError("TSiPhase2RecHit") << " geomdet does not cast into geomdet unit. cannot create pixel local parameters.";
    }
  }

}



// Another private constructor.  It creates the TrackingRecHit internally, 
// avoiding redundent cloning.
TSiPhase2RecHit::TSiPhase2RecHit( const LocalPoint& pos, const LocalError& err,
				const GeomDet* det, 
				clusterRef const & clust,
				const PixelClusterParameterEstimator* cpe) :
  TValidTrackingRecHit(det), theCPE(cpe),
  theHitData( pos, err, det->geographicalId(), clust)
{
}
