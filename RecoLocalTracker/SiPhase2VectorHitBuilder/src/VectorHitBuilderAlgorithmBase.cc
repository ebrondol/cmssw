#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithmBase.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkStripCPERecord.h"

#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

VectorHitBuilderAlgorithmBase::VectorHitBuilderAlgorithmBase(const edm::ParameterSet& conf) :
  nMaxVHforeachStack(conf.getParameter<int>("maxVectorHitsinaStack")),
  cpeTag(conf.getParameter<edm::ESInputTag>("CPE"))
{}

void VectorHitBuilderAlgorithmBase::initialize(const edm::EventSetup& es)
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
  initTkGeom(geomHandle);

  edm::ESHandle< TrackerTopology > tTopoHandle;
  es.get< TrackerTopologyRcd >().get(tTopoHandle);
  initTkTopo(tTopoHandle);

  es.get< TkStripCPERecord >().get(cpeTag, parameterestimator);
}

void VectorHitBuilderAlgorithmBase::initTkGeom(edm::ESHandle< TrackerGeometry > tkGeomHandle){
  theTkGeom = tkGeomHandle.product();
}
void VectorHitBuilderAlgorithmBase::initTkTopo(edm::ESHandle< TrackerTopology > tkTopoHandle){
  theTkTopo = tkTopoHandle.product();
}

void VectorHitBuilderAlgorithmBase::printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters){

  int nCluster = 0;
  int numberOfDSV = 0;
  edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter;
  for( DSViter = clusters.begin() ; DSViter != clusters.end(); DSViter++){

    ++numberOfDSV;

    // Loop over the clusters in the detector unit
    for (edmNew::DetSet< Phase2TrackerCluster1D >::const_iterator clustIt = DSViter->begin(); clustIt != DSViter->end(); ++clustIt) {

      nCluster++;

      // get the detector unit's id
      const GeomDetUnit* geomDetUnit(theTkGeom->idToDetUnit(DSViter->detId()));
      if (!geomDetUnit) return;

      printCluster(geomDetUnit, clustIt);

    }
  }
  LogDebug("VectorHitBuilder") << " Number of input clusters: " << nCluster << std::endl;

}


void VectorHitBuilderAlgorithmBase::printCluster(const GeomDet* geomDetUnit, const Phase2TrackerCluster1D* clustIt){

  if (!geomDetUnit) return;

  const PixelGeomDetUnit* pixelGeomDetUnit = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
  const PixelTopology& topol = pixelGeomDetUnit->specificTopology();
  if (!pixelGeomDetUnit) return;

  unsigned int layer = theTkTopo->layer(geomDetUnit->geographicalId());
  unsigned int module = theTkTopo->module(geomDetUnit->geographicalId());
  LogTrace("VectorHitBuilder") << "Layer:" << layer << " and DetId: " << geomDetUnit->geographicalId().rawId() << std::endl;
  if(topol.ncolumns() == 32) 
    LogTrace("VectorHitBuilder") << "Pixel cluster (module:" << module << ") " << std::endl;
  else if(topol.ncolumns() == 2 ) 
    LogTrace("VectorHitBuilder") << "Strip cluster (module:" << module << ") " << std::endl;
  else LogTrace("VectorHitBuilder") << "no module?!" << std::endl;
  LogTrace("VectorHitBuilder") << "with pitch:" << topol.pitch().first << " , " << topol.pitch().second << std::endl;
  LogTrace("VectorHitBuilder") << " and width:" << pixelGeomDetUnit->surface().bounds().width() << " , lenght:" << pixelGeomDetUnit->surface().bounds().length() << std::endl;


  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpClu(clustIt->center(), clustIt->column() + 0.5);
  Local3DPoint localPosClu = pixelGeomDetUnit->topology().localPosition(mpClu);
  Global3DPoint globalPosClu = pixelGeomDetUnit->surface().toGlobal(localPosClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError localErrClu = pixelGeomDetUnit->topology().localError(mpClu,meClu);

  LogTrace("VectorHitBuilder") << "\t global pos " << globalPosClu << std::endl;
  LogTrace("VectorHitBuilder") << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;
  LogTrace("VectorHitBuilder") << std::endl;

  return;
}

void VectorHitBuilderAlgorithmBase::loadDetSetVector( std::map< DetId,std::vector<VectorHit> >& theMap, edmNew::DetSetVector<VectorHit>& theCollection ) const{

  std::map<DetId,std::vector<VectorHit> >::const_iterator it = theMap.begin();
  std::map<DetId,std::vector<VectorHit> >::const_iterator lastDet = theMap.end();
  for( ; it != lastDet ; ++it ) {
    edmNew::DetSetVector<VectorHit>::FastFiller vh_col(theCollection, it->first);
    std::vector<VectorHit>::const_iterator vh_it = it->second.begin();
    std::vector<VectorHit>::const_iterator vh_end = it->second.end();
    for( ; vh_it != vh_end ; ++vh_it)  {
      vh_col.push_back(*vh_it);
    }
  }

}
