#ifndef TkStackMeasurementDet_H
#define TkStackMeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "TkPhase2OTMeasurementDet.h"

#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderEDProducer.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"

#include "FWCore/Utilities/interface/Visibility.h"

// FIXME::TkStackMeasurementDet in this moment is just a prototype: to be fixed soon!

class TkStackMeasurementDet GCC11_FINAL : public MeasurementDet {

 public:

  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;

  TkStackMeasurementDet( const StackGeomDet* gdet, const VectorHitBuilderEDProducer* matcher, const PixelClusterParameterEstimator* cpe);
  void init(const MeasurementDet* lowerDet,
	    const MeasurementDet* upperDet);

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface&, const MeasurementTrackerEvent & data) const;

  const StackGeomDet& specificGeomDet() const {return static_cast<StackGeomDet const&>(fastGeomDet());}

  virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			     const MeasurementEstimator& est, const MeasurementTrackerEvent & data,
			     TempMeasurements & result) const;

  const TkPhase2OTMeasurementDet* lowerDet() const{ return theLowerDet;}
  const TkPhase2OTMeasurementDet* upperDet() const{ return theUpperDet;}

  /// return TRUE if both lower and upper components are active
  bool isActive(const MeasurementTrackerEvent & data) const {return lowerDet()->isActive(data) && upperDet()->isActive(data); }
  bool isEmpty(const Phase2OTMeasurementDetSet & data) const {return data.empty(lowerDet()->index()) && data.empty(upperDet()->index());}

  /// return TRUE if at least one of the lower and upper components has badChannels
  bool hasBadComponents( const TrajectoryStateOnSurface &tsos, const MeasurementTrackerEvent & data ) const {
    return (lowerDet()->hasBadComponents(tsos, data) || upperDet()->hasBadComponents(tsos, data));}

 private:
  const VectorHitBuilderEDProducer* theMatcher;
  const PixelClusterParameterEstimator* thePixelCPE;
  const TkPhase2OTMeasurementDet*       theLowerDet;
  const TkPhase2OTMeasurementDet*       theUpperDet;

};

#endif
