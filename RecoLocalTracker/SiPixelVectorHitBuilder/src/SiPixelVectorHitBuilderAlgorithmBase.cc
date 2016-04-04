#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/SiPixelVectorHitBuilderAlgorithmBase.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkStripCPERecord.h"

#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

SiPixelVectorHitBuilderAlgorithmBase::SiPixelVectorHitBuilderAlgorithmBase(const edm::ParameterSet& conf) :
  nMaxVHforeachStack(conf.getParameter<int>("maxVectorHitsinaStack")),
  cpeTag(conf.getParameter<edm::ESInputTag>("CPE"))
{}

void SiPixelVectorHitBuilderAlgorithmBase::initialize(const edm::EventSetup& es)
{
  std::cout << "sono entrato proprio dove volevo" << std::endl;
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
  es.get< IdealGeometryRecord >().get(tTopoHandle);
  initTkTopo(tTopoHandle);

  es.get< TkStripCPERecord >().get(cpeTag, parameterestimator);
}

void SiPixelVectorHitBuilderAlgorithmBase::initTkGeom(edm::ESHandle< TrackerGeometry > tkGeomHandle){
  theTkGeom = tkGeomHandle.product();
}
void SiPixelVectorHitBuilderAlgorithmBase::initTkTopo(edm::ESHandle< TrackerTopology > tkTopoHandle){
  theTkTopo = tkTopoHandle.product();
}

void SiPixelVectorHitBuilderAlgorithmBase::printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters){

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
  //LogDebug("SiPixelVectorHitBuilder") << " Number of input clusters: " << nCluster << std::endl;
  std::cout << " Number of input clusters: " << nCluster << std::endl;

}


void SiPixelVectorHitBuilderAlgorithmBase::printCluster(const GeomDetUnit* geomDetUnit, const Phase2TrackerCluster1D* clustIt){

  if (!geomDetUnit) return;

  const PixelGeomDetUnit* pixelGeomDetUnit = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
  const PixelTopology& topol = pixelGeomDetUnit->specificTopology();
  if (!pixelGeomDetUnit) return;

  unsigned int layer = theTkTopo->layer(geomDetUnit->geographicalId());
  unsigned int module = theTkTopo->module(geomDetUnit->geographicalId());
  LogTrace("SiPixelVectorHitBuilder") << "Layer:" << layer << " and DetId: " << geomDetUnit->geographicalId().rawId() << std::endl;
  if(topol.ncolumns() == 32) 
    LogTrace("SiPixelVectorHitBuilder") << "Pixel cluster (module:" << module << ") " << std::endl;
  else if(topol.ncolumns() == 2 ) 
    LogTrace("SiPixelVectorHitBuilder") << "Strip cluster (module:" << module << ") " << std::endl;
  else LogTrace("SiPixelVectorHitBuilder") << "no module?!" << std::endl;
  LogTrace("SiPixelVectorHitBuilder") << "with pitch:" << topol.pitch().first << " , " << topol.pitch().second << std::endl;
  LogTrace("SiPixelVectorHitBuilder") << " and width:" << pixelGeomDetUnit->surface().bounds().width() << " , lenght:" << pixelGeomDetUnit->surface().bounds().length() << std::endl;


  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpClu(clustIt->center(), clustIt->column() + 0.5);
  Local3DPoint localPosClu = pixelGeomDetUnit->topology().localPosition(mpClu);
  Global3DPoint globalPosClu = pixelGeomDetUnit->surface().toGlobal(localPosClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError localErrClu = pixelGeomDetUnit->topology().localError(mpClu,meClu);

  LogTrace("SiPixelVectorHitBuilder") << "\t global pos " << globalPosClu << std::endl;
  LogTrace("SiPixelVectorHitBuilder") << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;
  LogTrace("SiPixelVectorHitBuilder") << std::endl;

  return;
}

void SiPixelVectorHitBuilderAlgorithmBase::loadDetSetVector( std::map< DetId,std::vector<VectorHit> >& theMap, edmNew::DetSetVector<VectorHit>& theCollection ) const{

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
