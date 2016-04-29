#ifndef TkPixelMeasurementDet_H
#define TkPixelMeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
//#include "DataFormats/SiPixelCluster/interface/SiPixelClusterFwd.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"

class TransientTrackingRecHit;
class LocalTrajectoryParameters;

class TkPixelMeasurementDet : public MeasurementDet {
public:

  typedef edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> SiPixelClusterRef;
  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  
  typedef edmNew::DetSet<SiPixelCluster> detset;
  typedef detset::const_iterator const_iterator;
  typedef edmNew::DetSet<Phase2TrackerCluster1D> phase2detset;
  typedef phase2detset::const_iterator ph2_const_iterator;
  typedef PixelClusterParameterEstimator::LocalValues    LocalValues;

  TkPixelMeasurementDet( const GeomDet* gdet,
			 const PixelClusterParameterEstimator* cpe);

  void update( const detset & detSet, 
	       const edm::Handle<edmNew::DetSetVector<SiPixelCluster> > & h,
	       unsigned int id ) { 
    detSet_ = detSet; 
    handle_ = h;
    id_ = id;
    empty = false;
    activeThisEvent_ = true;
  }

  void update( const phase2detset & detSet,
               const edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > & h,
               unsigned int id ) {
    phase2detSet_ = detSet;
    phase2handle_ = h;
    id_ = id;
    phase2empty = false;
    phase2activeThisEvent_ = true;
  }


  void setEmpty(){empty = true; activeThisEvent_ = true; }
  void setPhase2Empty(){phase2empty = true; phase2activeThisEvent_ = true; }

  virtual ~TkPixelMeasurementDet() { }

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface& ) const;

 virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			    const MeasurementEstimator& est,
			    TempMeasurements & result) const;


  const PixelGeomDetUnit& specificGeomDet() const {return static_cast<PixelGeomDetUnit const &>(fastGeomDet());}

  TransientTrackingRecHit::RecHitPointer 
  buildRecHit( const SiPixelClusterRef & cluster,
	       const LocalTrajectoryParameters & ltp) const;
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

  bool isPhase2Active() const { return phase2activeThisEvent_ && phase2activeThisPeriod_; }
  void setPhase2Active(bool active) { phase2activeThisPeriod_ = active; phase2activeThisEvent_ = true; if (!active) phase2empty = true; }
  void setPhase2ActiveThisEvent(bool active) { phase2activeThisEvent_ = active;  if (!active) phase2empty = true; }

  bool hasBadComponents( const TrajectoryStateOnSurface &tsos ) const ; 

  /** \brief Sets the list of bad ROCs, identified by the positions of their centers in the local coordinate frame*/
  void setBadRocPositions(std::vector< LocalPoint > & positions) { badRocPositions_.swap(positions); }
  /** \brief Clear the list of bad ROCs */
  void clearBadRocPositions() { badRocPositions_.clear(); }
private:

  const PixelClusterParameterEstimator* theCPE;
  edm::Handle<edmNew::DetSetVector<SiPixelCluster> > handle_;
  edm::Handle<edmNew::DetSetVector<Phase2TrackerCluster1D> > phase2handle_;
  detset detSet_;
  phase2detset phase2detSet_;
  std::vector< LocalPoint > badRocPositions_;
  const std::vector<bool>* skipClusters_;
  unsigned int id_;
  bool empty;
  bool phase2empty;
  bool activeThisEvent_, activeThisPeriod_;
  bool phase2activeThisEvent_, phase2activeThisPeriod_;
  
 public:

  inline bool accept(SiPixelClusterRefNew & r) const {
    
    if(0==skipClusters_ || skipClusters_->empty()) return true;
    if (r.key()>=skipClusters_->size()){
      edm::LogError("IndexMisMatch")<<r.key()<<" is larger than: "<<skipClusters_->size()<<" no skipping done";
      return true;
    }
    return not (*skipClusters_)[r.key()];
  }

  void setClusterToSkip(const std::vector<bool>*skip ){
     skipClusters_ = skip;
  }
};

#endif
