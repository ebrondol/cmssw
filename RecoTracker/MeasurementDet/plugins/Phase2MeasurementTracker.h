#ifndef RecoTracker_MeasurementDet_Phase2MeasurementTracker_H
#define RecoTracker_MeasurementDet_Phase2MeasurementTracker_H

/* The purpose of this class is implementing MeasurementTrackerImpl
 * in a stacked geometry */

#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"

#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"

#include <unordered_map>

class TkPixelMeasurementDet;
class TkStackMeasurementDet;
class StackGeomDet;

class Phase2MeasurementTracker : public MeasurementTracker {

 public:

  Phase2MeasurementTracker(//const TrackerTopology*  trackerTopol,
                           const TrackerGeometry*  trackerGeom,
                           const GeometricSearchTracker* geometricSearchTracker,
                           const PixelClusterParameterEstimator* pixelCPE);

  virtual ~Phase2MeasurementTracker() {};

  typedef std::unordered_map<unsigned int,MeasurementDet*>   DetContainer;

  //initialize MeasurementDets
  void initialize();
  void addPixelDets( const TrackingGeometry::DetContainer& dets );
  void addPixelDet( const GeomDet* gd, const PixelClusterParameterEstimator* cpe );
  void addStackDet( const StackGeomDet* gd, const PixelClusterParameterEstimator* cpe );
  void initStackDet( TkStackMeasurementDet & det );

  //update
  void update( const edm::Event&) const {return;};
  void updatePixels( const edm::Event&) const {return;};
  void updateStrips( const edm::Event&) const {return;};

  const MeasurementDet* idToDet(const DetId& id) const ;

  void setClusterToSkip(const edm::InputTag & cluster, const edm::Event& event) const {return;};
  void unsetClusterToSkip() const {return;};

 private:
  const PixelClusterParameterEstimator* thePixelCPE;
  const TrackerTopology* theTkTopol;

  DetContainer theDetMap;
  std::vector<TkPixelMeasurementDet*> thePixelDets;
  std::vector<TkStackMeasurementDet*> theStackDets; 	//ERICA::why not a pointer??
};

#endif
