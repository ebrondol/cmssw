#ifndef RecoTracker_MeasurementDet_VHMeasurementTracker_H
#define RecoTracker_MeasurementDet_VHMeasurementTracker_H

#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"

class VHMeasurementTracker : public MeasurementTracker {

 public:

  VHMeasurementTracker(const TrackerGeometry*  trackerGeom,
                       const GeometricSearchTracker* geometricSearchTracker):
                       MeasurementTracker(trackerGeom,geometricSearchTracker) {
    std::cout << "using VHMeasurementTracker!" << std::endl;
  }

  virtual ~VHMeasurementTracker() {};

  void update( const edm::Event&) const {return;};
  void updatePixels( const edm::Event&) const {return;};
  void updateStrips( const edm::Event&) const {return;};

  const MeasurementDet* idToDet(const DetId& id) const ; 

  void setClusterToSkip(const edm::InputTag & cluster, const edm::Event& event) const {return;};
  void unsetClusterToSkip() const {return;};

};

#endif
