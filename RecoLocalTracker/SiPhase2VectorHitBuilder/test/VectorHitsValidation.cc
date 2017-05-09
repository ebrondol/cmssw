#include "RecoLocalTracker/SiPhase2VectorHitBuilder/test/VectorHitsValidation.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

VectorHitsBuilderValidation::VectorHitsBuilderValidation(const edm::ParameterSet& conf) {
//    srcClu_(conf.getParameter< edm::InputTag >("src")),
//    srcVH_(conf.getParameter< edm::InputTag >("src2")),
//    links_(conf.getParameiter< edm::InputTag >("links")) { 
  srcClu_ = consumes< edmNew::DetSetVector<Phase2TrackerCluster1D> >(edm::InputTag(conf.getParameter<std::string>("src")));
  srcVH_ = consumes< VectorHitCollectionNew >(edm::InputTag(conf.getParameter<edm::InputTag>("src2")));
  siphase2OTSimLinksToken_ = consumes<edm::DetSetVector<PixelDigiSimLink> >(conf.getParameter<edm::InputTag>("links"));
  simHitsToken_ = consumes< edm::PSimHitContainer >(edm::InputTag("g4SimHits", "TrackerHitsPixelBarrelLowTof"));
  simTracksToken_ = consumes< edm::SimTrackContainer >(edm::InputTag("g4SimHits"));
  simVerticesToken_ = consumes< edm::SimVertexContainer >(edm::InputTag("g4SimHits")); 
}

VectorHitsBuilderValidation::~VectorHitsBuilderValidation() { 
}

void VectorHitsBuilderValidation::beginJob() {

  edm::Service<TFileService> fs;
  fs->file().cd("/");
  TFileDirectory td = fs->mkdir("Common");

  //Create common ntuple
  tree = td.make< TTree >("VectorHits","VectorHits");

  // Create common graphs
  TFileDirectory tdGloPos = td.mkdir("GlobalPositions");
  trackerLayoutRZ_[0] = tdGloPos.make< TGraph >();
  trackerLayoutRZ_[0] -> SetName("RVsZ_Mixed");
  trackerLayoutRZ_[1] = tdGloPos.make< TGraph >();
  trackerLayoutRZ_[1] -> SetName("RVsZ_Pixel");
  trackerLayoutRZ_[2] = tdGloPos.make< TGraph >();
  trackerLayoutRZ_[2] -> SetName("RVsZ_Strip");
  trackerLayoutXY_[0] = tdGloPos.make< TGraph >();
  trackerLayoutXY_[0] -> SetName("YVsX_Mixed");
  trackerLayoutXY_[1] = tdGloPos.make< TGraph >();
  trackerLayoutXY_[1] -> SetName("YVsX_Pixel");
  trackerLayoutXY_[2] = tdGloPos.make< TGraph >();
  trackerLayoutXY_[2] -> SetName("YVsX_Strip");
  trackerLayoutXYBar_ = tdGloPos.make< TGraph >();
  trackerLayoutXYBar_ -> SetName("YVsXBar");
  trackerLayoutXYEC_ = tdGloPos.make< TGraph >();
  trackerLayoutXYEC_ -> SetName("YVsXEC");

  TFileDirectory tdLocPos = td.mkdir("LocalPositions");
  localPosXvsDeltaX_[0] = tdLocPos.make< TGraph >();
  localPosXvsDeltaX_[0] -> SetName("localPosXvsDeltaX_Mixed");
  localPosXvsDeltaX_[1] = tdLocPos.make< TGraph >();
  localPosXvsDeltaX_[1] -> SetName("localPosXvsDeltaX_Pixel");
  localPosXvsDeltaX_[2] = tdLocPos.make< TGraph >();
  localPosXvsDeltaX_[2] -> SetName("localPosXvsDeltaX_Strip");
  localPosYvsDeltaY_[0] = tdLocPos.make< TGraph >();
  localPosYvsDeltaY_[0] -> SetName("localPosYvsDeltaY_Mixed");
  localPosYvsDeltaY_[1] = tdLocPos.make< TGraph >();
  localPosYvsDeltaY_[1] -> SetName("localPosYvsDeltaY_Pixel");
  localPosYvsDeltaY_[2] = tdLocPos.make< TGraph >();
  localPosYvsDeltaY_[2] -> SetName("localPosYvsDeltaY_Strip");

  //drawing VHs arrows
  TFileDirectory tdArr = td.mkdir("Directions");
  VHRZ_ = tdArr.make< TCanvas >(); VHRZ_ -> SetName("RVsZ_Mixed");
  VHXY_ = tdArr.make< TCanvas >(); VHXY_ -> SetName("YVsX_Mixed");

  //VHXY_[0] = tdArr.make< TCanvas >(); VHXY_[0] -> SetName("YVsX_Mixed");
  //VHXY_[1] = tdArr.make< TCanvas >(); VHXY_[1] -> SetName("YVsX_Pixel");
  //VHXY_[2] = tdArr.make< TCanvas >(); VHXY_[2] -> SetName("YVsX_Strip");


}

void VectorHitsBuilderValidation::endJob() {}

void VectorHitsBuilderValidation::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) {

  // Get the needed objects

  // Get the clusters
  edm::Handle< Phase2TrackerCluster1DCollectionNew > clusters;
  event.getByToken(srcClu_, clusters);

  // Get the vector hits
  edm::Handle< VectorHitCollectionNew > vhs;
  event.getByToken(srcVH_, vhs);

  // Get the Phase2 DigiSimLink
  edm::Handle<edm::DetSetVector<PixelDigiSimLink> > siphase2SimLinks;
  event.getByToken(siphase2OTSimLinksToken_, siphase2SimLinks);

  // Get the SimHits
  edm::Handle< edm::PSimHitContainer > simHitsRaw;
  event.getByToken(simHitsToken_, simHitsRaw);
//  edm::Handle< edm::PSimHitContainer > simHitsRawEndcap;
//  event.getByLabel("g4SimHits", "TrackerHitsPixelEndcapLowTof", simHitsRawEndcap);

  // Get the SimTracks
  edm::Handle< edm::SimTrackContainer > simTracksRaw;
  event.getByToken(simTracksToken_, simTracksRaw);

  // Get the SimVertex
  edm::Handle< edm::SimVertexContainer > simVertices;
  event.getByToken(simVerticesToken_, simVertices);

  // Get the geometry
  edm::ESHandle< TrackerGeometry > geomHandle;
  eventSetup.get< TrackerDigiGeometryRecord >().get(geomHandle);
  tkGeom = &(*geomHandle);
  edm::ESHandle< TrackerTopology > tTopoHandle;
  eventSetup.get< TrackerTopologyRcd >().get(tTopoHandle);
  tkTopo = tTopoHandle.product();

  edm::ESHandle< MagneticField > magFieldHandle;
  eventSetup.get< IdealMagneticFieldRecord >().get(magFieldHandle);
  magField = magFieldHandle.product();

  //set up for tree
  int eventNum;
  int layer;
  int module_id;
  int module_number;
  int module_type; //1: pixel, 2: strip
  float x_global, y_global, z_global;
  float vh_x_local, vh_y_local;
  float vh_x_le, vh_y_le;
  float curvature, phi;
  float QOverPT, QOverP;
  int sim_track_id;
  float sim_x_local, sim_y_local;
  float sim_x_global, sim_y_global, sim_z_global;
  float low_x_global, low_y_global, low_z_global;
  float upp_x_global, upp_y_global, upp_z_global;
  float low_xx_global_err, low_yy_global_err, low_zz_global_err;
  float low_xy_global_err, low_zx_global_err, low_zy_global_err;
  float upp_xx_global_err, upp_yy_global_err, upp_zz_global_err;
  float upp_xy_global_err, upp_zx_global_err, upp_zy_global_err;
  float deltaXVHSimHits, deltaYVHSimHits;
  unsigned int processType(99);

  tree -> Branch("event",&eventNum,"eventNum/I");
  tree -> Branch("layer",&layer,"layer/I");
  tree -> Branch("module_id",&module_id,"module_id/I");
  tree -> Branch("module_type",&module_type,"module_type/I");
  tree -> Branch("module_number",&module_number,"module_number/I");
  tree -> Branch("x_global",&x_global,"x_global/F");
  tree -> Branch("y_global",&y_global,"y_global/F");
  tree -> Branch("z_global",&z_global,"z_global/F");
  tree -> Branch("vh_x_local",&vh_x_local,"vh_x_local/F");
  tree -> Branch("vh_y_local",&vh_y_local,"vh_y_local/F");
  tree -> Branch("vh_x_lError",&vh_x_le,"vh_x_le/F");
  tree -> Branch("vh_y_lError",&vh_y_le,"vh_y_le/F");
  tree -> Branch("curvature",&curvature,"curvature/F");
  tree -> Branch("phi",&phi,"phi/F");
  tree -> Branch("QOverP",&QOverP,"QOverP/F");
  tree -> Branch("QOverPT",&QOverPT,"QOverPT/F");
  tree -> Branch("sim_track_id",&sim_track_id,"sim_track_id/I");
  tree -> Branch("sim_x_local",&sim_x_local,"sim_x_local/F");
  tree -> Branch("sim_y_local",&sim_y_local,"sim_y_local/F");
  tree -> Branch("sim_x_global",&sim_x_global,"sim_x_global/F");
  tree -> Branch("sim_y_global",&sim_y_global,"sim_y_global/F");
  tree -> Branch("sim_z_global",&sim_z_global,"sim_z_global/F");
  tree -> Branch("low_x_global",&low_x_global,"low_x_global/F");
  tree -> Branch("low_y_global",&low_y_global,"low_y_global/F");
  tree -> Branch("low_z_global",&low_z_global,"low_z_global/F");
  tree -> Branch("low_xx_global_err",&low_xx_global_err,"low_xx_global_err/F");
  tree -> Branch("low_yy_global_err",&low_yy_global_err,"low_yy_global_err/F");
  tree -> Branch("low_zz_global_err",&low_zz_global_err,"low_zz_global_err/F");
  tree -> Branch("low_xy_global_err",&low_xy_global_err,"low_xy_global_err/F");
  tree -> Branch("low_zx_global_err",&low_zx_global_err,"low_zx_global_err/F");
  tree -> Branch("low_zy_global_err",&low_zy_global_err,"low_zy_global_err/F");
  tree -> Branch("upp_x_global",&upp_x_global,"upp_x_global/F");
  tree -> Branch("upp_y_global",&upp_y_global,"upp_y_global/F");
  tree -> Branch("upp_z_global",&upp_z_global,"upp_z_global/F");
  tree -> Branch("upp_xx_global_err",&upp_xx_global_err,"upp_xx_global_err/F");
  tree -> Branch("upp_yy_global_err",&upp_yy_global_err,"upp_yy_global_err/F");
  tree -> Branch("upp_zz_global_err",&upp_zz_global_err,"upp_zz_global_err/F");
  tree -> Branch("upp_xy_global_err",&upp_xy_global_err,"upp_xy_global_err/F");
  tree -> Branch("upp_zx_global_err",&upp_zx_global_err,"upp_zx_global_err/F");
  tree -> Branch("upp_zy_global_err",&upp_zy_global_err,"upp_zy_global_err/F");
  tree -> Branch("deltaXVHSimHits",&deltaXVHSimHits,"deltaXVHSimHits/F");
  tree -> Branch("deltaYVHSimHits",&deltaYVHSimHits,"deltaYVHSimHits/F");
  tree -> Branch("processType",&processType,"processType/i");

  // Rearrange the simTracks for ease of use <simTrackID, simTrack>
  SimTracksMap simTracks;
  for (edm::SimTrackContainer::const_iterator simTrackIt(simTracksRaw->begin()); simTrackIt != simTracksRaw->end(); ++simTrackIt) simTracks.insert(std::pair< unsigned int, SimTrack >(simTrackIt->trackId(), *simTrackIt));

  // Collect barrel and endcap SimHits in one vector
//  edm::PSimHitContainer simHitsRaw;
//  simHitsRaw.reserve( simHitsRawBarrel->size() + simHitsRawEndcap->size() ); // preallocate memory
//  simHitsRaw.insert( simHitsRaw.end(), simHitsRawBarrel->begin(), simHitsRawBarrel->end() );
//  simHitsRaw.insert( simHitsRaw.end(), simHitsRawEndcap->begin(), simHitsRawEndcap->end() );

  // Rearrange the simHits by detUnit for ease of use
  SimHitsMap simHitsDetUnit;
  SimHitsMap simHitsTrackId;
  for (edm::PSimHitContainer::const_iterator simHitIt(simHitsRaw->begin()); simHitIt != simHitsRaw->end(); ++simHitIt) {
    SimHitsMap::iterator simHitsDetUnitIt(simHitsDetUnit.find(simHitIt->detUnitId()));
    if (simHitsDetUnitIt == simHitsDetUnit.end()) {
        std::pair< SimHitsMap::iterator, bool > newIt(simHitsDetUnit.insert(std::pair< unsigned int, std::vector< PSimHit > >(simHitIt->detUnitId(), std::vector< PSimHit >())));
        simHitsDetUnitIt = newIt.first;
    }
    simHitsDetUnitIt->second.push_back(*simHitIt);

    SimHitsMap::iterator simHitsTrackIdIt(simHitsTrackId.find(simHitIt->trackId()));
    if (simHitsTrackIdIt == simHitsTrackId.end()) {
        std::pair< SimHitsMap::iterator, bool > newIt(simHitsTrackId.insert(std::pair< unsigned int, std::vector< PSimHit > >(simHitIt->trackId(), std::vector< PSimHit >())));
        simHitsTrackIdIt = newIt.first;
    }
    simHitsTrackIdIt->second.push_back(*simHitIt);
  }


  // Validation
  eventNum = event.id().event();

  unsigned int nVHsTot(0), nVHsPSTot(0), nVHs2STot(0);
  std::vector<Global3DPoint> glVHs;
  std::vector<Global3DVector> dirVHs;

  // Loop over modules
  for (VectorHitCollectionNew::const_iterator DSViter = vhs->begin(); DSViter != vhs->end(); ++DSViter) {

    // Get the detector unit's id
    unsigned int rawid(DSViter->detId());
    module_id = rawid;
    DetId detId(rawid);

    module_number = getModuleNumber(detId);
    layer = getLayerNumber(detId);

    LogDebug("VectorHitsBuilderValidation") << "Layer: " << layer << "  det id" << rawid << std::endl;

    // Get the geometry of the tracker
    const GeomDet* geomDet(tkGeom->idToDet(detId));
    if (!geomDet) break;

    // Create histograms for the layer if they do not yet exist
    std::map< unsigned int, VHHistos >::iterator histogramLayer(histograms_.find(layer));
    if (histogramLayer == histograms_.end()) histogramLayer = createLayerHistograms(layer);
    // Number of clusters
    unsigned int nVHsPS(0), nVHs2S(0);

    // Loop over the vhs in the detector unit
    for (edmNew::DetSet< VectorHit >::const_iterator vhIt = DSViter->begin(); vhIt != DSViter->end(); ++vhIt) {

      // vh variables
      if( vhIt->isValid() ){

         LogDebug("VectorHitsBuilderValidation") << " vh analyzing ..." ;
         Local3DPoint localPosVH = vhIt->localPosition();
         vh_x_local = localPosVH.x();
         vh_y_local = localPosVH.y();
         LogTrace("VectorHitsBuilderValidation") << "local VH position " << localPosVH << std::endl;

         LocalError localErrVH = vhIt->localPositionError();
         vh_x_le = localErrVH.xx();
         vh_y_le = localErrVH.yy();
         LogTrace("VectorHitsBuilderValidation") << "local VH error " << localErrVH << std::endl;

         Global3DPoint globalPosVH = geomDet->surface().toGlobal(localPosVH);
         x_global = globalPosVH.x();
         y_global = globalPosVH.y();
         z_global = globalPosVH.z();
         glVHs.push_back(globalPosVH);
         LogTrace("VectorHitsBuilderValidation") << " global VH position " << globalPosVH << std::endl;

         Local3DVector localDirVH = vhIt->localDirection();
         LogTrace("VectorHitsBuilderValidation") << "local VH direction " << localDirVH << std::endl;

         VectorHit vh = *vhIt;
         Global3DVector globalDirVH = vh.globalDelta();
         dirVHs.push_back(globalDirVH);
         LogTrace("VectorHitsBuilderValidation") << "global VH direction " << globalDirVH << std::endl;

         curvature = vh.curvatureORphi("curvature").first;
         phi = vh.curvatureORphi("phi").first;
         QOverPT = vh.transverseMomentum(magField);
         QOverP = vh.momentum(magField);

         // Fill the position histograms
         trackerLayoutRZ_[0]->SetPoint(nVHsTot, globalPosVH.z(), globalPosVH.perp());
         trackerLayoutXY_[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

         if (layer < 100) trackerLayoutXYBar_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());
         else trackerLayoutXYEC_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

         histogramLayer->second.localPosXY[0]->SetPoint(nVHsTot, vh_x_local, vh_y_local);
         histogramLayer->second.globalPosXY[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

         localPosXvsDeltaX_[0]->SetPoint(nVHsTot, vh_x_local, localDirVH.x());
         localPosYvsDeltaY_[0]->SetPoint(nVHsTot, vh_y_local, localDirVH.y());

         // Pixel module
         const StackGeomDet* stackDet = dynamic_cast<const StackGeomDet*>(geomDet);
         const PixelGeomDetUnit* geomDetLower = dynamic_cast< const PixelGeomDetUnit* >(stackDet->lowerDet());

         TrackerGeometry::ModuleType mType = tkGeom->getDetectorType(stackDet->lowerDet()->geographicalId());
         module_type = 0;
         if (mType == TrackerGeometry::ModuleType::Ph2PSP) {
           module_type = 1;
           trackerLayoutRZ_[1]->SetPoint(nVHsPSTot, globalPosVH.z(), globalPosVH.perp());
           trackerLayoutXY_[1]->SetPoint(nVHsPSTot, globalPosVH.x(), globalPosVH.y());

           histogramLayer->second.localPosXY[1]->SetPoint(nVHsPSTot, vh_x_local, vh_y_local);
           histogramLayer->second.globalPosXY[1]->SetPoint(nVHsPSTot, globalPosVH.x(), globalPosVH.y());

           localPosXvsDeltaX_[1]->SetPoint(nVHsPSTot, vh_x_local, localDirVH.x());
           localPosYvsDeltaY_[1]->SetPoint(nVHsPSTot, vh_y_local, localDirVH.y());

           ++nVHsPS;
           ++nVHsPSTot;
         }

         // Strip module
         else if (mType == TrackerGeometry::ModuleType::Ph2SS) {
           module_type = 2;
           trackerLayoutRZ_[2]->SetPoint(nVHs2STot, globalPosVH.z(), globalPosVH.perp());
           trackerLayoutXY_[2]->SetPoint(nVHs2STot, globalPosVH.x(), globalPosVH.y());

           histogramLayer->second.localPosXY[2]->SetPoint(nVHs2STot, vh_x_local, vh_y_local);
           histogramLayer->second.globalPosXY[2]->SetPoint(nVHs2STot, globalPosVH.x(), globalPosVH.y());

           localPosXvsDeltaX_[2]->SetPoint(nVHs2STot, vh_x_local, localDirVH.x());
           localPosYvsDeltaY_[2]->SetPoint(nVHs2STot, vh_y_local, localDirVH.y());

           ++nVHs2S;
           ++nVHs2STot;
         } else if (mType == TrackerGeometry::ModuleType::Ph2PSS) {
           edm::LogError("VectorHitsBuilderValidation") << "module type " << module_type << " should never happen!";
         }
         LogTrace("VectorHitsBuilderValidation") << "module type " << module_type << std::endl;

         // get the geomDetUnit of the clusters
         low_x_global = vhIt->lowerGlobalPos().x();
         low_y_global = vhIt->lowerGlobalPos().y();
         low_z_global = vhIt->lowerGlobalPos().z();
         upp_x_global = vhIt->upperGlobalPos().x();
         upp_y_global = vhIt->upperGlobalPos().y();
         upp_z_global = vhIt->upperGlobalPos().z();

         low_xx_global_err = vhIt->lowerGlobalPosErr().cxx();
         low_yy_global_err = vhIt->lowerGlobalPosErr().cyy();
         low_zz_global_err = vhIt->lowerGlobalPosErr().czz();
         low_xy_global_err = vhIt->lowerGlobalPosErr().cyx();
         low_zx_global_err = vhIt->lowerGlobalPosErr().czx();
         low_zy_global_err = vhIt->lowerGlobalPosErr().czy();

         upp_xx_global_err = vhIt->upperGlobalPosErr().cxx();
         upp_yy_global_err = vhIt->upperGlobalPosErr().cyy();
         upp_zz_global_err = vhIt->upperGlobalPosErr().czz();
         upp_xy_global_err = vhIt->upperGlobalPosErr().cyx();
         upp_zx_global_err = vhIt->upperGlobalPosErr().czx();
         upp_zy_global_err = vhIt->upperGlobalPosErr().czy();

         LogDebug("VectorHitsBuilderValidation") << "print Clusters into the VH:" << std::endl;
         printCluster(geomDetLower,vhIt->lowerClusterRef());
         LogTrace("VectorHitsBuilderValidation") << "\t global pos lower " << vhIt->lowerGlobalPos() << std::endl;
         LogTrace("VectorHitsBuilderValidation") << "\t global posErr lower " << vhIt->lowerGlobalPosErr().cxx() << std::endl;
         const GeomDetUnit* geomDetUpper = stackDet->upperDet();
         printCluster(geomDetUpper,vhIt->upperClusterRef());
         LogTrace("VectorHitsBuilderValidation") << "\t global pos upper " << vhIt->upperGlobalPos() << std::endl;
         //LogTrace("VectorHitsBuilderValidation") << "\t global posErr upper " << vhIt->upperGlobalPosErr() << std::endl;

         //comparison with SIM hits
         LogDebug("VectorHitsBuilderValidation") << "comparison Clusters with sim hits ... " << std::endl;
         std::vector< unsigned int > clusterSimTrackIds;
         std::set<std::pair<uint32_t, EncodedEventId> > simTkIds;

         for (unsigned int istr(0); istr < (*(vhIt->lowerClusterRef().cluster_phase2OT())).size(); ++istr) {
           uint32_t channel = Phase2TrackerDigi::pixelToChannel((*(vhIt->lowerClusterRef().cluster_phase2OT())).firstRow() + istr, (*(vhIt->lowerClusterRef().cluster_phase2OT())).column());
           DetId detIdCluster = geomDetLower->geographicalId();
           unsigned int LowerSimTrackId(getSimTrackId(siphase2SimLinks, detIdCluster, channel));
           std::vector<std::pair<uint32_t, EncodedEventId> > trkid(getSimTrackId(siphase2SimLinks, detIdCluster, channel));
           if (trkid.size()==0) continue;
           clusterSimTrackIds.push_back(LowerSimTrackId);
           simTkIds.insert(trkid.begin(),trkid.end());
           sim_track_id = LowerSimTrackId;
           LogTrace("VectorHitsBuilderValidation") << "LowerSimTrackId " << LowerSimTrackId << std::endl;
         }
/*
         for (unsigned int istr(0); istr < (*(vhIt->upperClusterRef().cluster_phase2OT())).size(); ++istr) {
           uint32_t channel = Phase2TrackerDigi::pixelToChannel((*(vhIt->upperClusterRef().cluster_phase2OT())).firstRow() + istr, (*(vhIt->upperClusterRef().cluster_phase2OT())).column());
           DetId detIdCluster = geomDetUpper->geographicalId();
           unsigned int UpperSimTrackId(getSimTrackId(siphase2SimLinks, detIdCluster, channel));
           std::vector<std::pair<uint32_t, EncodedEventId> > trkid(getSimTrackId(siphase2SimLinks, detIdCluster, channel));
           if (trkid.size()==0) continue;
           clusterSimTrackIds.push_back(UpperSimTrackId);
           simTkIds.insert(trkid.begin(),trkid.end());
         }
*/

           // loop over all simHits
           unsigned int totalSimHits(0);
           unsigned int primarySimHits(0);
           unsigned int otherSimHits(0);


           for (edm::PSimHitContainer::const_iterator hitIt(simHitsRaw->begin()); hitIt != simHitsRaw->end(); ++hitIt) {

             if(hitIt->detUnitId() == geomDetLower->geographicalId()){// || hitIt->detUnitId() == geomDetUpper->geographicalId()){

               //check clusters track id compatibility
               if(std::find(clusterSimTrackIds.begin(), clusterSimTrackIds.end(), hitIt->trackId()) != clusterSimTrackIds.end()){

                 Local3DPoint localPosHit(hitIt->localPosition());
                 sim_x_local = localPosHit.x();
                 sim_y_local = localPosHit.y();

                 deltaXVHSimHits = vh_x_local - sim_x_local;
                 deltaYVHSimHits = vh_y_local - sim_y_local;

                 Global3DPoint globalPosHit = geomDetLower->surface().toGlobal(localPosHit);
                 sim_x_global = globalPosHit.x();
                 sim_y_global = globalPosHit.y();
                 sim_z_global = globalPosHit.z();

                 histogramLayer->second.deltaXVHSimHits[0]->Fill(vh_x_local - sim_x_local);
                 histogramLayer->second.deltaYVHSimHits[0]->Fill(vh_y_local - sim_y_local);

                 // Pixel module
                if (layer == 1 || layer == 2 || layer == 3) {
                     histogramLayer->second.deltaXVHSimHits[1]->Fill(vh_x_local - sim_x_local);
                     histogramLayer->second.deltaYVHSimHits[1]->Fill(vh_y_local - sim_y_local);
                 }
                 // Strip module
                 else if (layer == 4 || layer == 5 || layer == 6) {
                     histogramLayer->second.deltaXVHSimHits[2]->Fill(vh_x_local - sim_x_local);
                     histogramLayer->second.deltaYVHSimHits[2]->Fill(vh_y_local - sim_y_local);
                 }

                 ++totalSimHits;

                 std::map< unsigned int, SimTrack >::const_iterator simTrackIt(simTracks.find(hitIt->trackId()));
                 if (simTrackIt == simTracks.end()) continue;

                 // Primary particles only
                 processType = hitIt->processType();

                 if (simTrackIt->second.vertIndex() == 0 and (processType == 2 || processType == 7 || processType == 9 || processType == 11 || processType == 13 || processType == 15)) {
                     histogramLayer->second.deltaXVHSimHits_P[0]->Fill(vh_x_local - sim_x_local);
                     histogramLayer->second.deltaYVHSimHits_P[0]->Fill(vh_y_local - sim_y_local);

                     // Pixel module
                     if (layer == 1 || layer == 2 || layer == 3) {
                         histogramLayer->second.deltaXVHSimHits_P[1]->Fill(vh_x_local - sim_x_local);
                         histogramLayer->second.deltaYVHSimHits_P[1]->Fill(vh_y_local - sim_y_local);
                     }
                     // Strip module
                     else if (layer == 4 || layer == 5 || layer == 6) {
                         histogramLayer->second.deltaXVHSimHits_P[2]->Fill(vh_x_local - sim_x_local);
                         histogramLayer->second.deltaYVHSimHits_P[2]->Fill(vh_y_local - sim_y_local);
                     }

                     ++primarySimHits;
                 }

                 otherSimHits = totalSimHits - primarySimHits;

                 histogramLayer->second.totalSimHits->Fill(totalSimHits);
                 histogramLayer->second.primarySimHits->Fill(primarySimHits);
                 histogramLayer->second.otherSimHits->Fill(otherSimHits);

               }
             }
           }// loop simhits


           nVHsTot++;
           tree->Fill();

        }// vh valid

      }// loop vhs

        if (nVHsPS) histogramLayer->second.numberVHsPS->Fill(nVHsPS);
        if (nVHs2S) histogramLayer->second.numberVHs2S->Fill(nVHs2S);
        LogTrace("VectorHitsBuilderValidation") << "nVHsPS for this layer : " << nVHsPS << ", nVHs2S for this layer : " << nVHs2S << std::endl;

    }

    CreateVHsXYGraph(glVHs, dirVHs);
    CreateVHsRZGraph(glVHs, dirVHs);

}

// Create the histograms
std::map< unsigned int, VHHistos >::iterator VectorHitsBuilderValidation::createLayerHistograms(unsigned int ival) {
    std::ostringstream fname1, fname2;

    edm::Service<TFileService> fs;
    fs->file().cd("/");

    std::string tag;
    unsigned int id;
    if (ival < 100) {
        id = ival;
        fname1 << "Barrel";
        fname2 << "Layer_" << id;
        tag = "_layer_";
    }
    else {
        int side = ival / 100;
        id = ival - side * 100;
        fname1 << "EndCap_Side_" << side;
        fname2 << "Disc_" << id;
        tag = "_disc_";
    }

    TFileDirectory td1 = fs->mkdir(fname1.str().c_str());
    TFileDirectory td = td1.mkdir(fname2.str().c_str());

    VHHistos local_histos;

    std::ostringstream histoName;

    /*
     * Number of clusters
     */

    histoName.str(""); histoName << "Number_VHs_PS" << tag.c_str() <<  id;
    local_histos.numberVHsPS = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberVHsPS->SetFillColor(kAzure + 7);

    histoName.str(""); histoName << "Number_VHs_2S" << tag.c_str() <<  id;
    local_histos.numberVHs2S = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberVHs2S->SetFillColor(kOrange - 3);

    histoName.str(""); histoName << "Number_VHs_Mixed" << tag.c_str() <<  id;
    local_histos.numberVHsMixed = td.make< THStack >(histoName.str().c_str(), histoName.str().c_str());
    local_histos.numberVHsMixed->Add(local_histos.numberVHsPS);
    local_histos.numberVHsMixed->Add(local_histos.numberVHs2S);

      /*
     * Local and Global positions
     */

     histoName.str(""); histoName << "Local_Position_XY_Mixed" << tag.c_str() <<  id;
     local_histos.localPosXY[0] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[0]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Local_Position_XY_PS" << tag.c_str() <<  id;
     local_histos.localPosXY[1] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[1]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Local_Position_XY_2S" << tag.c_str() <<  id;
     local_histos.localPosXY[2] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[2]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_Mixed" << tag.c_str() <<  id;
     local_histos.globalPosXY[0] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[0]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_PS" << tag.c_str() <<  id;
     local_histos.globalPosXY[1] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[1]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_2S" << tag.c_str() <<  id;
     local_histos.globalPosXY[2] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[2]->SetName(histoName.str().c_str());

    /*
     * Delta positions with SimHits
     */

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_PS" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_2S" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_PS" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_2S" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    /*
     * Delta position with simHits for primary tracks only
     */

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_PS_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_2S_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_PS_P" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_2S_P" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits_P[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    /*
     * Information on the Digis per cluster
     */

    histoName.str(""); histoName << "Total_Digis" << tag.c_str() <<  id;
    local_histos.totalSimHits= td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 10, 0., 10.);

    histoName.str(""); histoName << "Primary_Digis" << tag.c_str() <<  id;
    local_histos.primarySimHits= td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 10, 0., 10.);

    histoName.str(""); histoName << "Other_Digis" << tag.c_str() <<  id;
    local_histos.otherSimHits= td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 10, 0., 10.);


    std::pair< std::map< unsigned int, VHHistos >::iterator, bool > insertedIt(histograms_.insert(std::make_pair(ival, local_histos)));
    fs->file().cd("/");

    return insertedIt.first;
}

void VectorHitsBuilderValidation::CreateVHsXYGraph(const std::vector<Global3DPoint> glVHs,const std::vector<Global3DVector> dirVHs){

  if(glVHs.size() != dirVHs.size()){
    std::cout << "Cannot fullfil the graphs for this event. Return." << std::endl;
    return;
  }

  // opening canvas and drawing XY TGraph
  VHXY_->cd();
  trackerLayoutXY_[0]->Draw("AP");

  float finalposX, finalposY;

  for(unsigned int nVH = 0; nVH < glVHs.size(); nVH++){

    finalposX = glVHs.at(nVH).x() + dirVHs.at(nVH).x();
    finalposY = glVHs.at(nVH).y() + dirVHs.at(nVH).y();
    //std::cout << glVHs.at(nVH) << " " << " \tr: " << glVHs.at(nVH).perp() << std::endl;
    //std::cout << dirVHs.at(nVH).x() << "," << dirVHs.at(nVH).y() << std::endl;

    //same r
    if((fabs(dirVHs.at(nVH).x()) < 10e-5) && (fabs(dirVHs.at(nVH).y()) < 10e-5)){

      //std::cout << "same pos!";
      continue;

    } else {

      TArrow* vh_arrow = new TArrow(glVHs.at(nVH).x(), glVHs.at(nVH).y(), finalposX, finalposY, 0.05, ">");
      vh_arrow->Draw("same");

    }


  }

  return;
}

void VectorHitsBuilderValidation::CreateVHsRZGraph(const std::vector<Global3DPoint> glVHs,const std::vector<Global3DVector> dirVHs){

  if(glVHs.size() != dirVHs.size()){
    std::cout << "Cannot fullfil the graphs for this event. Return." << std::endl;
    return;
  }

  // opening canvas and drawing RZ TGraph
  VHRZ_->cd();
  trackerLayoutRZ_[0]->Draw("AP");

  float finalposX, finalposY, finalposR, finalposZ;

  for(unsigned int nVH = 0; nVH < glVHs.size(); nVH++){

    finalposX = glVHs.at(nVH).x() + dirVHs.at(nVH).x();
    finalposY = glVHs.at(nVH).y() + dirVHs.at(nVH).y();
    finalposR = sqrt( pow(finalposX,2) + pow(finalposY,2) );
    finalposZ = glVHs.at(nVH).z() + dirVHs.at(nVH).z();

    //std::cout << dirVHs.at(nVH) " " << " \tr: " << dirVHs.at(nVH).perp() << std::endl;
    //std::cout << finalposX << ", " << finalposY << " " << " \tr: " << finalposR << std::endl;
    //std::cout << std::endl;

    TArrow* vh_arrow = new TArrow(glVHs.at(nVH).z(), glVHs.at(nVH).perp(), finalposZ, finalposR, 0.05, "|>");
    vh_arrow->SetLineWidth(2);
    vh_arrow->Draw("same");

  }

  return;
}

unsigned int VectorHitsBuilderValidation::getLayerNumber(const DetId& detid) {
  if (detid.det() == DetId::Tracker) {
      if (detid.subdetId() == StripSubdetector::TOB) return (tkTopo->layer(detid));
      else if (detid.subdetId() == StripSubdetector::TID) return (100 * tkTopo->side(detid) + tkTopo->layer(detid));
      else return 999;
  }
  return 999;
//  return (tkTopo->layer(detid));
}

unsigned int VectorHitsBuilderValidation::getModuleNumber(const DetId& detid) {
  return (tkTopo->module(detid));
}

std::vector< std::pair<uint32_t, EncodedEventId> >
VectorHitsBuilderValidation::getSimTrackId(const edm::Handle<edm::DetSetVector<PixelDigiSimLink> >& simLinks,
                                           const DetId& detId, uint32_t channel) const 
{
  std::vector< std::pair<uint32_t, EncodedEventId> > simTrkId;
  auto isearch = simLinks->find(detId);
  if (isearch != simLinks->end()) {
    // Loop over DigiSimLink in this det unit
    edm::DetSet<PixelDigiSimLink> link_detset = (*isearch);
    for (typename edm::DetSet<PixelDigiSimLink>::const_iterator it  = link_detset.data.begin(); 
         it != link_detset.data.end(); ++it) {
      if (channel == it->channel()) simTrkId.push_back(std::make_pair(it->SimTrackId(), it->eventId()));
      }
  }
  return simTrkId;
}

unsigned int VectorHitsBuilderValidation::getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >& pixelSimLinks, const DetId& detId, unsigned int channel) {
    edm::DetSetVector< PixelDigiSimLink >::const_iterator DSViter(pixelSimLinks->find(detId));
    if (DSViter == pixelSimLinks->end()) return 0;
    for (edm::DetSet< PixelDigiSimLink >::const_iterator it = DSViter->data.begin(); it != DSViter->data.end(); ++it) {
        if (channel == it->channel()) return it->SimTrackId();
    }
    return 0;
}

void VectorHitsBuilderValidation::printCluster(const GeomDetUnit* geomDetUnit, const OmniClusterRef cluster){

  if (!geomDetUnit) return;
  
  const PixelGeomDetUnit* theGeomDet = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
  const PixelTopology& topol = theGeomDet->specificTopology();

  unsigned int layer = getLayerNumber(geomDetUnit->geographicalId());
  unsigned int module = getModuleNumber(geomDetUnit->geographicalId());
  LogTrace("VectorHitsBuilderValidation") << "Layer:" << layer << std::endl;
  if(topol.ncolumns() == 32)
    LogTrace("VectorHitsBuilderValidation") << "Pixel cluster with detId:" << geomDetUnit->geographicalId().rawId() << "(module:" << module << ") " << std::endl;
  else if(topol.ncolumns() == 2 )
    LogTrace("VectorHitsBuilderValidation") << "Strip cluster with detId:" << geomDetUnit->geographicalId().rawId() << "(module:" << module << ") " << std::endl;
  else std::cout << "no module?!" << std::endl;
  LogTrace("VectorHitsBuilderValidation") << "with pitch:" << topol.pitch().first << " , " << topol.pitch().second << std::endl;
  LogTrace("VectorHitsBuilderValidation") << " and width:" << theGeomDet->surface().bounds().width() << " , lenght:" << theGeomDet->surface().bounds().length() << std::endl;


  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*cluster,geomDetUnit);
  MeasurementPoint mpClu((*cluster.cluster_phase2OT()).center(), (*cluster.cluster_phase2OT()).column() + 0.5);
  Local3DPoint localPosClu = geomDetUnit->topology().localPosition(mpClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError localErrClu = geomDetUnit->topology().localError(mpClu,meClu);

  LogTrace("VectorHitsBuilderValidation") << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;

  return;
}

DEFINE_FWK_MODULE(VectorHitsBuilderValidation);
