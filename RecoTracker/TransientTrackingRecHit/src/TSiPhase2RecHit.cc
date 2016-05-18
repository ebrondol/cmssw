#include "RecoTracker/TransientTrackingRecHit/interface/TSiPhase2RecHit.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
//#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
//#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementError.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

TSiPhase2RecHit::RecHitPointer TSiPhase2RecHit::clone (const TrajectoryStateOnSurface& ts) const
{
  if (theCPE == 0){
    return new TSiPhase2RecHit( det(), &theHitData, 0,false);
  }else{
    const Phase2TrackerCluster1D& clust = *specificHit()->cluster();  
    //PixelClusterParameterEstimator::LocalValues lv = 
    //  theCPE->localParameters( clust, *detUnit(), ts);
    const PixelGeomDetUnit & gdu = (const PixelGeomDetUnit &) *(detUnit()) ;
    const PixelTopology * topo = &gdu.specificTopology();
  
    float pitch_x = topo->pitch().first;
    float pitch_y = topo->pitch().second;
    float ix = clust.center();
    float iy = clust.column()+0.5; // halfway the column
  
    LocalPoint lp( topo->localX(ix), topo->localY(iy), 0 );          // x, y, z
    LocalError le( pow(pitch_x, 2) / 12, 0, pow(pitch_y, 2) / 12);   // e2_xx, e2_xy, e2_yy
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
      //PixelClusterParameterEstimator::LocalValues lval= theCPE->localParameters(*rh->cluster(), *gdu);
      const PixelGeomDetUnit & pgdu = (const PixelGeomDetUnit &) *gdu ;
      const PixelTopology * topo = &pgdu.specificTopology();
  
      float pitch_x = topo->pitch().first;
      float pitch_y = topo->pitch().second;
      float ix = rh->cluster()->center();
      float iy = rh->cluster()->column()+0.5; // halfway the column
  
      LocalPoint lp( topo->localX(ix), topo->localY(iy), 0 );          // x, y, z
      LocalError le( pow(pitch_x, 2) / 12, 0, pow(pitch_y, 2) / 12);   // e2_xx, e2_xy, e2_yy
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
