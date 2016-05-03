
#ifndef RECOTRACKER_TRANSIENTRACKINGRECHIT_TSiPhase2RecHit_H
#define RECOTRACKER_TRANSIENTRACKINGRECHIT_TSiPhase2RecHit_H

#include "DataFormats/TrackerRecHit2D/interface/Phase2TrackerRecHit1D.h"
#include "Geometry/CommonTopologies/interface/Topology.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TValidTrackingRecHit.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "TrackingTools/TransientTrackingRecHit/interface/HelpertRecHit2DLocalPos.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

class TSiPhase2RecHit GCC11_FINAL : public TValidTrackingRecHit {
public:

  typedef Phase2TrackerRecHit1D::Phase2ClusterRef clusterRef;


  virtual ~TSiPhase2RecHit() {}

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

  //RC  virtual TSiPhase2RecHit* clone (const TrajectoryStateOnSurface& ts) const;
  virtual RecHitPointer clone (const TrajectoryStateOnSurface& ts) const;

  virtual const GeomDetUnit* detUnit() const;

  // Extension of the TransientTrackingRecHit interface

  const Phase2TrackerRecHit1D* specificHit() const {return &theHitData;};
  const PixelClusterParameterEstimator* cpe() const {return theCPE;}

  static RecHitPointer build( const GeomDet * geom, const Phase2TrackerRecHit1D* rh, 
			      const PixelClusterParameterEstimator* cpe,
			      bool computeCoarseLocalPosition=false) {
    return RecHitPointer( new TSiPhase2RecHit( geom, rh, cpe, computeCoarseLocalPosition));
  }

  static RecHitPointer build( const LocalPoint& pos, const LocalError& err,
			      const GeomDet* det, 
			      const clusterRef & cluster,
			      const PixelClusterParameterEstimator* cpe) {
    return RecHitPointer( new TSiPhase2RecHit( pos, err, det, cluster, cpe));
  }


  //!  Probability of the compatibility of the track with the pixel cluster shape.
  //virtual float clusterProbability() const {
  //  return theHitData.clusterProbability( theClusterProbComputationFlag );
  //}



private:
  const PixelClusterParameterEstimator* theCPE;
  Phase2TrackerRecHit1D                         theHitData;
  unsigned int                          theClusterProbComputationFlag;


  /// This private constructor copies the TrackingRecHit.  It should be used when the 
  /// TrackingRecHit exist already in some collection.
  TSiPhase2RecHit(const GeomDet * geom, const Phase2TrackerRecHit1D* rh, 
		 const PixelClusterParameterEstimator* cpe,
		 bool computeCoarseLocalPosition);



  /// Another private constructor.  It creates the TrackingRecHit internally, 
  /// avoiding redundent cloning.
  TSiPhase2RecHit( const LocalPoint& pos, const LocalError& err,
		  const GeomDet* det, 
		  const clusterRef & clust,
		  const PixelClusterParameterEstimator* cpe);


  virtual TSiPhase2RecHit * clone() const {
    return new TSiPhase2RecHit(*this);
  }

};



#endif
