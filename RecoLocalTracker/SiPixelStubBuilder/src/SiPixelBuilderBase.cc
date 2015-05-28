#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderBase.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkStripCPERecord.h"

#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

SiPixelStubBuilderBase::SiPixelStubBuilderBase(const edm::ParameterSet& conf) :
  cpeTag(conf.getParameter<edm::ESInputTag>("CPE"))
{}

void SiPixelStubBuilderBase::initialize(const edm::EventSetup& es)
{
  //FIXME:ask Vincenzo
  /*
  uint32_t tk_cache_id = es.get<TrackerDigiGeometryRecord>().cacheIdentifier();
  uint32_t c_cache_id = es.get<TkStripCPERecord>().cacheIdentifier();

  if(tk_cache_id != tracker_cache_id) {
    es.get<TrackerDigiGeometryRecord>().get(tracker);
    tracker_cache_id = tk_cache_id;
  }
  if(c_cache_id != cpe_cache_id) {
    es.get<TkStripCPERecord>().get(matcherTag, matcher);
    es.get<TkStripCPERecord>().get(cpeTag, parameterestimator);
    cpe_cache_id = c_cache_id;
  }
  */

  // get the geometry
  edm::ESHandle< TrackerGeometry > geomHandle;
  es.get< TrackerDigiGeometryRecord >().get( geomHandle );
  theTkGeom = geomHandle.product();

  edm::ESHandle< TrackerTopology > tTopoHandle;
  es.get< IdealGeometryRecord >().get(tTopoHandle);
  theTkTopo = tTopoHandle.product();

  es.get< TkStripCPERecord >().get(cpeTag, parameterestimator);
}

unsigned int SiPixelStubBuilderBase::getLayerNumber(const DetId& detid) {
  if (detid.det() == DetId::Tracker) {
      if (detid.subdetId() == PixelSubdetector::PixelBarrel) {
        return (theTkTopo->pxbLayer(detid));
      }
      // ERICA::chech: E+ 100; E- 200 : needed?
      else if (detid.subdetId() == PixelSubdetector::PixelEndcap) {
        return (100 * theTkTopo->pxfSide(detid) + theTkTopo->pxfDisk(detid));
      }
      else return 999;
  }
  return 999;
}

unsigned int SiPixelStubBuilderBase::getModuleNumber(const DetId& detid) {
  if (detid.det() == DetId::Tracker) {
      if (detid.subdetId() == PixelSubdetector::PixelBarrel) {
        return ( theTkTopo->pxbModule(detid) );
      }
      else if (detid.subdetId() == PixelSubdetector::PixelEndcap) {
        return ( theTkTopo->pxfModule(detid) );
      }
      else return 999;
  }
  return 999;
}

void SiPixelStubBuilderBase::printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters){

  int nCluster = 0;
  int numberOfDSV = 0;
  edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter;
  for( DSViter = clusters.begin() ; DSViter != clusters.end(); DSViter++){

    ++numberOfDSV;

    // Loop over the clusters in the detector unit
    for (edmNew::DetSet< Phase2TrackerCluster1D >::const_iterator clustIt = DSViter->begin(); clustIt != DSViter->end(); ++clustIt) {

      nCluster++;
      // get the detector unit's id
      unsigned int rawid(DSViter->detId());
      DetId detId(rawid);

      // get the geom of the tracker
      const GeomDetUnit* geomDetUnit(theTkGeom->idToDetUnit(detId));
      const PixelGeomDetUnit* theGeomDet = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
      const PixelTopology& topol = theGeomDet->specificTopology();

      unsigned int layer = getLayerNumber(detId);
      unsigned int module = getModuleNumber(detId);
      std::cout << "Layer:" << layer << std::endl;
      if(topol.ncolumns() == 32) std::cout << "Pixel cluster with detId:" << rawid << "(module:" << module
             << ") in DetSet#" << numberOfDSV << std::endl;
      else if(topol.ncolumns() == 2 ) std::cout << "Strip cluster with detId " << rawid << "(module:" << module
             << ") in DetSet#" << numberOfDSV << std::endl;
      else std::cout << "no module?!" << std::endl;

      if (!geomDetUnit) break;

      //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
      MeasurementPoint mpClu(clustIt->center(), clustIt->column() + 0.5);
      Local3DPoint localPosClu = geomDetUnit->topology().localPosition(mpClu);
      Global3DPoint globalPosClu = geomDetUnit->surface().toGlobal(localPosClu);
      MeasurementError meClu(1./12,0.0,1./12);
      LocalError localErrClu = geomDetUnit->topology().localError(mpClu,meClu);

      std::cout << "\t global pos " << globalPosClu ;
      std::cout << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;

    }
  }
  std::cout << " Number of input clusters: " << nCluster << std::endl;

}
