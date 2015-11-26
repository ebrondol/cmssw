#include "RecoLocalTracker/SiPixelStubBuilder/interface/SiPixelStubBuilderAlgorithmBase.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkStripCPERecord.h"

#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

SiPixelStubBuilderAlgorithmBase::SiPixelStubBuilderAlgorithmBase(const edm::ParameterSet& conf) :
  nMaxVHforeachStack(conf.getParameter<int>("maxStubsinaStack")),
  cpeTag(conf.getParameter<edm::ESInputTag>("CPE"))
{}

void SiPixelStubBuilderAlgorithmBase::initialize(const edm::EventSetup& es)
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

void SiPixelStubBuilderAlgorithmBase::printClusters(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters){

  int nCluster = 0;
  int numberOfDSV = 0;
  edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter;
  for( DSViter = clusters.begin() ; DSViter != clusters.end(); DSViter++){

    ++numberOfDSV;

    // Loop over the clusters in the detector unit
    for (edmNew::DetSet< Phase2TrackerCluster1D >::const_iterator clustIt = DSViter->begin(); clustIt != DSViter->end(); ++clustIt) {

      nCluster++;
      // get the detector unit's id
      unsigned int rawId(DSViter->detId());

      printCluster(rawId, clustIt);

    }
  }
  LogDebug("SiPixelStubBuilder") << " Number of input clusters: " << nCluster << std::endl;

}


void SiPixelStubBuilderAlgorithmBase::printCluster(unsigned int rawId, const Phase2TrackerCluster1D* clustIt){

  DetId detId(rawId);

  // get the geom of the tracker
  const GeomDetUnit* geomDetUnit(theTkGeom->idToDetUnit(detId));
  const PixelGeomDetUnit* theGeomDet = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
  const PixelTopology& topol = theGeomDet->specificTopology();

  unsigned int layer = theTkTopo->layer(detId);
  unsigned int module = theTkTopo->module(detId);
  LogDebug("SiPixelStubBuilder") << "Layer:" << layer << std::endl;
  if(topol.ncolumns() == 32) 
    LogDebug("SiPixelStubBuilder") << "Pixel cluster with detId:" << rawId << "(module:" << module << ") " << std::endl;
  else if(topol.ncolumns() == 2 ) 
    LogDebug("SiPixelStubBuilder") << "Strip cluster with detId " << rawId << "(module:" << module << ") " << std::endl;
  else LogDebug("SiPixelStubBuilder") << "no module?!" << std::endl;
  LogDebug("SiPixelStubBuilder") << "with pitch:" << topol.pitch().first << " , " << topol.pitch().second << std::endl;
  LogDebug("SiPixelStubBuilder") << " and width:" << theGeomDet->surface().bounds().width() << " , lenght:" << theGeomDet->surface().bounds().length() << std::endl;

  if (!geomDetUnit) return;

  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpClu(clustIt->center(), clustIt->column() + 0.5);
  Local3DPoint localPosClu = geomDetUnit->topology().localPosition(mpClu);
  Global3DPoint globalPosClu = geomDetUnit->surface().toGlobal(localPosClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError localErrClu = geomDetUnit->topology().localError(mpClu,meClu);

  LogDebug("SiPixelStubBuilder") << "\t global pos " << globalPosClu << std::endl;
  LogDebug("SiPixelStubBuilder") << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;
  LogDebug("SiPixelStubBuilder") << std::endl;

  return;
}

void SiPixelStubBuilderAlgorithmBase::loadDetSetVector( std::map< DetId,std::vector<VectorHit> >& theMap, edmNew::DetSetVector<VectorHit>& theCollection ) const{

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
