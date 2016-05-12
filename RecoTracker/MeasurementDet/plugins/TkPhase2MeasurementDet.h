#ifndef TkPhase2MeasurementDet_H
#define TkPhase2MeasurementDet_H

#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
//#include "DataFormats/SiPixelCluster/interface/SiPixelClusterFwd.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "RecoTracker/MeasurementDet/src/TkMeasurementDetSet.h"

class TrackingRecHit;
class LocalTrajectoryParameters;

class dso_hidden TkPhase2MeasurementDet final : public MeasurementDet {
public:

  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2TrackerCluster1DRef;
  
  typedef edmNew::DetSet<Phase2TrackerCluster1D> detset;
  typedef detset::const_iterator const_iterator;
  typedef PixelClusterParameterEstimator::LocalValues    LocalValues;

  TkPhase2MeasurementDet( const GeomDet* gdet,
			 Phase2MeasurementConditionSet & conditionSet );

  void update(Phase2MeasurementDetSet &data, const detset & detSet ) { 
    data.update(index(), detSet);
    data.setActiveThisEvent(index(), true);
  }

  void setEmpty(Phase2MeasurementDetSet & data) { data.setEmpty(index());  }
  bool isEmpty(const Phase2MeasurementDetSet & data) const {return data.empty(index());}

  virtual ~TkPhase2MeasurementDet() { }

  virtual RecHitContainer recHits( const TrajectoryStateOnSurface&, const MeasurementTrackerEvent & dat ) const;

 // simple hits
  virtual bool recHits(SimpleHitContainer & result,  
		       const TrajectoryStateOnSurface& stateOnThisDet, const MeasurementEstimator&, const MeasurementTrackerEvent & data) const {
    assert("not implemented for Pixel yet"==nullptr);
  }

 

  virtual bool measurements( const TrajectoryStateOnSurface& stateOnThisDet,
			    const MeasurementEstimator& est, const MeasurementTrackerEvent & dat,
			    TempMeasurements & result) const;


  const PixelGeomDetUnit& specificGeomDet() const {return static_cast<PixelGeomDetUnit const &>(fastGeomDet());}

  TrackingRecHit::RecHitPointer
  buildRecHit( const Phase2TrackerCluster1DRef & cluster,
  	       const LocalTrajectoryParameters & ltp) const;

  /** \brief Turn on/off the module for reconstruction, for the full run or lumi (using info from DB, usually). */
  void setActive(bool active) { conditionSet().setActive(index(), active); }
  /** \brief Turn on/off the module for reconstruction for one events.
             This per-event flag is cleared by any call to 'update' or 'setEmpty'  */
  void setActiveThisEvent(Phase2MeasurementDetSet & data, bool active) const { data.setActiveThisEvent(index(), active); }
  /** \brief Is this module active in reconstruction? It must be both 'setActiveThisEvent' and 'setActive'. */
  bool isActive(const MeasurementTrackerEvent & data) const { return data.pixelData().isActive(index()); }

  bool hasBadComponents( const TrajectoryStateOnSurface &tsos, const MeasurementTrackerEvent & dat ) const ; 

  /** \brief Sets the list of bad ROCs, identified by the positions of their centers in the local coordinate frame*/
//  void setBadRocPositions(std::vector< LocalPoint > & positions) { badRocPositions_.swap(positions); }
  /** \brief Clear the list of bad ROCs */
//  void clearBadRocPositions() { badRocPositions_.clear(); }

  int index() const { return index_; }
  void setIndex(int i) { index_ = i; }

private:
  unsigned int id_;
//  std::vector< LocalPoint > badRocPositions_;

  int index_;
  Phase2MeasurementConditionSet * theDetConditions;
  Phase2MeasurementConditionSet & conditionSet() { return *theDetConditions; }
  const Phase2MeasurementConditionSet & conditionSet() const { return *theDetConditions; }

  const PixelClusterParameterEstimator * cpe() const { return conditionSet().pixelCPE(); }

 public:
/*
  inline bool accept(Phase2TrackerCluster1DRefNew & r, const std::vector<bool> skipClusters) const {
    
    if(skipClusters.empty()) return true;
    if (r.key()>=skipClusters.size()){
      edm::LogError("IndexMisMatch")<<r.key()<<" is larger than: "<<skipClusters.size()<<" no skipping done";
      return true;
    }
    return not skipClusters[r.key()];
  }
*/
};

#endif
