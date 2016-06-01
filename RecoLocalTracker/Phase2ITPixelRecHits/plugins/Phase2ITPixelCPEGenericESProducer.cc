#include "RecoLocalTracker/Phase2ITPixelRecHits/interface/Phase2PixelCPEGenericESProducer.h"
#include "RecoLocalTracker/Phase2ITPixelRecHits/interface/Phase2PixelCPEGeneric.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include <string>
#include <memory>

#include "FWCore/Framework/interface/MakerMacros.h"

using namespace edm;

Phase2PixelCPEGenericESProducer::Phase2PixelCPEGenericESProducer(const edm::ParameterSet & p) 
{
  std::string myname = p.getParameter<std::string>("ComponentName");
  // Use LA-width from DB. If both (upper and this) are false LA-width is calcuated from LA-offset
  useLAWidthFromDB_ = p.existsAs<bool>("useLAWidthFromDB")?
    p.getParameter<bool>("useLAWidthFromDB"):false;
  // Use Alignment LA-offset 
  useLAAlignmentOffsets_ = p.existsAs<bool>("useLAAlignmentOffsets")?
    p.getParameter<bool>("useLAAlignmentOffsets"):false;
  magname_ = p.existsAs<edm::ESInputTag>("MagneticFieldRecord")?
    p.getParameter<edm::ESInputTag>("MagneticFieldRecord"):edm::ESInputTag("");

  pset_ = p;
  setWhatProduced(this,myname);

  //std::cout<<" ESProducer "<<myname<<" "<<useLAWidthFromDB_<<" "<<useLAAlignmentOffsets_<<" "
  //	   <<UseErrorsFromTemplates_<<std::endl; //dk

}

Phase2PixelCPEGenericESProducer::~Phase2PixelCPEGenericESProducer() {}

std::shared_ptr<Phase2PixelClusterParameterEstimator>
Phase2PixelCPEGenericESProducer::produce(const TkPixelCPERecord & iRecord){ 

  ESHandle<MagneticField> magfield;
  iRecord.getRecord<IdealMagneticFieldRecord>().get( magname_, magfield );

  edm::ESHandle<TrackerGeometry> pDD;
  iRecord.getRecord<TrackerDigiGeometryRecord>().get( pDD );

  edm::ESHandle<TrackerTopology> hTT;
  iRecord.getRecord<TrackerDigiGeometryRecord>().getRecord<TrackerTopologyRcd>().get(hTT);

  // Lorant angle for offsets
  ESHandle<SiPixelLorentzAngle> lorentzAngle;
  if(useLAAlignmentOffsets_) // LA offsets from alignment 
    iRecord.getRecord<SiPixelLorentzAngleRcd>().get("fromAlignment",lorentzAngle );
  else // standard LA, from calibration, label=""
    iRecord.getRecord<SiPixelLorentzAngleRcd>().get(lorentzAngle );

  // add the new la width object
  ESHandle<SiPixelLorentzAngle> lorentzAngleWidth;
  const SiPixelLorentzAngle * lorentzAngleWidthProduct = 0;
  if(useLAWidthFromDB_) { // use the width LA
    iRecord.getRecord<SiPixelLorentzAngleRcd>().get("forWidth",lorentzAngleWidth );
    lorentzAngleWidthProduct = lorentzAngleWidth.product();
  } else { lorentzAngleWidthProduct = NULL;} // do not use it
  //std::cout<<" la width "<<lorentzAngleWidthProduct<<std::endl; //dk


  cpe_  = std::make_shared<Phase2PixelCPEGeneric>(
                         pset_,magfield.product(),*pDD.product(),
			 *hTT.product(),lorentzAngle.product(),
			 lorentzAngleWidthProduct);

  return cpe_;
}

DEFINE_FWK_EVENTSETUP_MODULE(Phase2PixelCPEGenericESProducer);
