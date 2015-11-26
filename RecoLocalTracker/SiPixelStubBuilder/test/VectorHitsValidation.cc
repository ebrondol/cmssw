#include "RecoLocalTracker/SiPixelStubBuilder/test/VectorHitsValidation.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"

VectorHitsBuilderValidation::VectorHitsBuilderValidation(const edm::ParameterSet& conf) :
    src_(conf.getParameter< edm::InputTag >("src")),
    src2_(conf.getParameter< edm::InputTag >("src2")),
    links_(conf.getParameter< edm::InputTag >("links")) { 
  std::cout << "VectorHitsBuilderValidation starting" << std::endl;
}

VectorHitsBuilderValidation::~VectorHitsBuilderValidation() { 
  std::cout << "VectorHitsBuilderValidation ending" << std::endl;
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
    event.getByLabel(src_, clusters);

    // Get the vector hits
    edm::Handle< VectorHitCollectionNew > vhs;
    event.getByLabel(src2_, vhs);

    // Get the PixelDigiSimLinks
    edm::Handle< edm::DetSetVector< PixelDigiSimLink > > pixelSimLinks;
    event.getByLabel(links_, pixelSimLinks);

    // Get the SimHits
    edm::Handle< edm::PSimHitContainer > simHitsRawBarrel;
    event.getByLabel("g4SimHits", "TrackerHitsPixelBarrelLowTof", simHitsRawBarrel);
    edm::Handle< edm::PSimHitContainer > simHitsRawEndcap;
    event.getByLabel("g4SimHits", "TrackerHitsPixelEndcapLowTof", simHitsRawEndcap);

    // Get the SimTracks
    edm::Handle< edm::SimTrackContainer > simTracksRaw;
    event.getByLabel("g4SimHits", simTracksRaw);

    // Get the SimVertex
    edm::Handle< edm::SimVertexContainer > simVertices;
    event.getByLabel("g4SimHits", simVertices);

    // Get the geometry
    edm::ESHandle< TrackerGeometry > geomHandle;
    eventSetup.get< TrackerDigiGeometryRecord >().get(geomHandle);
    tkGeom = &(*geomHandle);
    edm::ESHandle< TrackerTopology > tTopoHandle;
    eventSetup.get< IdealGeometryRecord >().get(tTopoHandle);
    tkTopo = tTopoHandle.product();

    //set up for tree
    int layer;
    int module_id;
    int module_number;
    int module_type; //1: pixel, 2: strip
    float x_global, y_global, z_global;
    float x_local, y_local, z_local;
    int sim_track_id;
    float deltaXVHSimHits, deltaYVHSimHits;
    unsigned int processType(99);

    tree -> Branch("layer",&layer,"layer/I");
    tree -> Branch("module_id",&module_id,"module_id/I");
    tree -> Branch("module_type",&module_type,"module_type/I");
    tree -> Branch("module_number",&module_number,"module_number/I");
    tree -> Branch("x_global",&x_global,"x_global/F");
    tree -> Branch("y_global",&y_global,"y_global/F");
    tree -> Branch("z_global",&z_global,"z_global/F");
    tree -> Branch("x_local",&x_local,"x_local/F");
    tree -> Branch("y_local",&y_local,"y_local/F");
    tree -> Branch("z_local",&z_local,"z_local/F");
    tree -> Branch("sim_track_id",&sim_track_id,"sim_track_id/I");
    tree -> Branch("deltaXVHSimHits",&deltaXVHSimHits,"deltaXVHSimHits/F");
    tree -> Branch("deltaYVHSimHits",&deltaYVHSimHits,"deltaYVHSimHits/F");
    tree -> Branch("processType",&processType,"processType/i");

    // Rearrange the simTracks for ease of use <simTrackID, simTrack>
    SimTracksMap simTracks;
    for (edm::SimTrackContainer::const_iterator simTrackIt(simTracksRaw->begin()); simTrackIt != simTracksRaw->end(); ++simTrackIt) simTracks.insert(std::pair< unsigned int, SimTrack >(simTrackIt->trackId(), *simTrackIt));

    // Collect barrel and endcap SimHits in one vector
    edm::PSimHitContainer simHitsRaw;
    simHitsRaw.reserve( simHitsRawBarrel->size() + simHitsRawEndcap->size() ); // preallocate memory
    simHitsRaw.insert( simHitsRaw.end(), simHitsRawBarrel->begin(), simHitsRawBarrel->end() );
    simHitsRaw.insert( simHitsRaw.end(), simHitsRawEndcap->begin(), simHitsRawEndcap->end() );

    // Rearrange the simHits by detUnit for ease of use
    SimHitsMap simHitsDetUnit;
    SimHitsMap simHitsTrackId;
    for (edm::PSimHitContainer::const_iterator simHitIt(simHitsRaw.begin()); simHitIt != simHitsRaw.end(); ++simHitIt) {
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
    unsigned int nVHsTot(0), nVHsPixelTot(0), nVHsStripTot(0);
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

      LogDebug("VectorHitsBuilderValidation") << "det id" << rawid << std::endl;

      // Get the geometry of the tracker
      const GeomDet* geomDet(tkGeom->idToDet(detId));
      if (!geomDet) break;

      // Create histograms for the layer if they do not yet exist
      std::map< unsigned int, VHHistos >::iterator histogramLayer(histograms_.find(layer));
      if (histogramLayer == histograms_.end()) histogramLayer = createLayerHistograms(layer);
      // Number of clusters
      unsigned int nVHsPixel(0), nVHsStrip(0);

      // Loop over the clusters in the detector unit
      for (edmNew::DetSet< VectorHit >::const_iterator vhIt = DSViter->begin(); vhIt != DSViter->end(); ++vhIt) {

        // vh variables
        if( vhIt->isValid() ){

           LogDebug("VectorHitsBuilderValidation") << " vh analyzing ..." ;
           Local3DPoint localPosVH = vhIt->localPosition();
           x_local = localPosVH.x();
           y_local = localPosVH.y();
           z_local = localPosVH.z();
           LogTrace("VectorHitsBuilderValidation") << "local VH position " << localPosVH << std::endl;

           Global3DPoint globalPosVH = geomDet->surface().toGlobal(localPosVH);
           x_global = globalPosVH.x();
           y_global = globalPosVH.y();
           z_global = globalPosVH.z();
           glVHs.push_back(globalPosVH);
           LogTrace("VectorHitsBuilderValidation") << " global VH position " << globalPosVH << std::endl;

           Local3DVector localDirVH = vhIt->localDirection();
           LogTrace("VectorHitsBuilderValidation") << "local VH direction " << localDirVH << std::endl;

           VectorHit vh = *vhIt;
           Global3DVector globalDirVH = vh.globalDirection(geomDet->surface());
           dirVHs.push_back(globalDirVH);
           LogTrace("VectorHitsBuilderValidation") << "global VH direction " << globalDirVH << std::endl;

           // Fill the position histograms
           trackerLayoutRZ_[0]->SetPoint(nVHsTot, globalPosVH.z(), globalPosVH.perp());
           trackerLayoutXY_[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

           if (layer < 100) trackerLayoutXYBar_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());
           else trackerLayoutXYEC_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

           histogramLayer->second.localPosXY[0]->SetPoint(nVHsTot, localPosVH.x(), localPosVH.y());
           histogramLayer->second.globalPosXY[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

           localPosXvsDeltaX_[0]->SetPoint(nVHsTot, localPosVH.x(), localDirVH.x());
           localPosYvsDeltaY_[0]->SetPoint(nVHsTot, localPosVH.y(), localDirVH.y());

           // Pixel module
          if (layer == 1 || layer == 2 || layer == 3) {

             module_type = 1;
             trackerLayoutRZ_[1]->SetPoint(nVHsPixelTot, globalPosVH.z(), globalPosVH.perp());
             trackerLayoutXY_[1]->SetPoint(nVHsPixelTot, globalPosVH.x(), globalPosVH.y());

             histogramLayer->second.localPosXY[1]->SetPoint(nVHsPixelTot, localPosVH.x(), localPosVH.y());
             histogramLayer->second.globalPosXY[1]->SetPoint(nVHsPixelTot, globalPosVH.x(), globalPosVH.y());

             localPosXvsDeltaX_[1]->SetPoint(nVHsPixelTot, localPosVH.x(), localDirVH.x());
             localPosYvsDeltaY_[1]->SetPoint(nVHsPixelTot, localPosVH.y(), localDirVH.y());

             ++nVHsPixel;
             ++nVHsPixelTot;

           }

           // Strip module
          else if (layer == 4 || layer == 5 || layer == 6) {
             module_type = 2;
             trackerLayoutRZ_[2]->SetPoint(nVHsStripTot, globalPosVH.z(), globalPosVH.perp());
             trackerLayoutXY_[2]->SetPoint(nVHsStripTot, globalPosVH.x(), globalPosVH.y());

             histogramLayer->second.localPosXY[2]->SetPoint(nVHsStripTot, localPosVH.x(), localPosVH.y());
             histogramLayer->second.globalPosXY[2]->SetPoint(nVHsStripTot, globalPosVH.x(), globalPosVH.y());

             localPosXvsDeltaX_[2]->SetPoint(nVHsStripTot, localPosVH.x(), localDirVH.x());
             localPosYvsDeltaY_[2]->SetPoint(nVHsStripTot, localPosVH.y(), localDirVH.y());

             ++nVHsStrip;
             ++nVHsStripTot;
           }

           // get the geomDetUnit of the clusters
           LogDebug("VectorHitsBuilderValidation") << "print Clusters into the VH:" << std::endl;
           const StackGeomDet* stackDet = dynamic_cast<const StackGeomDet*>(geomDet);
           const GeomDetUnit* geomDetLower = stackDet->lowerDet();
           printCluster(geomDetLower,vhIt->innerCluster());
           const GeomDetUnit* geomDetUpper = stackDet->upperDet();
           printCluster(geomDetUpper,vhIt->outerCluster());

           //comparison with SIM hits
           std::vector< unsigned int > clusterSimTrackIds;

           // Get the simTrack that pass into the first digi of the inner clusters
           // ERICA: it should be done also for the outer? don t think so.
           for (unsigned int i(0); i < vhIt->innerCluster()->size(); ++i) {
             //FIXME:: IT SHOULD BE vhIt->innerCluster()->firstDigi().channel(), but
             //here we have to use the old pixelToChannel function (not Phase2TrackerDigi but PixelDigi), change this when using new Digis
             unsigned int InnerChannel(PixelDigi::pixelToChannel(vhIt->innerCluster()->firstRow() + i, vhIt->innerCluster()->column()));

             unsigned int InnerSimTrackId(getSimTrackId(pixelSimLinks, detId, InnerChannel));
             clusterSimTrackIds.push_back(InnerSimTrackId);
           }

           // loop over all simHits
           unsigned int totalSimHits(0);
           unsigned int primarySimHits(0);
           unsigned int otherSimHits(0);

           for (edm::PSimHitContainer::const_iterator hitIt(simHitsRaw.begin()); hitIt != simHitsRaw.end(); ++hitIt) {
             //check det compatibility
             if (rawid == hitIt->detUnitId()){
               //check clusters track id compatibility
               if(std::find(clusterSimTrackIds.begin(), clusterSimTrackIds.end(), hitIt->trackId()) != clusterSimTrackIds.end()){

                 Local3DPoint localPosHit(hitIt->localPosition());

                 sim_track_id = hitIt->trackId();
                 deltaXVHSimHits = localPosVH.x() - localPosHit.x();
                 deltaYVHSimHits = localPosVH.y() - localPosHit.y();


                 histogramLayer->second.deltaXVHSimHits[0]->Fill(localPosVH.x() - localPosHit.x());
                 histogramLayer->second.deltaYVHSimHits[0]->Fill(localPosVH.y() - localPosHit.y());

                 // Pixel module
                if (layer == 1 || layer == 2 || layer == 3) {
                     histogramLayer->second.deltaXVHSimHits[1]->Fill(localPosVH.x() - localPosHit.x());
                     histogramLayer->second.deltaYVHSimHits[1]->Fill(localPosVH.y() - localPosHit.y());
                 }
                 // Strip module
                 else if (layer == 4 || layer == 5 || layer == 6) {
                     histogramLayer->second.deltaXVHSimHits[2]->Fill(localPosVH.x() - localPosHit.x());
                     histogramLayer->second.deltaYVHSimHits[2]->Fill(localPosVH.y() - localPosHit.y());
                 }

                 ++totalSimHits;

                 std::map< unsigned int, SimTrack >::const_iterator simTrackIt(simTracks.find(hitIt->trackId()));
                 if (simTrackIt == simTracks.end()) continue;

                 // Primary particles only
                 processType = hitIt->processType();

                 if (simTrackIt->second.vertIndex() == 0 and (processType == 2 || processType == 7 || processType == 9 || processType == 11 || processType == 13 || processType == 15)) {
                     histogramLayer->second.deltaXVHSimHits_P[0]->Fill(localPosVH.x() - localPosHit.x());
                     histogramLayer->second.deltaYVHSimHits_P[0]->Fill(localPosVH.y() - localPosHit.y());

                     // Pixel module
                     if (layer == 1 || layer == 2 || layer == 3) {
                         histogramLayer->second.deltaXVHSimHits_P[1]->Fill(localPosVH.x() - localPosHit.x());
                         histogramLayer->second.deltaYVHSimHits_P[1]->Fill(localPosVH.y() - localPosHit.y());
                     }
                     // Strip module
                     else if (layer == 4 || layer == 5 || layer == 6) {
                         histogramLayer->second.deltaXVHSimHits_P[2]->Fill(localPosVH.x() - localPosHit.x());
                         histogramLayer->second.deltaYVHSimHits_P[2]->Fill(localPosVH.y() - localPosHit.y());
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

        }// vh valid

      }// loop vhs

        if (nVHsPixel) histogramLayer->second.numberVHPixel->Fill(nVHsPixel);
        if (nVHsStrip) histogramLayer->second.numberVHStrip->Fill(nVHsStrip);
        nVHsTot++;
        tree->Fill();

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

    histoName.str(""); histoName << "Number_VHs_Pixel" << tag.c_str() <<  id;
    local_histos.numberVHPixel = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberVHPixel->SetFillColor(kAzure + 7);

    histoName.str(""); histoName << "Number_VHs_Strip" << tag.c_str() <<  id;
    local_histos.numberVHStrip = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberVHStrip->SetFillColor(kOrange - 3);

    histoName.str(""); histoName << "Number_VHs_Mixed" << tag.c_str() <<  id;
    local_histos.numberVHsMixed = td.make< THStack >(histoName.str().c_str(), histoName.str().c_str());
    local_histos.numberVHsMixed->Add(local_histos.numberVHPixel);
    local_histos.numberVHsMixed->Add(local_histos.numberVHStrip);

      /*
     * Local and Global positions
     */

     histoName.str(""); histoName << "Local_Position_XY_Mixed" << tag.c_str() <<  id;
     local_histos.localPosXY[0] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[0]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Local_Position_XY_Pixel" << tag.c_str() <<  id;
     local_histos.localPosXY[1] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[1]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Local_Position_XY_Strip" << tag.c_str() <<  id;
     local_histos.localPosXY[2] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2000, 0., 0., 2000, 0., 0.);
     local_histos.localPosXY[2]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_Mixed" << tag.c_str() <<  id;
     local_histos.globalPosXY[0] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[0]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_Pixel" << tag.c_str() <<  id;
     local_histos.globalPosXY[1] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[1]->SetName(histoName.str().c_str());

     histoName.str(""); histoName << "Global_Position_XY_Strip" << tag.c_str() <<  id;
     local_histos.globalPosXY[2] = td.make< TGraph >();//histoName.str().c_str(), histoName.str().c_str(), 2400, -120.0, 120.0, 2400, -120.0, 120.0);
     local_histos.globalPosXY[2]->SetName(histoName.str().c_str());

    /*
     * Delta positions with SimHits
     */

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Pixel" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Strip" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Pixel" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Strip" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    /*
     * Delta position with simHits for primary tracks only
     */

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Pixel_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_VH_SimHits_Strip_P" << tag.c_str() <<  id;
    local_histos.deltaXVHSimHits_P[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Pixel_P" << tag.c_str() <<  id;
    local_histos.deltaYVHSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_VH_SimHits_Strip_P" << tag.c_str() <<  id;
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
  return (tkTopo->layer(detid));
}

unsigned int VectorHitsBuilderValidation::getModuleNumber(const DetId& detid) {
  return (tkTopo->module(detid));
}

unsigned int VectorHitsBuilderValidation::getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >& pixelSimLinks, const DetId& detId, unsigned int channel) {
/*
    edm::DetSetVector< PixelDigiSimLink >::const_iterator DSViter(pixelSimLinks->find(detId));
    if (DSViter == pixelSimLinks->end()) return 0;
    for (edm::DetSet< PixelDigiSimLink >::const_iterator it = DSViter->data.begin(); it != DSViter->data.end(); ++it) {
        if (channel == it->channel()) return it->SimTrackId();
    }
*/
    return 0;
}

void VectorHitsBuilderValidation::printCluster(const GeomDetUnit* geomDetUnit, const Phase2TrackerCluster1DRef & cluster){

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
  MeasurementPoint mpClu(cluster->center(), cluster->column() + 0.5);
  Local3DPoint localPosClu = geomDetUnit->topology().localPosition(mpClu);
  Global3DPoint globalPosClu = geomDetUnit->surface().toGlobal(localPosClu);
  MeasurementError meClu(1./12,0.0,1./12);
  LocalError localErrClu = geomDetUnit->topology().localError(mpClu,meClu);

  LogTrace("VectorHitsBuilderValidation") << "\t global pos " << globalPosClu << std::endl;
  LogTrace("VectorHitsBuilderValidation") << "\t local  pos " << localPosClu << "with err " << localErrClu << std::endl;

  return;
}

DEFINE_FWK_MODULE(VectorHitsBuilderValidation);
