#include "RecoTracker/MeasurementDet/plugins/Phase2MeasurementTracker.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"

#include "TkPixelMeasurementDet.h"
#include "TkStackMeasurementDet.h"

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
  sortTKD(theStackDets);

  return;
}

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
    const GeomDetUnit* gdu = dynamic_cast<const GeomDetUnit*>(*gd);
    if (gdu != 0) {
std::cout << "GeomDetUnit" << std::endl;
      addPixelDet(*gd, thePixelCPE);
    } else {
      const StackGeomDet* stackDet = dynamic_cast<const StackGeomDet*>(*gd);
      if (stackDet != 0) {
std::cout << "StackGeomDet" << std::endl;
        addStackDet(stackDet, thePixelCPE);
      }
    }

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

void Phase2MeasurementTracker::addStackDet( const StackGeomDet* gd, const PixelClusterParameterEstimator* cpe)
{
  TkStackMeasurementDet* det = new TkStackMeasurementDet( gd, cpe);
  theStackDets.push_back(det);
  initStackDet(*det);
//  theDetMap[gd->geographicalId()] = det;
  return;
}

void Phase2MeasurementTracker::initStackDet( TkStackMeasurementDet & det)
{
  /*
 const StackGeomDet& gd = det.specificGeomDet();
 const MeasurementDet* monoDet = findDet( gd.monoDet()->geographicalId());
 const MeasurementDet* stereoDet = findDet( gd.stereoDet()->geographicalId());
 if (monoDet == 0 || stereoDet == 0) {
   edm::LogError("MeasurementDet") << "MeasurementTracker ERROR: StackDet components not found as MeasurementDets ";
   throw MeasurementDetException("MeasurementTracker ERROR: StackDet components not found as MeasurementDets");
 }
 det.init(monoDet,stereoDet);
 theDetMap[gd.geographicalId()] = &det;
 */
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
