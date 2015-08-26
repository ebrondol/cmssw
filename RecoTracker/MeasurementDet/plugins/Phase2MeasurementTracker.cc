#include "RecoTracker/MeasurementDet/plugins/Phase2MeasurementTracker.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"

#include "TkPixelMeasurementDet.h"

using namespace std;

namespace {

  struct CmpTKD {
    bool operator()(MeasurementDet const* rh, MeasurementDet const * lh) {
      return rh->fastGeomDet().geographicalId().rawId() < lh->fastGeomDet().geographicalId().rawId();
    }
    bool operator()(MeasurementDet const & rh, MeasurementDet const &  lh) {
      return rh.fastGeomDet().geographicalId().rawId() < lh.fastGeomDet().geographicalId().rawId();
    }
  };

  template<typename TKD>
  void sortTKD( std::vector<TKD*> & det) {
    std::sort(det.begin(),det.end(),CmpTKD());
  }
  template<typename TKD>
  void sortTKD( std::vector<TKD> & det) {
    std::sort(det.begin(),det.end(),CmpTKD());
  }

}


Phase2MeasurementTracker::Phase2MeasurementTracker(//const TrackerTopology*  trackerTopol,
                                                   const TrackerGeometry*  trackerGeom,
		                                   const GeometricSearchTracker* geometricSearchTracker,
			                           const PixelClusterParameterEstimator* pixelCPE):
			                           MeasurementTracker(trackerGeom,geometricSearchTracker),
						   thePixelCPE(pixelCPE),
						   theTkTopol(0)
{
  std::cout << "using Phase2MeasurementTracker!" << std::endl;
  this->initialize();
}

void Phase2MeasurementTracker::initialize()
{

  addPixelDets(theTrackerGeom->detsPXB());
  addPixelDets(theTrackerGeom->detsPXF());

  sortTKD(thePixelDets);

  return;
}
  //             // now the glued dets
  //               sortTKD(theGluedDets);
  //                 for (unsigned int i=0; i!=theGluedDets.size(); ++i)
  //                     initGluedDet(theGluedDets[i]);
  //
  //                       sortTKD(thePixelDets);
  //
  //
  //                       }

void Phase2MeasurementTracker::addPixelDets( const TrackingGeometry::DetContainer& dets )
{
  //FIXME::ERICA: in this moment in the PixelGeometry there is both inner and
  //outer tracker
  for (TrackerGeometry::DetContainer::const_iterator gd = dets.begin(); gd != dets.end(); gd++) {

//    DetId pixelId( (**gd).geographicalId() );
//    if (pixelId.subdetId() == PixelSubdetector::PixelBarrel) {
//      cout << theTkTopol.pxbLayer(pixelId) << endl;
//    } else if (pixelId.subdetId() == PixelSubdetector::PixelEndcap) {
//      cout << (100 * theTkTopol.pxfSide(pixelId) + theTkTopol.pxfDisk(pixelId)) << endl;
//    }

    addPixelDet(*gd, thePixelCPE);

  }
  return;
}

void Phase2MeasurementTracker::addPixelDet( const GeomDet* gd, const PixelClusterParameterEstimator* cpe )
{
  TkPixelMeasurementDet* det = new TkPixelMeasurementDet( gd, cpe);
  thePixelDets.push_back(det);
  //FIXME::ERICA: SkipClusters not implemented in this first version
  //det->setClusterToSkip(&thePixelsToSkip);
  theDetMap[gd->geographicalId()] = det;
  return;
}

const MeasurementDet* Phase2MeasurementTracker::idToDet(const DetId& id) const { 
  std::cout << "Phase2MeasurementTracker::idToDet" << std::endl;
  auto it = theDetMap.find(id);
  if( it != theDetMap.end() ) {
      return it->second;
  } else {
      //throw exception;
  }
  return 0; 
};
