#ifndef RecoLocalTracker_Phase2ITPixelCluster_Parameter_Estimator_H
#define RecoLocalTracker_Phase2ITPixelCluster_Parameter_Estimator_H

#include "DataFormats/GeometrySurface/interface/LocalError.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "DataFormats/Phase2ITPixelCluster/interface/Phase2ITPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitQuality.h"
#include<tuple>

class Phase2PixelClusterParameterEstimator
{
  public:

  virtual ~Phase2PixelClusterParameterEstimator(){}
  
  typedef std::pair<LocalPoint,LocalError>  LocalValues;
  typedef std::vector<LocalValues> VLocalValues;

  using ReturnType = std::tuple<LocalPoint,LocalError,SiPixelRecHitQuality::QualWordType>;

  // here just to implement it in the clients;
  // to be properly implemented in the sub-classes in order to make them thread-safe

  virtual ReturnType getParameters(const Phase2ITPixelCluster & cl, 
                                   const GeomDetUnit    & det) const =0;

  virtual ReturnType getParameters(const Phase2ITPixelCluster & cl, 
				   const GeomDetUnit    & det, 
				   const LocalTrajectoryParameters & ltp ) const =0;

  virtual ReturnType getParameters(const Phase2ITPixelCluster & cl, 
				   const GeomDetUnit    & det, 
				   const TrajectoryStateOnSurface& tsos ) const {
    return getParameters(cl,det,tsos.localParameters());
  }

  virtual VLocalValues localParametersV(const Phase2ITPixelCluster& cluster, const GeomDetUnit& gd) const {
    VLocalValues vlp;
    ReturnType tuple = getParameters(cluster, gd);
    vlp.push_back(std::make_pair(std::get<0>(tuple), std::get<1>(tuple)));
    return vlp;
  }
  virtual VLocalValues localParametersV(const Phase2ITPixelCluster& cluster, const GeomDetUnit& gd, TrajectoryStateOnSurface& tsos) const {
    VLocalValues vlp;
    ReturnType tuple = getParameters(cluster,  gd, tsos);
    vlp.push_back(std::make_pair(std::get<0>(tuple), std::get<1>(tuple)));
    return vlp;
  }

};

#endif
