#ifndef TkStackMeasurementDet_H
#define TkStackMeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "TkPixelMeasurementDet.h"

class StackGeomDet;
//class SiStripRecHitMatcher;
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TSiStripMatchedRecHit.h"
#include <TrackingTools/DetLayers/interface/MeasurementEstimator.h>
#include <TrackingTools/PatternTools/interface/TrajectoryMeasurement.h>


#include "FWCore/Utilities/interface/Visibility.h"


class TkStackMeasurementDet GCC11_FINAL : public MeasurementDet {

 public:

  TkStackMeasurementDet( const StackGeomDet* gdet, const PixelClusterParameterEstimator* cpe);
  void init(const MeasurementDet* innerDet,
	    const MeasurementDet* outerDet);

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface&) const;

  const StackGeomDet& specificGeomDet() const {return static_cast<StackGeomDet const&>(fastGeomDet());}

  virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			     const MeasurementEstimator& est,
			     TempMeasurements & result) const;

  const TkPixelMeasurementDet* innerDet() const{ return theInnerDet;}
  const TkPixelMeasurementDet* outerDet() const{ return theOuterDet;}

  /// return TRUE if both inner and outer components are active
  bool isActive() const {return innerDet()->isActive() && outerDet()->isActive(); }

  /// return TRUE if at least one of the inner and outer components has badChannels
  bool hasBadComponents( const TrajectoryStateOnSurface &tsos ) const {
    return (innerDet()->hasBadComponents(tsos) || outerDet()->hasBadComponents(tsos));}

 private:
  const PixelClusterParameterEstimator* theCPE;
  const TkPixelMeasurementDet*       theInnerDet;
  const TkPixelMeasurementDet*       theOuterDet;

/*
  template<typename Collector>
  void doubleMatch(const TrajectoryStateOnSurface& ts, Collector & collector) const  dso_internal;

  template<typename Collector>
  void collectRecHits(const TrajectoryStateOnSurface&, Collector &coll) const dso_internal;

  class dso_internal  HitCollectorForRecHits {
  public:
    typedef SiStripRecHitMatcher::Collector Collector;
    HitCollectorForRecHits(const GeomDet * geomDet,
			   const SiStripRecHitMatcher * matcher,
			   const StripClusterParameterEstimator* cpe,
			   RecHitContainer & target) ;
    void add(SiStripMatchedRecHit2D const& hit) {
      target_.push_back(
			TSiStripMatchedRecHit::build( geomDet_, std::auto_ptr<TrackingRecHit>(hit.clone()),
						      matcher_,cpe_)
			);
      hasNewHits_ = true;
    }
    void addProjected(const TransientTrackingRecHit& hit,
		      const GlobalVector & gdir) ;
    SiStripRecHitMatcher::Collector & collector() { return collector_; }
    bool hasNewMatchedHits() const { return hasNewHits_;  }
    void clearNewMatchedHitsFlag() { hasNewHits_ = false; }
    static bool filter() { return false;}   /// always fast as no estimator available here!
    size_t size() const { return target_.size();}

    static const MeasurementEstimator  & estimator() { static MeasurementEstimator * dummy=0; return *dummy;}

  private:
    const GeomDet              * geomDet_;
    const SiStripRecHitMatcher * matcher_;
    const StripClusterParameterEstimator* cpe_;
    RecHitContainer       & target_;
    SiStripRecHitMatcher::Collector collector_;
    bool hasNewHits_;
  };


  class dso_internal HitCollectorForFastMeasurements {
  public:
    typedef TransientTrackingRecHit::RecHitPointer RecHitPointer;
    typedef SiStripRecHitMatcher::Collector Collector;

    HitCollectorForFastMeasurements(const GeomDet * geomDet,
				    const SiStripRecHitMatcher * matcher,
				    const StripClusterParameterEstimator* cpe,
				    const TrajectoryStateOnSurface& stateOnThisDet,
				    const MeasurementEstimator& est,
				    TempMeasurements & target) ;
    void add(SiStripMatchedRecHit2D const& hit) ;
    void addProjected(const TransientTrackingRecHit& hit,
		      const GlobalVector & gdir) ;

    SiStripRecHitMatcher::Collector & collector() { return collector_; }
    bool hasNewMatchedHits() const { return hasNewHits_;  }
    void clearNewMatchedHitsFlag() { hasNewHits_ = false; }
    static bool filter() { return false;}   // if true mono-colection will been filter using the estimator before matching
    size_t size() const { return target_.size();}
    const MeasurementEstimator  & estimator() { return est_;}
  private:
    const GeomDet              * geomDet_;
    const SiStripRecHitMatcher * matcher_;
    const StripClusterParameterEstimator* cpe_;
    const TrajectoryStateOnSurface & stateOnThisDet_;
    const MeasurementEstimator     & est_;
    TempMeasurements & target_;
    SiStripRecHitMatcher::Collector collector_;
    bool hasNewHits_;
  };


  RecHitContainer
  projectOnGluedDet( const RecHitContainer& hits,
		     const TrajectoryStateOnSurface& ts) const dso_internal;

  template<typename HitCollector>
  void
  projectOnGluedDet( HitCollector & collector,
                     const RecHitContainer& hits,
                     const GlobalVector & gdir ) const  dso_internal;

  void checkProjection(const TrajectoryStateOnSurface& ts,
		       const RecHitContainer& monoHits,
		       const RecHitContainer& stereoHits) const;
  void checkHitProjection(const TransientTrackingRecHit& hit,
			  const TrajectoryStateOnSurface& ts,
			  const GeomDet& det) const dso_internal;

  // brief Test the strips on one of the two dets with projection
  bool testStrips(const TrajectoryStateOnSurface& tsos,
                  const BoundPlane &gluedPlane,
                  const TkStripMeasurementDet &mdet) const  dso_internal;
*/
};

#endif
