#ifndef RecoLocalTracker_Phase2ITPixelRecHits_Phase2ITPixelRecHitConverter_h
#define RecoLocalTracker_Phase2ITPixelRecHits_Phase2ITPixelRecHitConverter_h

//---------------------------------------------------------------------------
//! \class Phase2ITPixelRecHitConverter
//!
//! \brief EDProducer to covert Phase2ITPixelClusters into Phase2ITPixelRecHits
//!
//! Phase2ITPixelRecHitConverter is an EDProducer subclass (i.e., a module)
//! which orchestrates the conversion of Phase2ITPixelClusters into Phase2ITPixelRecHits.
//! Consequently, the input is a edm::DetSetVector<Phase2ITPixelCluster> and the output is
//! Phase2ITPixelRecHitCollection.
//!
//! Phase2ITPixelRecHitConverter invokes one of descendents from 
//! ClusterParameterEstimator (templated on Phase2ITPixelCluster), e.g.
//! CPEFromDetPosition (which is the only available option 
//! right now).  Phase2ITPixelRecHitConverter loads the Phase2ITPixelClusterCollection,
//! and then iterates over DetIds, invoking the chosen CPE's methods
//! localPosition() and localError() to perform the correction (some of which
//! may be rather involved).  A RecHit is made on the spot, and appended
//! to the output collection.
//!
//! The calibrations are not loaded at the moment, 
//! although that is being planned for the near future.
//!
//! \author Porting from ORCA by Petar Maksimovic (JHU). Implementation of the
//!         DetSetVector by V.Chiochia (Zurich University).
//!
//! \version v2, May 30, 2006  
//! change to use Lorentz angle from DB Lotte Wilke, Jan. 31st, 2008
//!
//---------------------------------------------------------------------------

//--- Base class for CPEs:

#include "RecoLocalTracker/Phase2ITPixelRecHits/interface/Phase2PixelCPEBase.h"

//--- Geometry + DataFormats
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "DataFormats/Phase2ITPixelCluster/interface/Phase2ITPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/Phase2ITPixelRecHitCollection.h"
#include "DataFormats/Common/interface/DetSetVector.h"

//--- Framework
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

//#define TP_OLD
#ifdef TP_OLD
#include "FWCore/Framework/interface/Handle.h"
#else
#include "DataFormats/Common/interface/Handle.h"
#endif
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

class MagneticField;

  class Phase2ITPixelRecHitConverter : public edm::stream::EDProducer<>
  {
  public:
    //--- Constructor, virtual destructor (just in case)
    explicit Phase2ITPixelRecHitConverter(const edm::ParameterSet& conf);
    virtual ~Phase2ITPixelRecHitConverter();

    //--- Factory method to make CPE's depending on the ParameterSet
    //--- Not sure if we need to make more than one CPE to run concurrently
    //--- on different parts of the detector (e.g., one for the barrel and the 
    //--- one for the forward).  The way the CPE's are written now, it's
    //--- likely we can use one (and they will switch internally), or
    //--- make two of the same but configure them differently.  We need a more
    //--- realistic use case...

    //--- The top-level event method.
    virtual void produce(edm::Event& e, const edm::EventSetup& c) override;

    //--- Execute the position estimator algorithm(s).
    //--- New interface with DetSetVector
    void run(const edmNew::DetSetVector<Phase2ITPixelCluster>& input,
	     Phase2ITPixelRecHitCollectionNew & output,
	     edm::ESHandle<TrackerGeometry> & geom);

    void run(edm::Handle<edmNew::DetSetVector<Phase2ITPixelCluster> >  inputhandle,
	     Phase2ITPixelRecHitCollectionNew & output,
	     edm::ESHandle<TrackerGeometry> & geom);

  private:
    edm::ParameterSet conf_;
    // TO DO: maybe allow a map of pointers?
    std::string cpeName_="None";                      // what the user said s/he wanted
    /// const PixelClusterParameterEstimator * cpe_;  // what we got (for now, one ptr to base class)
    Phase2PixelCPEBase const * cpe_=nullptr;          // What we got (for now, one ptr to base class)
    edm::InputTag src_;
    edm::EDGetTokenT<edmNew::DetSetVector<Phase2ITPixelCluster>> tPhase2ITPixelCluster;
    bool m_newCont; // save also in emdNew::DetSetVector
  };

#endif
