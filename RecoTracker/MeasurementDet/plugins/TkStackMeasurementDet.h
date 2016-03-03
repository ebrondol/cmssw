#ifndef TkStackMeasurementDet_H
#define TkStackMeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "TkPixelMeasurementDet.h"

#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"

//#include "CommonTools/Statistics/interface/LinearFit.h"
 
class TkStackMeasurementDet : public MeasurementDet {

 public:

  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;

  typedef PixelClusterParameterEstimator::LocalValues    LocalValues;

  TkStackMeasurementDet( const StackGeomDet* gdet, const PixelClusterParameterEstimator* cpe) ;
  void init(const MeasurementDet* lowerDet,
	    const MeasurementDet* upperDet);

  virtual ~TkStackMeasurementDet() { }; //delete theFitter; };

  void update( const detset & lowerDetSet,
               const detset & upperDetSet,
               const edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > & h) {
    theLowerDetSet = lowerDetSet;
    theUpperDetSet = upperDetSet;
    theHandle = h;
    Empty = false;
    Active = true;
  }

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface&) const;

  const StackGeomDet& specificGeomDet() const {return static_cast<StackGeomDet const&>(fastGeomDet());}

  virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			     const MeasurementEstimator& est,
			     TempMeasurements & result) const;
  TransientTrackingRecHit::RecHitPointer buildVectorHit( const Phase2TrackerCluster1DRef & clusterLower,
                                                         const Phase2TrackerCluster1DRef & clusterUpper,
                                                         const LocalTrajectoryParameters & ltp) const;

  const TkPixelMeasurementDet* lowerDet() const{ return theLowerDet;}
  const TkPixelMeasurementDet* upperDet() const{ return theUpperDet;}
  
  // set if the event is active
  void setActiveThisEvent(bool active) { Active = active; return; }
  void setEmpty() { Empty = true; Active = true; }
  bool isActive() const {return Active; }

  /// return TRUE if at least one of the lower and upper components has badChannels
  bool hasBadComponents( const TrajectoryStateOnSurface &tsos ) const {
    return (lowerDet()->hasBadComponents(tsos) || upperDet()->hasBadComponents(tsos));}
/*
  bool checkClustersCompatibility (Local3DPoint& posLower, Local3DPoint& posUpper, LocalError& errLower, LocalError& errUpper) const;
  void fit2Dzx(const Local3DPoint lpCI, const Local3DPoint lpCO,
               const LocalError leCI, const LocalError leCO,
               Local3DPoint& pos, Local3DVector& dir,
               AlgebraicSymMatrix22& covMatrix, double& chi2) const;
  void fit2Dzy(const Local3DPoint lpCI, const Local3DPoint lpCO,
               const LocalError leCI, const LocalError leCO,
               Local3DPoint& pos, Local3DVector& dir,
               AlgebraicSymMatrix22& covMatrix, double& chi2) const;

  void fit(const std::vector<float>& x,
           const std::vector<float>& y,
           const std::vector<float>& sigy,
           Local3DPoint& pos, Local3DVector& dir,
           AlgebraicSymMatrix22& covMatrix, double& chi2) const;
*/

 private:
  const PixelClusterParameterEstimator* thePixelCPE;
  const TkPixelMeasurementDet*       theLowerDet;
  const TkPixelMeasurementDet*       theUpperDet;
  detset theLowerDetSet, theUpperDetSet;
  edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > theHandle;
  bool Active, Empty;
//  LinearFit* theFitter;

};

#endif
