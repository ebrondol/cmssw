
#ifndef RECOTRACKER_TRANSIENTRACKINGRECHIT_TVectorHit_H
#define RECOTRACKER_TRANSIENTRACKINGRECHIT_TVectorHit_H

#include "DataFormats/TrackingRecHit/interface/VectorHit.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TValidTrackingRecHit.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "TrackingTools/TransientTrackingRecHit/interface/HelpertRecHit2DLocalPos.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// class GeomDetUnit;

class TVectorHit GCC11_FINAL : public TValidTrackingRecHit {
public:

  //typedef VectorHit::ClusterRef clusterRef;
  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;

  virtual ~TVectorHit() {}

  virtual AlgebraicVector parameters() const {return theHitData.parameters();}

  virtual AlgebraicSymMatrix parametersError() const {
    return HelpertRecHit2DLocalPos().parError( theHitData.localPositionError(), *det()); 
    // return theHitData->parametersError();
  }

  virtual AlgebraicMatrix projectionMatrix() const {return theHitData.projectionMatrix();}
  virtual int dimension() const {return theHitData.dimension();}

  virtual LocalPoint localPosition() const {return theHitData.localPosition();}
  virtual LocalError localPositionError() const {return theHitData.localPositionError();}

  virtual void getKfComponents( KfComponentsHolder & holder ) const {
      HelpertRecHit2DLocalPos().getKfComponents(holder, theHitData, *det()); 
  }

  virtual const TrackingRecHit * hit() const {return &theHitData;};
  
  virtual std::vector<const TrackingRecHit*> recHits() const {
    return hit()->recHits();
  }
  virtual std::vector<TrackingRecHit*> recHits() {
    return theHitData.recHits();
  }

  virtual bool canImproveWithTrack() const {return true;}

  //RC  virtual TVectorHit* clone (const TrajectoryStateOnSurface& ts) const;
  virtual RecHitPointer clone (const TrajectoryStateOnSurface& ts) const;

  virtual const GeomDetUnit* detUnit() const;

  // Extension of the TransientTrackingRecHit interface

  const VectorHit* specificHit() const {return &theHitData;};
  const PixelClusterParameterEstimator* cpe() const {return theCPE;}

  static RecHitPointer build( const GeomDet * geom, const VectorHit* rh, 
			      const PixelClusterParameterEstimator* cpe) {
    return RecHitPointer( new TVectorHit( geom, rh, cpe));
  }

  static RecHitPointer build( const Phase2TrackerCluster1DRef & clusterLower,
                              const Phase2TrackerCluster1DRef & clusterUpper,
                              const VectorHit2D vzx,
                              const VectorHit2D vzy,
                              const GeomDet* geom, 
			      const PixelClusterParameterEstimator* cpe) {
    return RecHitPointer( new TVectorHit( clusterLower, clusterUpper, vzx, vzy, geom, cpe ));
  }


  //!  Probability of the compatibility of the track with the pixel cluster shape.
  //virtual float clusterProbability() const {
  //  return theHitData.clusterProbability( theClusterProbComputationFlag );
  //}



private:
  const PixelClusterParameterEstimator* theCPE;
  VectorHit                             theHitData;
  unsigned int                          theClusterProbComputationFlag;


  /// This private constructor copies the TrackingRecHit.  It should be used when the 
  /// TrackingRecHit exist already in some collection.
  //TVectorHit(const GeomDet * geom, const VectorHit* rh, 
//		 const PixelClusterParameterEstimator* cpe,
//		 bool computeCoarseLocalPosition);

  TVectorHit(const GeomDet* geom,
             const VectorHit* vh,
             const PixelClusterParameterEstimator* cpe);


  TVectorHit( const Phase2TrackerCluster1DRef & clusterLower,
              const Phase2TrackerCluster1DRef & clusterUpper,
              const VectorHit2D vxz,
              const VectorHit2D vyz,
              const GeomDet* geom, 
              const PixelClusterParameterEstimator* cpe);


  virtual TVectorHit * clone() const {
    return new TVectorHit(*this);
  }

};



#endif
