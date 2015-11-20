#include "RecoTracker/TransientTrackingRecHit/interface/TVectorHit.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include<typeinfo>

TVectorHit::RecHitPointer TVectorHit::clone (const TrajectoryStateOnSurface& ts) const
{
  if (theCPE == 0){
    return new TVectorHit( det(), &theHitData, 0);
  }else{
/*    const SiPixelCluster& clust = *specificHit()->cluster();  
    PixelClusterParameterEstimator::LocalValues lv = 
      theCPE->localParameters( clust, *detUnit(), ts);
    return TVectorHit::build( lv.first, lv.second, det(), specificHit()->cluster(), theCPE);
*/
    return new TVectorHit( det(), &theHitData, 0);
  }
}

const GeomDetUnit* TVectorHit::detUnit() const
{
  return static_cast<const GeomDetUnit*>(det());
}



// This private constructor copies the TrackingRecHit.  It should be used when the 
// TrackingRecHit exist already in some collection.
/*
TVectorHit::TVectorHit(const GeomDet * geom, const VectorHit* rh, 
			       const PixelClusterParameterEstimator* cpe,
			       bool computeCoarseLocalPosition) : 
  TValidTrackingRecHit(geom, *rh), theCPE(cpe), theHitData(*rh)
{
  if (! (rh->hasPositionAndError() || !computeCoarseLocalPosition)) {
    const GeomDetUnit* gdu = dynamic_cast<const GeomDetUnit*>(geom);
    if (gdu){
      PixelClusterParameterEstimator::LocalValues lval= theCPE->localParameters(*rh->cluster(), *gdu);
      LogDebug("TVectorHit")<<"calculating coarse position/error.";
      theHitData = VectorHit(lval.first, lval.second,geom->geographicalId(),rh->cluster());
    }else{
      edm::LogError("TVectorHit") << " geomdet does not cast into geomdet unit. cannot create pixel local parameters.";
    }
  }

  // Additionally, fill the SiPixeRecHitQuality from the PixelCPE.
  theHitData.setRawQualityWord( cpe->rawQualityWord() );
  theClusterProbComputationFlag = cpe->clusterProbComputationFlag(); 
}
*/


TVectorHit::TVectorHit(const GeomDet* geom,
              const VectorHit* vh,
              const PixelClusterParameterEstimator* cpe):
  TValidTrackingRecHit(geom), theCPE(cpe), theHitData(vh)
{};

TVectorHit::TVectorHit( const Phase2TrackerCluster1DRef & clusterLower,
              const Phase2TrackerCluster1DRef & clusterUpper,
              const VectorHit2D vzx,
              const VectorHit2D vzy,
              const GeomDet* geom, 
              const PixelClusterParameterEstimator* cpe) :
  TValidTrackingRecHit(geom), theCPE(cpe), theHitData( VectorHit(geom->geographicalId(), vzx, vzy, clusterLower, clusterUpper))
{};
