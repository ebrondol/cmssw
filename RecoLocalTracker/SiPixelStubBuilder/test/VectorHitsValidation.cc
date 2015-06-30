#include <map>
#include <vector>
#include <algorithm>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
//#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
//#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"

#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/TrackingRecHit/interface/VectorHit.h"

#include <TH1F.h>
#include <TGraph.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TArrow.h>



struct ClusterHistos {
    THStack* numberClustersMixed;
    TH1F* numberClusterPixel;
    TH1F* numberClusterStrip;

    TGraph* globalPosXY[3];
    TGraph* localPosXY[3];

    TH1F* deltaXClusterSimHits[3];
    TH1F* deltaYClusterSimHits[3];

    TH1F* deltaXClusterSimHits_P[3];
    TH1F* deltaYClusterSimHits_P[3];

    TH1F* digiEfficiency[3];

    TH1F* primarySimHits;
    TH1F* otherSimHits;
};

class VectorHitsBuilderValidation : public edm::EDAnalyzer {

    public:

        typedef std::map< unsigned int, std::vector< PSimHit > > SimHitsMap;
        typedef std::map< unsigned int, SimTrack > SimTracksMap;

        explicit VectorHitsBuilderValidation(const edm::ParameterSet&);
        ~VectorHitsBuilderValidation();
        void beginJob();
        void endJob();
        void analyze(const edm::Event&, const edm::EventSetup&);

    private:

        std::map< unsigned int, ClusterHistos >::iterator createLayerHistograms(unsigned int);
        void CreateVHsXYGraph( const std::vector<Global3DPoint>,const  std::vector<Global3DVector> );
        void CreateVHsRZGraph( const std::vector<Global3DPoint>,const  std::vector<Global3DVector> );
        unsigned int getLayerNumber(const DetId&, const TrackerTopology*);
        unsigned int getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >&, const DetId&, unsigned int);

        edm::InputTag src_;
        edm::InputTag src2_;
        edm::InputTag links_;
        TGraph* trackerLayoutRZ_[3];
        TGraph* trackerLayoutXY_[3];
        TGraph* trackerLayoutXYBar_;
        TGraph* trackerLayoutXYEC_;
        TCanvas* VHXY_;
        TCanvas* VHRZ_;
        //std::vector<TArrow*> arrowVHs;

        std::map< unsigned int, ClusterHistos > histograms_;

};

VectorHitsBuilderValidation::VectorHitsBuilderValidation(const edm::ParameterSet& conf) :
    src_(conf.getParameter< edm::InputTag >("src")),
    src2_(conf.getParameter< edm::InputTag >("src2")),
    links_(conf.getParameter< edm::InputTag >("links")) { }

VectorHitsBuilderValidation::~VectorHitsBuilderValidation() { }

void VectorHitsBuilderValidation::beginJob() {

  edm::Service<TFileService> fs;
  fs->file().cd("/");
  TFileDirectory td = fs->mkdir("Common");

  // Create common graphs
  TFileDirectory tdPos = td.mkdir("Positions");
  trackerLayoutRZ_[0] = tdPos.make< TGraph >();
  trackerLayoutRZ_[0] -> SetName("RVsZ_Mixed");
  trackerLayoutRZ_[1] = tdPos.make< TGraph >();
  trackerLayoutRZ_[1] -> SetName("RVsZ_Pixel");
  trackerLayoutRZ_[2] = tdPos.make< TGraph >();
  trackerLayoutRZ_[2] -> SetName("RVsZ_Strip");
  trackerLayoutXY_[0] = tdPos.make< TGraph >();
  trackerLayoutXY_[0] -> SetName("YVsX_Mixed");
  trackerLayoutXY_[1] = tdPos.make< TGraph >();
  trackerLayoutXY_[1] -> SetName("YVsX_Pixel");
  trackerLayoutXY_[2] = tdPos.make< TGraph >();
  trackerLayoutXY_[2] -> SetName("YVsX_Strip");
  trackerLayoutXYBar_ = tdPos.make< TGraph >();
  trackerLayoutXYBar_ -> SetName("YVsXBar");
  trackerLayoutXYEC_ = tdPos.make< TGraph >();
  trackerLayoutXYEC_ -> SetName("YVsXEC");

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
    edm::Handle< edm::PSimHitContainer > simHitsRaw;
    event.getByLabel("g4SimHits", "TrackerHitsPixelBarrelLowTof", simHitsRaw);

    // Get the SimTracks
    edm::Handle< edm::SimTrackContainer > simTracksRaw;
    event.getByLabel("g4SimHits", simTracksRaw);

    // Get the SimVertex
    edm::Handle< edm::SimVertexContainer > simVertices;
    event.getByLabel("g4SimHits", simVertices);

    // Get the geometry
    edm::ESHandle< TrackerGeometry > geomHandle;
    eventSetup.get< TrackerDigiGeometryRecord >().get(geomHandle);
    const TrackerGeometry* tkGeom = &(*geomHandle);

    edm::ESHandle< TrackerTopology > tTopoHandle;
    eventSetup.get< IdealGeometryRecord >().get(tTopoHandle);
    const TrackerTopology* tTopo = tTopoHandle.product();


    // Rearrange the simTracks


    // Rearrange the simTracks for ease of use <simTrackID, simTrack>
    SimTracksMap simTracks;
    for (edm::SimTrackContainer::const_iterator simTrackIt(simTracksRaw->begin()); simTrackIt != simTracksRaw->end(); ++simTrackIt) simTracks.insert(std::pair< unsigned int, SimTrack >(simTrackIt->trackId(), *simTrackIt));


    // Rearrange the simHits by detUnit


    // Rearrange the simHits for ease of use
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
    unsigned int nVHsTot(0), nVHsPixelTot(0), nVHsStripTot(0);
    std::vector<Global3DPoint> glVHs;
    std::vector<Global3DVector> dirVHs;

    // Loop over modules
    for (VectorHitCollectionNew::const_iterator DSViter = vhs->begin(); DSViter != vhs->end(); ++DSViter) {

        // Get the detector unit's id
        unsigned int rawid(DSViter->detId());
	      DetId detId(rawid);
        unsigned int layer(getLayerNumber(detId, tTopo));

        // Get the geometry of the tracker
        const GeomDetUnit* geomDetUnit(tkGeom->idToDetUnit(detId));
        const PixelGeomDetUnit* theGeomDet = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit);
        const PixelTopology& topol = theGeomDet->specificTopology();

        if (!geomDetUnit) break;

        // Create histograms for the layer if they do not yet exist
        std::map< unsigned int, ClusterHistos >::iterator histogramLayer(histograms_.find(layer));
        if (histogramLayer == histograms_.end()) histogramLayer = createLayerHistograms(layer);

        // Number of clusters
        unsigned int nVHsPixel(0), nVHsStrip(0);


        // Loop over the clusters in the detector unit
        for (edmNew::DetSet< VectorHit >::const_iterator vhIt = DSViter->begin(); vhIt != DSViter->end(); ++vhIt) {

             // vh variables

             if( vhIt->isValid() ){

               Local3DPoint localPosVH = vhIt->localPosition();
               //std::cout << localPosVH << std::endl;

               Global3DPoint globalPosVH = geomDetUnit->surface().toGlobal(localPosVH);
               glVHs.push_back(globalPosVH);
               //std::cout << globalPosVH << std::endl;

               //Local3DVector localDirVH = vhIt->localDirection();
               //std::cout << localDirVH << std::endl;

               VectorHit vh = *vhIt;
               Global3DVector globalDirVH = vh.globalDirection(geomDetUnit->surface());
               dirVHs.push_back(globalDirVH);
               //std::cout << globalDirVH << std::endl;
               //std::cout << std::endl;

               // Fill the position histograms
               trackerLayoutRZ_[0]->SetPoint(nVHsTot, globalPosVH.z(), globalPosVH.perp());
               trackerLayoutXY_[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

               if (layer < 100) trackerLayoutXYBar_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());
               else trackerLayoutXYEC_->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

               histogramLayer->second.localPosXY[0]->SetPoint(nVHsTot, localPosVH.x(), localPosVH.y());
               histogramLayer->second.globalPosXY[0]->SetPoint(nVHsTot, globalPosVH.x(), globalPosVH.y());

               // Pixel module
               if (topol.ncolumns() == 32) {

                 trackerLayoutRZ_[1]->SetPoint(nVHsPixelTot, globalPosVH.z(), globalPosVH.perp());
                 trackerLayoutXY_[1]->SetPoint(nVHsPixelTot, globalPosVH.x(), globalPosVH.y());

                 histogramLayer->second.localPosXY[1]->SetPoint(nVHsPixelTot, localPosVH.x(), localPosVH.y());
                 histogramLayer->second.globalPosXY[1]->SetPoint(nVHsPixelTot, globalPosVH.x(), globalPosVH.y());

                 ++nVHsPixel;
                 ++nVHsPixelTot;
               }
               // Strip module
               else if (topol.ncolumns() == 2) {
                 trackerLayoutRZ_[2]->SetPoint(nVHsStripTot, globalPosVH.z(), globalPosVH.perp());
                 trackerLayoutXY_[2]->SetPoint(nVHsStripTot, globalPosVH.x(), globalPosVH.y());

                 histogramLayer->second.localPosXY[2]->SetPoint(nVHsStripTot, localPosVH.x(), localPosVH.y());
                 histogramLayer->second.globalPosXY[2]->SetPoint(nVHsStripTot, globalPosVH.x(), globalPosVH.y());

                 ++nVHsStrip;
                 ++nVHsStripTot;
               }

          }




/*

             // Digis related variables


            std::vector< unsigned int > clusterSimTrackIds;

            // Get all the simTracks that form the cluster
            for (unsigned int i(0); i < vhIt->size(); ++i) {
                unsigned int channel(PixelDigi::pixelToChannel(vhIt->firstRow() + i, vhIt->column())); // Here we have to use the old pixelToChannel function (not Phase2TrackerDigi but PixelDigi), change this when using new Digis
                unsigned int simTrackId(getSimTrackId(pixelSimLinks, detId, channel));
                clusterSimTrackIds.push_back(simTrackId);
            }


             // SimHits related variables



            unsigned int primarySimHits(0);
            unsigned int otherSimHits(0);

            for (edm::PSimHitContainer::const_iterator hitIt(simHitsRaw->begin()); hitIt != simHitsRaw->end(); ++hitIt) {
                if (rawid == hitIt->detUnitId() and std::find(clusterSimTrackIds.begin(), clusterSimTrackIds.end(), hitIt->trackId()) != clusterSimTrackIds.end()) {
                    Local3DPoint localPosHit(hitIt->localPosition());

                    histogramLayer->second.deltaXClusterSimHits[0]->Fill(localPosVH.x() - localPosHit.x());
                    histogramLayer->second.deltaYClusterSimHits[0]->Fill(localPosVH.y() - localPosHit.y());

                    // Pixel module
                    if (topol.ncolumns() == 32) {
                        histogramLayer->second.deltaXClusterSimHits[1]->Fill(localPosVH.x() - localPosHit.x());
                        histogramLayer->second.deltaYClusterSimHits[1]->Fill(localPosVH.y() - localPosHit.y());
                    }
                    // Strip module
                    else if (topol.ncolumns() == 2) {
                        histogramLayer->second.deltaXClusterSimHits[2]->Fill(localPosVH.x() - localPosHit.x());
                        histogramLayer->second.deltaYClusterSimHits[2]->Fill(localPosVH.y() - localPosHit.y());
                    }

                    ++otherSimHits;

                    std::map< unsigned int, SimTrack >::const_iterator simTrackIt(simTracks.find(hitIt->trackId()));
                    if (simTrackIt == simTracks.end()) continue;

                    // Primary particles only
                    unsigned int processType(hitIt->processType());
                    if (simTrackIt->second.vertIndex() == 0 and (processType == 2 || processType == 7 || processType == 9 || processType == 11 || processType == 13 || processType == 15)) {
                        histogramLayer->second.deltaXClusterSimHits_P[0]->Fill(localPosVH.x() - localPosHit.x());
                        histogramLayer->second.deltaYClusterSimHits_P[0]->Fill(localPosVH.y() - localPosHit.y());

                        // Pixel module
                        if (topol.ncolumns() == 32) {
                            histogramLayer->second.deltaXClusterSimHits_P[1]->Fill(localPosVH.x() - localPosHit.x());
                            histogramLayer->second.deltaYClusterSimHits_P[1]->Fill(localPosVH.y() - localPosHit.y());
                        }
                        // Strip module
                        else if (topol.ncolumns() == 2) {
                            histogramLayer->second.deltaXClusterSimHits_P[2]->Fill(localPosVH.x() - localPosHit.x());
                            histogramLayer->second.deltaYClusterSimHits_P[2]->Fill(localPosVH.y() - localPosHit.y());
                        }

                        ++primarySimHits;
                    }
                }
            }

            otherSimHits -= primarySimHits;

            histogramLayer->second.primarySimHits->Fill(primarySimHits);
            histogramLayer->second.otherSimHits->Fill(otherSimHits);
*/
        }

        if (nVHsPixel) histogramLayer->second.numberClusterPixel->Fill(nVHsPixel);
        if (nVHsStrip) histogramLayer->second.numberClusterStrip->Fill(nVHsStrip);
        nVHsTot++;

    }

    CreateVHsXYGraph(glVHs, dirVHs);
    CreateVHsRZGraph(glVHs, dirVHs);

}

// Create the histograms
std::map< unsigned int, ClusterHistos >::iterator VectorHitsBuilderValidation::createLayerHistograms(unsigned int ival) {
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

    ClusterHistos local_histos;

    std::ostringstream histoName;

    /*
     * Number of clusters
     */

    histoName.str(""); histoName << "Number_Clusters_Pixel" << tag.c_str() <<  id;
    local_histos.numberClusterPixel = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberClusterPixel->SetFillColor(kAzure + 7);

    histoName.str(""); histoName << "Number_Clusters_Strip" << tag.c_str() <<  id;
    local_histos.numberClusterStrip = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 20, 0., 20.);
    local_histos.numberClusterStrip->SetFillColor(kOrange - 3);

    histoName.str(""); histoName << "Number_Clusters_Mixed" << tag.c_str() <<  id;
    local_histos.numberClustersMixed = td.make< THStack >(histoName.str().c_str(), histoName.str().c_str());
    local_histos.numberClustersMixed->Add(local_histos.numberClusterPixel);
    local_histos.numberClustersMixed->Add(local_histos.numberClusterStrip);

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

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Pixel" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Strip" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Mixed" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Pixel" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Strip" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    /*
     * Delta position with simHits for primary tracks only
     */

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Pixel_P" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_X_Cluster_SimHits_Strip_P" << tag.c_str() <<  id;
    local_histos.deltaXClusterSimHits_P[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Mixed_P" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits_P[0] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Pixel_P" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits_P[1] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    histoName.str(""); histoName << "Delta_Y_Cluster_SimHits_Strip_P" << tag.c_str() <<  id;
    local_histos.deltaYClusterSimHits_P[2] = td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 100, 0., 0.);

    /*
     * Information on the Digis per cluster
     */

    histoName.str(""); histoName << "Primary_Digis" << tag.c_str() <<  id;
    local_histos.primarySimHits= td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 10, 0., 10.);

    histoName.str(""); histoName << "Other_Digis" << tag.c_str() <<  id;
    local_histos.otherSimHits= td.make< TH1F >(histoName.str().c_str(), histoName.str().c_str(), 10, 0., 10.);


    std::pair< std::map< unsigned int, ClusterHistos >::iterator, bool > insertedIt(histograms_.insert(std::make_pair(ival, local_histos)));
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
    std::cout << glVHs.at(nVH) << " " << " \tr: " << glVHs.at(nVH).perp() << std::endl;
    std::cout << dirVHs.at(nVH).x() << "," << dirVHs.at(nVH).y() << std::endl;

    //same r
    if((fabs(dirVHs.at(nVH).x()) < 10e-5) && (fabs(dirVHs.at(nVH).y()) < 10e-5)){

      std::cout << "same pos!";

    } else {

      TArrow* vh_arrow = new TArrow(glVHs.at(nVH).x(), glVHs.at(nVH).y(), finalposX, finalposY, 0.05, ">");
      vh_arrow->SetLineWidth(2);
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

unsigned int VectorHitsBuilderValidation::getLayerNumber(const DetId& detid, const TrackerTopology* topo) {
    if (detid.det() == DetId::Tracker) {
        if (detid.subdetId() == PixelSubdetector::PixelBarrel) return (topo->pxbLayer(detid));
        else if (detid.subdetId() == PixelSubdetector::PixelEndcap) return (100 * topo->pxfSide(detid) + topo->pxfDisk(detid));
        else return 999;
    }
    return 999;
}

unsigned int VectorHitsBuilderValidation::getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >& pixelSimLinks, const DetId& detId, unsigned int channel) {
    edm::DetSetVector< PixelDigiSimLink >::const_iterator DSViter(pixelSimLinks->find(detId));
    if (DSViter == pixelSimLinks->end()) return 0;
    for (edm::DetSet< PixelDigiSimLink >::const_iterator it = DSViter->data.begin(); it != DSViter->data.end(); ++it) {
        if (channel == it->channel()) return it->SimTrackId();
    }
    return 0;
}

DEFINE_FWK_MODULE(VectorHitsBuilderValidation);
