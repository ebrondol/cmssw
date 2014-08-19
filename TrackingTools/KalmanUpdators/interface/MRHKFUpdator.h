#ifndef _TRACKER_MRHKFUPDATOR_H_
#define _TRACKER_MRHKFUPDATOR_H_

/** \class MRHKFUpdator
 * Update trajectory state by combining predicted state and MRH measurements
 *
 * For each components i of the MRH:
 * x_filtered_i = x_pred + K_i * (measurement_i - H_i * x_pred) <BR> 
 *
 * The updated state will be:
 * x_filtered = sum(weight_i * x_filtered_i) + weight_pred * x_pred
 *
 *  \author brondolin
 */

#include "TrackingTools/PatternTools/interface/TrajectoryStateUpdator.h"
#include "TrackingTools/KalmanUpdators/interface/KFUpdator.h"

class MRHKFUpdator GCC11_FINAL : public TrajectoryStateUpdator {

public:

  // methods of Updator

  MRHKFUpdator() {
    theUpdator = new KFUpdator();
  } 

  TrajectoryStateOnSurface update(const TrajectoryStateOnSurface&,
                                  const TrackingRecHit&) const;

  TrajectoryStateOnSurface updateMRH(const TrajectoryStateOnSurface&,
                                  const TrackingRecHit&, float weight) const;

  template <unsigned int D> TrajectoryStateOnSurface updateMRH(const TrajectoryStateOnSurface&,
                                                            const TrackingRecHit&, float weight) const;

  virtual MRHKFUpdator * clone() const {
    return new MRHKFUpdator(*this);
  }

private:
  const TrajectoryStateUpdator* theUpdator; 
};

#endif
