#include <string>
#include <unordered_map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMGlobalEDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/transform.h"
#include "DataFormats/HGCalReco/interface/Trackster.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

using namespace ticl;
using namespace edm;

struct Histogram_TICLTrackstersValidation {
  dqm::reco::MonitorElement* energy_;
};

using Histograms_TICLTrackstersValidation = std::unordered_map<int, Histogram_TICLTrackstersValidation>;

class TICLTrackstersValidation : public DQMGlobalEDAnalyzer<Histograms_TICLTrackstersValidation> {
public:
  explicit TICLTrackstersValidation(const edm::ParameterSet&);
  ~TICLTrackstersValidation() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void bookHistograms(DQMStore::IBooker&,
                      edm::Run const&,
                      edm::EventSetup const&,
                      Histograms_TICLTrackstersValidation&) const override;

  void dqmAnalyze(edm::Event const&, edm::EventSetup const&, Histograms_TICLTrackstersValidation const&) const override;
  void dqmBeginRun(edm::Run const&, edm::EventSetup const&, Histograms_TICLTrackstersValidation&);

  std::string folder_;
  std::vector<edm::EDGetTokenT<std::vector<Trackster>>> tracksterTokens_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClustersToken_;
  hgcal::RecHitTools rhtools_;
};

TICLTrackstersValidation::TICLTrackstersValidation(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")) {
  tracksterTokens_ =
      edm::vector_transform(iConfig.getParameter<std::vector<edm::InputTag>>("tracksterCollections"),
                            [this](edm::InputTag const& tag) { return consumes<std::vector<Trackster>>(tag); });
  layerClustersToken_ = consumes<std::vector<reco::CaloCluster>>(iConfig.getParameter<edm::InputTag>("layerClusters"));
}

TICLTrackstersValidation::~TICLTrackstersValidation() {}

void TICLTrackstersValidation::dqmAnalyze(edm::Event const& iEvent,
                                          edm::EventSetup const& iSetup,
                                          Histograms_TICLTrackstersValidation const& histos) const {
  edm::Handle<std::vector<reco::CaloCluster>> layerClustersH;
  iEvent.getByToken(layerClustersToken_, layerClustersH);
  auto const& layerClusters = *layerClustersH.product();
  for (auto& trackster_token : tracksterTokens_) {
    edm::Handle<std::vector<Trackster>> trackster_h;
    iEvent.getByToken(trackster_token, trackster_h);
    auto numberOfTracksters = trackster_h->size();

    for (size_t i = 0; i < numberOfTracksters; ++i) {
      auto const& trackster = (*trackster_h)[i];
    }
  }
}

void TICLTrackstersValidation::bookHistograms(DQMStore::IBooker& ibook,
                                              edm::Run const& run,
                                              edm::EventSetup const& iSetup,
                                              Histograms_TICLTrackstersValidation& histos) const {
  ibook.setCurrentFolder(folder_ + "TICLTracksters/");
}

void TICLTrackstersValidation::dqmBeginRun(edm::Run const& run,
                                           edm::EventSetup const& iSetup,
                                           Histograms_TICLTrackstersValidation& histograms) {
  rhtools_.getEventSetup(iSetup);
}

void TICLTrackstersValidation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  std::vector<edm::InputTag> source_vector{edm::InputTag("ticlTrackstersTrk"),
                                           edm::InputTag("ticlTrackstersMIP"),
                                           edm::InputTag("ticlTrackstersEM"),
                                           edm::InputTag("ticlTrackstersHAD"),
                                           edm::InputTag("ticlTrackstersMerge")};
  desc.add<std::vector<edm::InputTag>>("tracksterCollections", source_vector);
  desc.add<std::string>("folder", "HGCAL/");
  descriptions.add("ticlTrackstersValidationDefault", desc);
}

DEFINE_FWK_MODULE(TICLTrackstersValidation);
