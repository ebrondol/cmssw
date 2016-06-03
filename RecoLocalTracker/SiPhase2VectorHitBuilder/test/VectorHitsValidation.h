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
#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"

#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"
#include "DataFormats/TrackerRecHit2D/interface/OmniClusterRef.h"

#include <TH1F.h>
#include <TGraph.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TArrow.h>

struct VHHistos {
    THStack* numberVHsMixed;
    TH1F* numberVHsPS;
    TH1F* numberVHs2S;

    TGraph* globalPosXY[3];
    TGraph* localPosXY[3];

    TH1F* deltaXVHSimHits[3];
    TH1F* deltaYVHSimHits[3];

    TH1F* deltaXVHSimHits_P[3];
    TH1F* deltaYVHSimHits_P[3];

    TH1F* digiEfficiency[3];

    TH1F* totalSimHits;
    TH1F* primarySimHits;
    TH1F* otherSimHits;
};

class VectorHitsBuilderValidation : public edm::EDAnalyzer {

    public:
        typedef edm::Ref<edmNew::DetSetVector<Phase2TrackerCluster1D>, Phase2TrackerCluster1D > Phase2TrackerCluster1DRef;

        typedef std::map< unsigned int, std::vector< PSimHit > > SimHitsMap;
        typedef std::map< unsigned int, SimTrack > SimTracksMap;

        explicit VectorHitsBuilderValidation(const edm::ParameterSet&);
        ~VectorHitsBuilderValidation();
        void beginJob();
        void endJob();
        void analyze(const edm::Event&, const edm::EventSetup&);

    private:

        std::map< unsigned int, VHHistos >::iterator createLayerHistograms(unsigned int);
        void CreateVHsXYGraph( const std::vector<Global3DPoint>,const  std::vector<Global3DVector> );
        void CreateVHsRZGraph( const std::vector<Global3DPoint>,const  std::vector<Global3DVector> );
        unsigned int getLayerNumber(const DetId&);
        unsigned int getModuleNumber(const DetId& detid);
        void printCluster(const GeomDetUnit* geomDetUnit, const OmniClusterRef cluster);
        std::vector< std::pair<uint32_t, EncodedEventId> > getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >&, const DetId&, uint32_t) const;
        unsigned int getSimTrackId(const edm::Handle< edm::DetSetVector< PixelDigiSimLink > >& pixelSimLinks, const DetId& detId, unsigned int channel) ;

        edm::EDGetTokenT< edmNew::DetSetVector<Phase2TrackerCluster1D> > srcClu_;
        edm::EDGetTokenT< VectorHitCollectionNew > srcVH_;
        edm::EDGetTokenT< edm::DetSetVector<PixelDigiSimLink> > siphase2OTSimLinksToken_;
        //SiPixelVectorHitBuilderAlgorithmBase *algo;
        const TrackerGeometry* tkGeom;
        const TrackerTopology* tkTopo;

        TTree* tree;
        TGraph* trackerLayoutRZ_[3];
        TGraph* trackerLayoutXY_[3];
        TGraph* trackerLayoutXYBar_;
        TGraph* trackerLayoutXYEC_;
        TGraph* localPosXvsDeltaX_[3];
        TGraph* localPosYvsDeltaY_[3];
        TCanvas* VHXY_;
        TCanvas* VHRZ_;
        std::vector<TArrow*> arrowVHs;

        std::map< unsigned int, VHHistos > histograms_;

};
