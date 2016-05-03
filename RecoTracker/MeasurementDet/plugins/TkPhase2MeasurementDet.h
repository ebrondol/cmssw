#ifndef TkPhase2MeasurementDet_H
#define TkPhase2MeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"

class TransientTrackingRecHit;
class LocalTrajectoryParameters;

class TkPhase2MeasurementDet : public MeasurementDet {
public:

  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;
  typedef PixelClusterParameterEstimator::LocalValues    LocalValues;

  TkPhase2MeasurementDet( const GeomDet* gdet,
			 const PixelClusterParameterEstimator* cpe);

  void update( const detset & detSet, 
	       const edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > & h,
	       unsigned int id ) { 
    detSet_ = detSet; 
    handle_ = h;
    id_ = id;
    empty = false;
    activeThisEvent_ = true;
  }

  void setEmpty(){empty = true; activeThisEvent_ = true; }

  virtual ~TkPhase2MeasurementDet() { }

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface& ) const;

 virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			    const MeasurementEstimator& est,
			    TempMeasurements & result) const;


  const PixelGeomDetUnit& specificGeomDet() const {return static_cast<PixelGeomDetUnit const &>(fastGeomDet());}

  TransientTrackingRecHit::RecHitPointer 
  buildRecHit( const Phase2TrackerCluster1DRef & cluster,
	       const LocalTrajectoryParameters & ltp) const;

  /** \brief Turn on/off the module for reconstruction, for the full run or lumi (using info from DB, usually).
             This also resets the 'setActiveThisEvent' to true */
  void setActive(bool active) { activeThisPeriod_ = active; activeThisEvent_ = true; if (!active) empty = true; }
  /** \brief Turn on/off the module for reconstruction for one events.
             This per-event flag is cleared by any call to 'update' or 'setEmpty'  */
  void setActiveThisEvent(bool active) { activeThisEvent_ = active;  if (!active) empty = true; }
  /** \brief Is this module active in reconstruction? It must be both 'setActiveThisEvent' and 'setActive'. */
  bool isActive() const { return activeThisEvent_ && activeThisPeriod_; }

  bool hasBadComponents( const TrajectoryStateOnSurface &tsos ) const ; 

  /** \brief Sets the list of bad ROCs, identified by the positions of their centers in the local coordinate frame*/
  void setBadRocPositions(std::vector< LocalPoint > & positions) { badRocPositions_.swap(positions); }
  /** \brief Clear the list of bad ROCs */
  void clearBadRocPositions() { badRocPositions_.clear(); }
private:

  const PixelClusterParameterEstimator* theCPE;
  edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > handle_;
  detset detSet_;
  std::vector< LocalPoint > badRocPositions_;
  unsigned int id_;
  bool empty;
  bool activeThisEvent_, activeThisPeriod_;
};

#endif
