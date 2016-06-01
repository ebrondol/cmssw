#ifndef DataFormats_Phase2ITPixelRecHit_h
#define DataFormats_Phase2ITPixelRecHit_h 1

//---------------------------------------------------------------------------
//!  \class Phase2ITPixelRecHit
//!  \brief Pixel Reconstructed Hit
//!
//!  A pixel hit is a 2D position and error in a given
//!  pixel sensor. It contains a persistent reference edm::Ref to the
//!  pixel cluster. 
//!
// EM 2016.05 development for phase2 pixel
// The IT IS chain should be  Phase2ITPixelRecHit->TrackerSingleRecHit->BaseTrackerRecHit-> TrackingRecHit
//---------------------------------------------------------------------------

#include "DataFormats/Common/interface/DetSetVectorNew.h"

//! Our base class
#include "DataFormats/TrackerRecHit2D/interface/TrackerSingleRecHit.h"

// 

class Phase2ITPixelRecHit final : public TrackerSingleRecHit {
  
public:
  typedef OmniClusterRef::Phase2ITPixelClusterRef CluRef;

  Phase2ITPixelRecHit()  {;}  
  ~Phase2ITPixelRecHit() {;}
  Phase2ITPixelRecHit( const LocalPoint& pos , const LocalError& err, 
		     GeomDet const & idet, 
		     CluRef const&  cluster) : TrackerSingleRecHit(pos,err,idet,cluster){}

  virtual Phase2ITPixelRecHit * clone() const override { return new Phase2ITPixelRecHit( * this); }
#ifndef __GCCXML__
  virtual RecHitPointer cloneSH() const override { return std::make_shared<Phase2ITPixelRecHit>(*this);}
#endif

  CluRef cluster()  const { return cluster_phase2IT(); }
  void setClusterRef(CluRef const & ref)  {setClusterPhase2ITPixelRef(ref);}

  virtual bool isPhase2() const override { return true; }
  //FIXME::check dimension of this!!
  virtual int dimension() const override {return 2;}
  virtual void getKfComponents( KfComponentsHolder & holder ) const override { getKfComponents2D(holder); }

 private:
  
};

#endif
