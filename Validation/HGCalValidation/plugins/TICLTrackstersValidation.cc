#include <string>
#include <unordered_map>

// user include files
#include "DataFormats/Math/interface/Point3D.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMGlobalEDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/transform.h"
#include "DataFormats/HGCalReco/interface/Trackster.h"
#include "DataFormats/HGCalReco/interface/TICLSeedingRegion.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

using namespace ticl;

struct Histogram_TICLTrackstersValidation {
  dqm::reco::MonitorElement* number_;
  dqm::reco::MonitorElement* energy_;
  dqm::reco::MonitorElement* delta_energy_;
  dqm::reco::MonitorElement* delta_energy_relative_;
  dqm::reco::MonitorElement* delta_energy_vs_energy_;
  dqm::reco::MonitorElement* delta_energy_vs_layer_;
  dqm::reco::MonitorElement* delta_energy_relative_vs_layer_;
  dqm::reco::MonitorElement* delta_layer_;
  dqm::reco::MonitorElement* ingoing_links_vs_layer_;
  dqm::reco::MonitorElement* outgoing_links_vs_layer_;
  // For the definition of the angles, read http://hgcal.web.cern.ch/hgcal/Reconstruction/Tutorial/
  dqm::reco::MonitorElement* angle_alpha_;
  dqm::reco::MonitorElement* angle_alpha_alternative_;
  dqm::reco::MonitorElement* angle_beta_;
  std::vector<dqm::reco::MonitorElement*> angle_beta_byLayer_;
  std::vector<dqm::reco::MonitorElement*> angle_beta_w_byLayer_;
};

using Histograms_TICLTrackstersValidation = std::unordered_map<unsigned int, Histogram_TICLTrackstersValidation>;

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
  void dqmBeginRun(edm::Run const&, edm::EventSetup const&, Histograms_TICLTrackstersValidation&) const override;

  std::string folder_;
  std::vector<std::string> trackstersCollectionsNames_;
  std::vector<edm::EDGetTokenT<std::vector<Trackster>>> tracksterTokens_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClustersToken_;
  edm::EDGetTokenT<std::vector<TICLSeedingRegion>> ticlSeedingGlobalToken_;
  edm::EDGetTokenT<std::vector<TICLSeedingRegion>> ticlSeedingTrkToken_;
  mutable hgcal::RecHitTools rhtools_;
};

TICLTrackstersValidation::TICLTrackstersValidation(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")) {
  tracksterTokens_ = edm::vector_transform(iConfig.getParameter<std::vector<edm::InputTag>>("tracksterCollections"),
                                           [this](edm::InputTag const& tag) {
                                             trackstersCollectionsNames_.emplace_back(tag.label());
                                             return consumes<std::vector<Trackster>>(tag);
                                           });
  layerClustersToken_ = consumes<std::vector<reco::CaloCluster>>(iConfig.getParameter<edm::InputTag>("layerClusters"));
  ticlSeedingGlobalToken_ = consumes<std::vector<TICLSeedingRegion>>(iConfig.getParameter<edm::InputTag>("ticlSeedingGlobal"));
  ticlSeedingTrkToken_ = consumes<std::vector<TICLSeedingRegion>>(iConfig.getParameter<edm::InputTag>("ticlSeedingTrk"));
}

TICLTrackstersValidation::~TICLTrackstersValidation() {}

void TICLTrackstersValidation::dqmAnalyze(edm::Event const& iEvent,
                                          edm::EventSetup const& iSetup,
                                          Histograms_TICLTrackstersValidation const& histos) const {

  edm::Handle<std::vector<reco::CaloCluster>> layerClustersH;
  iEvent.getByToken(layerClustersToken_, layerClustersH);
  auto const& layerClusters = *layerClustersH.product();

  edm::Handle<std::vector<TICLSeedingRegion>> ticlSeedingGlobalH;
  iEvent.getByToken(ticlSeedingGlobalToken_, ticlSeedingGlobalH);
  auto const& ticlSeedingGlobal = *ticlSeedingGlobalH.product();

  edm::Handle<std::vector<TICLSeedingRegion>> ticlSeedingTrkH;
  iEvent.getByToken(ticlSeedingTrkToken_, ticlSeedingTrkH);
  auto const& ticlSeedingTrk = *ticlSeedingTrkH.product();

  for (const auto& trackster_token : tracksterTokens_) {
    edm::Handle<std::vector<Trackster>> trackster_h;
    iEvent.getByToken(trackster_token, trackster_h);
    auto numberOfTracksters = trackster_h->size();
    //using .at() as [] is not const
    const auto& histo = histos.at(trackster_token.index());
    histo.number_->Fill(numberOfTracksters);
    for (unsigned int i = 0; i < numberOfTracksters; ++i) {
      const auto& thisTrackster = trackster_h->at(i);
      histo.energy_->Fill(thisTrackster.regressed_energy());
      for (const auto &edge : thisTrackster.edges()) {
        auto & ic = layerClusters[edge[0]];
        auto & oc = layerClusters[edge[1]];
        auto const & cl_in = ic.hitsAndFractions()[0].first;
        auto const & cl_out = oc.hitsAndFractions()[0].first;
        auto const layer_in = rhtools_.getLayerWithOffset(cl_in);
        auto const layer_out = rhtools_.getLayerWithOffset(cl_out);
        histo.delta_energy_->Fill(oc.energy() - ic.energy());
        histo.delta_energy_relative_->Fill((oc.energy() - ic.energy())/ic.energy());
        histo.delta_energy_vs_energy_->Fill(oc.energy() - ic.energy(), ic.energy());
        histo.delta_energy_vs_layer_->Fill(layer_in, oc.energy() - ic.energy());
        histo.delta_energy_relative_vs_layer_->Fill(layer_in, (oc.energy() - ic.energy())/ic.energy());
        histo.delta_layer_->Fill(layer_out - layer_in);

        // Alpha angles
        const auto & outer_outer_pos = oc.position();
        const auto & outer_inner_pos = ic.position();
        const auto & seed = thisTrackster.seedIndex();
        auto seedGlobalPos = math::XYZPoint(ticlSeedingGlobal[0].origin.x(),
            ticlSeedingGlobal[0].origin.y(), ticlSeedingGlobal[0].origin.z());
        auto seedDirectionPos = outer_inner_pos;
        if (thisTrackster.seedID().id() != 0) {
          // Seed to trackster association is, at present, rather convoluted.
          for (auto const &s : ticlSeedingTrk) {
            if (s.index == seed) {
              seedGlobalPos = math::XYZPoint(s.origin.x(),
                  s.origin.y(), s.origin.z());
              seedDirectionPos = math::XYZPoint(s.directionAtOrigin.x(),
                  s.directionAtOrigin.y(), s.directionAtOrigin.z());
              break;
            }
          }
        }

        auto alpha = (outer_inner_pos-seedGlobalPos).Dot(outer_outer_pos - outer_inner_pos)/
          sqrt((outer_inner_pos-seedGlobalPos).Mag2()*(outer_outer_pos-outer_inner_pos).Mag2());
        auto alpha_alternative = (outer_outer_pos - seedGlobalPos).Dot(seedDirectionPos) /
          sqrt((outer_outer_pos - seedGlobalPos).Mag2()*seedDirectionPos.Mag2());
        histo.angle_alpha_->Fill(alpha);
        histo.angle_alpha_alternative_->Fill(alpha_alternative);

        // Beta angle is usually computed using 2 edges. Another inner loop
        // is therefore needed.
        std::vector<std::array<unsigned int, 2>> innerDoublets;
        std::vector<std::array<unsigned int, 2>> outerDoublets;
        for ( const auto & otherEdge : thisTrackster.edges()) {
          if (otherEdge[1] == edge[0]) {
            innerDoublets.push_back(otherEdge);
          }
          if (edge[1] == otherEdge[0]) {
            outerDoublets.push_back(otherEdge);
          }
        }

        histo.ingoing_links_vs_layer_->Fill(layer_in, innerDoublets.size());
        histo.outgoing_links_vs_layer_->Fill(layer_out, outerDoublets.size());
        for (const auto & inner : innerDoublets) {
          const auto & inner_ic = layerClusters[inner[0]];
          const auto & inner_inner_pos = inner_ic.position();
          auto beta = (outer_inner_pos-inner_inner_pos).Dot(outer_outer_pos-inner_inner_pos)/
            sqrt((outer_inner_pos-inner_inner_pos).Mag2()*(outer_outer_pos-inner_inner_pos).Mag2());
          histo.angle_beta_->Fill(beta);
          histo.angle_beta_byLayer_[layer_in]->Fill(beta);
          histo.angle_beta_w_byLayer_[layer_in]->Fill(beta, ic.energy());
        }
      }
    }
  }
}

void TICLTrackstersValidation::bookHistograms(DQMStore::IBooker& ibook,
                                              edm::Run const& run,
                                              edm::EventSetup const& iSetup,
                                              Histograms_TICLTrackstersValidation& histos) const {
  int labelIndex = 0;
  for (const auto& trackster_token : tracksterTokens_) {
    auto& histo = histos[trackster_token.index()];
    ibook.setCurrentFolder(folder_ + "TICLTracksters/" + trackstersCollectionsNames_[labelIndex]);
    histo.number_ = ibook.book1D("Number of Trackster per Event", "Number of Trackster per Event", 250, 0., 250.);
    histo.energy_ = ibook.book1D("Regressed Energy", "Energy", 250, 0., 250.);
    histo.delta_energy_ = ibook.book1D("Delta energy", "Delta Energy (O-I)", 800, -20., 20.);
    histo.delta_energy_relative_ = ibook.book1D("Relative Delta energy", "Relative Delta Energy (O-I)/I", 200, -10., 10.);
    histo.delta_energy_vs_energy_ = ibook.book2D("Energy vs Delta Energy", "Energy (I) vs Delta Energy (O-I)", 800, -20., 20., 200, 0., 20.);
    histo.delta_energy_vs_layer_ = ibook.book2D("Delta Energy (O-I) vs Layer Number (I)", "Delta Energy (O-I) vs Layer Number (I)", 50, 0., 50., 800, -20., 20.);
    histo.delta_energy_relative_vs_layer_ = ibook.book2D("Relative Delta Energy (O-I)_I vs Layer Number (I)", "Relative Delta Energy (O-I)_I vs Layer Number (I)", 50, 0., 50., 200, -10., 10.);
    histo.ingoing_links_vs_layer_ = ibook.book2D("Ingoing links Layer Number", "Ingoing links vs Layer Number", 50, 0., 50., 40, 0., 40.);
    histo.outgoing_links_vs_layer_ = ibook.book2D("Outgoing links vs Layer Number", "Outgoing links vs Layer Number", 50, 0., 50., 40, 0., 40.);
    histo.delta_layer_ = ibook.book1D("Delta Layer", "Delta Layer", 10, 0., 10.);
    histo.angle_alpha_ = ibook.book1D("cosAngle Alpha", "cosAngle Alpha", 200, -1., 1.);
    histo.angle_beta_ = ibook.book1D("cosAngle Beta", "cosAngle Beta", 200, -1., 1.);
    histo.angle_alpha_alternative_ = ibook.book1D("cosAngle Alpha Alternative", "Angle Alpha Alternative", 200, 0., 1.);
    for (int layer = 0; layer < 50; layer++) {
      auto layerstr = std::to_string(layer+1);
      if (layerstr.length() < 2)
        layerstr.insert(0, 2 - layerstr.length(), '0');
      histo.angle_beta_byLayer_.push_back(ibook.book1D("cosAngle Beta on Layer " + layerstr,
            "cosAngle Beta on Layer " + layerstr, 200, -1., 1.));
      histo.angle_beta_w_byLayer_.push_back(ibook.book1D("cosAngle Beta Weighted on Layer " + layerstr,
            "cosAngle Beta Weighted on Layer " + layerstr, 200, -1., 1.));
    }
    labelIndex++;
  }
}

void TICLTrackstersValidation::dqmBeginRun(edm::Run const& run,
                                           edm::EventSetup const& iSetup,
                                           Histograms_TICLTrackstersValidation& histograms) const {
    edm::ESHandle<CaloGeometry> geom; iSetup.get<CaloGeometryRecord>().get(geom); rhtools_.setGeometry(*geom);
}

void TICLTrackstersValidation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  std::vector<edm::InputTag> source_vector{edm::InputTag("ticlTrackstersTrk"),
                                           edm::InputTag("ticlTrackstersTrkEM"),
                                           edm::InputTag("ticlTrackstersEM"),
                                           edm::InputTag("ticlTrackstersHAD"),
                                           edm::InputTag("ticlTrackstersDummy"),
                                           edm::InputTag("ticlTrackstersMerge")};
  desc.add<std::vector<edm::InputTag>>("tracksterCollections", source_vector);
  desc.add<edm::InputTag>("layerClusters", edm::InputTag("hgcalLayerClusters"));
  desc.add<edm::InputTag>("ticlSeedingGlobal", edm::InputTag("ticlSeedingGlobal"));
  desc.add<edm::InputTag>("ticlSeedingTrk", edm::InputTag("ticlSeedingTrk"));
  desc.add<std::string>("folder", "HGCAL/");
  descriptions.add("ticlTrackstersValidationDefault", desc);
}

DEFINE_FWK_MODULE(TICLTrackstersValidation);
