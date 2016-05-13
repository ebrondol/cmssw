#ifndef DATAFORMATS_TRACKERRECHIT2D_PHASE2TRACKERRECHIT1D_H 
#define DATAFORMATS_TRACKERRECHIT2D_PHASE2TRACKERRECHIT1D_H 

#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "DataFormats/TrackerRecHit2D/interface/TrackerSingleRecHit.h"

class Phase2TrackerRecHit1D GCC11_FINAL : public TrackerSingleRecHit {

public:

  typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D> Phase2ClusterRef;

  Phase2TrackerRecHit1D() {}

  ~Phase2TrackerRecHit1D() {}

  Phase2TrackerRecHit1D( const LocalPoint& pos, const LocalError& err, 
                         GeomDet const & idet,
                         Phase2ClusterRef clus) : 
    TrackerSingleRecHit(pos,err,idet,clus)
  {}

  virtual Phase2TrackerRecHit1D * clone() const {return new Phase2TrackerRecHit1D( * this); }

  Phase2ClusterRef cluster()  const { return cluster_phase2OT(); }
  void setClusterRef(Phase2ClusterRef const & ref)  {setClusterPhase2Ref(ref);}
  virtual bool isPhase2() const override { return true; }
  //FIXME::check dimension of this!!
  virtual int dimension() const {return 2;}
  virtual void getKfComponents( KfComponentsHolder & holder ) const { getKfComponents2D(holder); }

private:

};

typedef edmNew::DetSetVector< Phase2TrackerRecHit1D > Phase2TrackerRecHit1DCollectionNew;

#endif
