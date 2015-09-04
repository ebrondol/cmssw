#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerPixelPhase2EndcapBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerPhase1DiskBuilder.h"  
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLayerBuilder.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTDiscBuilder.h"  
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>

#include <bitset>

CmsTrackerPixelPhase2EndcapBuilder::CmsTrackerPixelPhase2EndcapBuilder()
{}

void
CmsTrackerPixelPhase2EndcapBuilder::buildComponent( DDFilteredView& fv, GeometricDet* g, std::string s )
{
  CmsTrackerPhase1DiskBuilder  theCmsTrackerPhase1DiskBuilder;   
  CmsTrackerLayerBuilder       theCmsTrackerLayerBuilder;
  CmsTrackerOTDiscBuilder      theCmsTrackerOTDiscBuilder;   

  GeometricDet * subdet = new GeometricDet( &fv, theCmsTrackerStringToEnum.type( ExtractStringFromDDD::getString( s, &fv )));
  std::string subdet_name = subdet->name();
  switch( theCmsTrackerStringToEnum.type( ExtractStringFromDDD::getString( s, &fv )))
  {
  case GeometricDet::PixelPhase2FullDisk:    
    theCmsTrackerPhase1DiskBuilder.build(fv,subdet,s);
    break;
  case GeometricDet::PixelPhase2ReducedDisk:    
    theCmsTrackerPhase1DiskBuilder.build(fv,subdet,s);
    break;
  case GeometricDet::OTPhase2Layer:
    theCmsTrackerLayerBuilder.build(fv,subdet,s);      
    break;
  case GeometricDet::OTPhase2Wheel:    
    theCmsTrackerOTDiscBuilder.build(fv,subdet,s);
    break;

  default:
    edm::LogError("CmsTrackerPixelPhase2EndcapBuilder")<<" ERROR - I was expecting a Disk... I got a "<<ExtractStringFromDDD::getString(s,&fv);
  }  
  
  g->addComponent(subdet);

}

void
CmsTrackerPixelPhase2EndcapBuilder::sortNS( DDFilteredView& fv, GeometricDet* det )
{
  GeometricDet::GeometricDetContainer & comp = det->components();

  switch( comp.front()->type() )
  {
  case GeometricDet::PixelPhase2FullDisk:
    std::sort( comp.begin(), comp.end(), LessModZ());
    break;
  case GeometricDet::PixelPhase2ReducedDisk:    
    std::sort( comp.begin(), comp.end(), LessModZ());
    break;
  case GeometricDet::OTPhase2Layer:
    std::sort( comp.begin(), comp.end(), LessR());
    break;
  case GeometricDet::OTPhase2Wheel:    
    std::sort( comp.begin(), comp.end(), LessModZ());
    break;

  default:
    edm::LogError( "CmsTrackerSubStrctBuilder" ) << "ERROR - wrong SubDet to sort..... " << det->components().front()->type();
  }

  for( uint32_t i = 0; i < comp.size(); i++ )
  {
    comp[i]->setGeographicalID(i+1); // Every subdetector: Layer/Disk/Wheel Number
  }

}

