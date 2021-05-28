#include <string>

#include "DQM/L1TMonitor/interface/L1TdeCSCTPG.h"

L1TdeCSCTPG::L1TdeCSCTPG(const edm::ParameterSet& ps)
    : dataALCT_token_(consumes<CSCALCTDigiCollection>(ps.getParameter<edm::InputTag>("dataALCT"))),
      emulALCT_token_(consumes<CSCALCTDigiCollection>(ps.getParameter<edm::InputTag>("emulALCT"))),
      dataCLCT_token_(consumes<CSCCLCTDigiCollection>(ps.getParameter<edm::InputTag>("dataCLCT"))),
      emulCLCT_token_(consumes<CSCCLCTDigiCollection>(ps.getParameter<edm::InputTag>("emulCLCT"))),
      dataLCT_token_(consumes<CSCCorrelatedLCTDigiCollection>(ps.getParameter<edm::InputTag>("dataLCT"))),
      emulLCT_token_(consumes<CSCCorrelatedLCTDigiCollection>(ps.getParameter<edm::InputTag>("emulLCT"))),
      monitorDir_(ps.getParameter<std::string>("monitorDir")),

      chambers_(ps.getParameter<std::vector<std::string>>("chambers")),
      dataEmul_(ps.getParameter<std::vector<std::string>>("dataEmul")),

      // variables
      alctVars_(ps.getParameter<std::vector<std::string>>("alctVars")),
      clctVars_(ps.getParameter<std::vector<std::string>>("clctVars")),
      lctVars_(ps.getParameter<std::vector<std::string>>("lctVars")),

      // binning
      alctNBin_(ps.getParameter<std::vector<unsigned>>("alctNBin")),
      clctNBin_(ps.getParameter<std::vector<unsigned>>("clctNBin")),
      lctNBin_(ps.getParameter<std::vector<unsigned>>("lctNBin")),
      alctMinBin_(ps.getParameter<std::vector<double>>("alctMinBin")),
      clctMinBin_(ps.getParameter<std::vector<double>>("clctMinBin")),
      lctMinBin_(ps.getParameter<std::vector<double>>("lctMinBin")),
      alctMaxBin_(ps.getParameter<std::vector<double>>("alctMaxBin")),
      clctMaxBin_(ps.getParameter<std::vector<double>>("clctMaxBin")),
      lctMaxBin_(ps.getParameter<std::vector<double>>("lctMaxBin")),
      b904Setup_(ps.getParameter<bool>("B904Setup")) {}

L1TdeCSCTPG::~L1TdeCSCTPG() {}

void L1TdeCSCTPG::bookHistograms(DQMStore::IBooker& iBooker, const edm::Run&, const edm::EventSetup&) {
  iBooker.setCurrentFolder(monitorDir_);

  // remove the non-ME1/1 chambers from the list when b904Setup is set to true
  if (b904Setup_) {
    chambers_.resize(1);
  }
  // do not analyze the 1/4-strip bit, 1/8-strip bit
  else {
    clctVars_.resize(9);
    lctVars_.resize(9);
  }

  // chamber type
  for (unsigned iType = 0; iType < chambers_.size(); iType++) {
    // data vs emulator
    for (unsigned iData = 0; iData < dataEmul_.size(); iData++) {
      // alct variable
      for (unsigned iVar = 0; iVar < alctVars_.size(); iVar++) {
        const std::string key("alct_" + alctVars_[iVar] + "_" + dataEmul_[iData]);
        const std::string histName(key + "_" + chambers_[iType]);
        const std::string histTitle(chambers_[iType] + " ALCT " + alctVars_[iVar] + " (" + dataEmul_[iData] + ") ");
        chamberHistos[iType][key] =
            iBooker.book1D(histName, histTitle, alctNBin_[iVar], alctMinBin_[iVar], alctMaxBin_[iVar]);
        chamberHistos[iType][key]->getTH1()->SetMinimum(0);
      }

      // clct variable
      for (unsigned iVar = 0; iVar < clctVars_.size(); iVar++) {
        const std::string key("clct_" + clctVars_[iVar] + "_" + dataEmul_[iData]);
        const std::string histName(key + "_" + chambers_[iType]);
        const std::string histTitle(chambers_[iType] + " CLCT " + clctVars_[iVar] + " (" + dataEmul_[iData] + ") ");
        chamberHistos[iType][key] =
            iBooker.book1D(histName, histTitle, clctNBin_[iVar], clctMinBin_[iVar], clctMaxBin_[iVar]);
        chamberHistos[iType][key]->getTH1()->SetMinimum(0);
      }

      // lct variable
      for (unsigned iVar = 0; iVar < lctVars_.size(); iVar++) {
        const std::string key("lct_" + lctVars_[iVar] + "_" + dataEmul_[iData]);
        const std::string histName(key + "_" + chambers_[iType]);
        const std::string histTitle(chambers_[iType] + " LCT " + lctVars_[iVar] + " (" + dataEmul_[iData] + ") ");
        chamberHistos[iType][key] =
            iBooker.book1D(histName, histTitle, lctNBin_[iVar], lctMinBin_[iVar], lctMaxBin_[iVar]);
        chamberHistos[iType][key]->getTH1()->SetMinimum(0);
      }
    }
  }
}

void L1TdeCSCTPG::analyze(const edm::Event& e, const edm::EventSetup& c) {
  // handles
  edm::Handle<CSCALCTDigiCollection> dataALCTs;
  edm::Handle<CSCALCTDigiCollection> emulALCTs;
  edm::Handle<CSCCLCTDigiCollection> dataCLCTs;
  edm::Handle<CSCCLCTDigiCollection> emulCLCTs;
  edm::Handle<CSCCorrelatedLCTDigiCollection> dataLCTs;
  edm::Handle<CSCCorrelatedLCTDigiCollection> emulLCTs;

  e.getByToken(dataALCT_token_, dataALCTs);
  e.getByToken(emulALCT_token_, emulALCTs);
  e.getByToken(dataCLCT_token_, dataCLCTs);
  e.getByToken(emulCLCT_token_, emulCLCTs);
  e.getByToken(dataLCT_token_, dataLCTs);
  e.getByToken(emulLCT_token_, emulLCTs);

  for (auto it = dataALCTs->begin(); it != dataALCTs->end(); it++) {
    auto range = dataALCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto alct = range.first; alct != range.second; alct++) {
      chamberHistos[type]["alct_quality_data"]->Fill(alct->getQuality());
      chamberHistos[type]["alct_wiregroup_data"]->Fill(alct->getKeyWG());
      chamberHistos[type]["alct_bx_data"]->Fill(alct->getBX());
    }
  }

  for (auto it = emulALCTs->begin(); it != emulALCTs->end(); it++) {
    auto range = emulALCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto alct = range.first; alct != range.second; alct++) {
      chamberHistos[type]["alct_quality_emul"]->Fill(alct->getQuality());
      chamberHistos[type]["alct_wiregroup_emul"]->Fill(alct->getKeyWG());
      chamberHistos[type]["alct_bx_emul"]->Fill(alct->getBX());
    }
  }

  for (auto it = dataCLCTs->begin(); it != dataCLCTs->end(); it++) {
    auto range = dataCLCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto clct = range.first; clct != range.second; clct++) {
      chamberHistos[type]["clct_pattern_data"]->Fill(clct->getPattern());
      chamberHistos[type]["clct_run3pattern_data"]->Fill(clct->getRun3Pattern());
      chamberHistos[type]["clct_quality_data"]->Fill(clct->getQuality());
      chamberHistos[type]["clct_halfstrip_data"]->Fill(clct->getKeyStrip());
      chamberHistos[type]["clct_quartstrip_data"]->Fill(clct->getKeyStrip(4));
      chamberHistos[type]["clct_eighthstrip_data"]->Fill(clct->getKeyStrip(8));
      chamberHistos[type]["clct_bend_data"]->Fill(clct->getBend());
      chamberHistos[type]["clct_slope_data"]->Fill(clct->getSlope());
      chamberHistos[type]["clct_compcode_data"]->Fill(clct->getCompCode());
      if (b904Setup_) {
        chamberHistos[type]["clct_quartstripbit_data"]->Fill(clct->getQuartStripBit());
        chamberHistos[type]["clct_eighthstripbit_data"]->Fill(clct->getEighthStripBit());
      }
    }
  }

  for (auto it = emulCLCTs->begin(); it != emulCLCTs->end(); it++) {
    auto range = emulCLCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto clct = range.first; clct != range.second; clct++) {
      chamberHistos[type]["clct_pattern_emul"]->Fill(clct->getPattern());
      chamberHistos[type]["clct_run3pattern_emul"]->Fill(clct->getRun3Pattern());
      chamberHistos[type]["clct_quality_emul"]->Fill(clct->getQuality());
      chamberHistos[type]["clct_halfstrip_emul"]->Fill(clct->getKeyStrip());
      chamberHistos[type]["clct_quartstrip_emul"]->Fill(clct->getKeyStrip(4));
      chamberHistos[type]["clct_eighthstrip_emul"]->Fill(clct->getKeyStrip(8));
      chamberHistos[type]["clct_bend_emul"]->Fill(clct->getBend());
      chamberHistos[type]["clct_slope_emul"]->Fill(clct->getSlope());
      chamberHistos[type]["clct_compcode_emul"]->Fill(clct->getCompCode());
      if (b904Setup_) {
        chamberHistos[type]["clct_quartstripbit_emul"]->Fill(clct->getQuartStripBit());
        chamberHistos[type]["clct_eighthstripbit_emul"]->Fill(clct->getEighthStripBit());
      }
    }
  }

  for (auto it = dataLCTs->begin(); it != dataLCTs->end(); it++) {
    auto range = dataLCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto lct = range.first; lct != range.second; lct++) {
      chamberHistos[type]["lct_pattern_data"]->Fill(lct->getCLCTPattern());
      chamberHistos[type]["lct_run3pattern_data"]->Fill(lct->getRun3Pattern());
      chamberHistos[type]["lct_quality_data"]->Fill(lct->getQuality());
      chamberHistos[type]["lct_wiregroup_data"]->Fill(lct->getKeyWG());
      chamberHistos[type]["lct_halfstrip_data"]->Fill(lct->getStrip());
      chamberHistos[type]["lct_bend_data"]->Fill(lct->getBend());
      chamberHistos[type]["lct_slope_data"]->Fill(lct->getSlope());
      chamberHistos[type]["lct_quartstrip_data"]->Fill(lct->getStrip(4));
      chamberHistos[type]["lct_eighthstrip_data"]->Fill(lct->getStrip(8));
      if (b904Setup_) {
        chamberHistos[type]["lct_quartstripbit_data"]->Fill(lct->getQuartStripBit());
        chamberHistos[type]["lct_eighthstripbit_data"]->Fill(lct->getEighthStripBit());
      }
    }
  }

  for (auto it = emulLCTs->begin(); it != emulLCTs->end(); it++) {
    auto range = emulLCTs->get((*it).first);
    const int type = ((*it).first).iChamberType() - 2;
    // ignore non-ME1/1 chambers when using B904 test-stand data
    if (b904Setup_ and !((*it).first).isME11())
      continue;
    for (auto lct = range.first; lct != range.second; lct++) {
      chamberHistos[type]["lct_pattern_emul"]->Fill(lct->getCLCTPattern());
      chamberHistos[type]["lct_run3pattern_emul"]->Fill(lct->getRun3Pattern());
      chamberHistos[type]["lct_quality_emul"]->Fill(lct->getQuality());
      chamberHistos[type]["lct_wiregroup_emul"]->Fill(lct->getKeyWG());
      chamberHistos[type]["lct_halfstrip_emul"]->Fill(lct->getStrip());
      chamberHistos[type]["lct_bend_emul"]->Fill(lct->getBend());
      chamberHistos[type]["lct_slope_emul"]->Fill(lct->getSlope());
      chamberHistos[type]["lct_quartstrip_emul"]->Fill(lct->getStrip(4));
      chamberHistos[type]["lct_eighthstrip_emul"]->Fill(lct->getStrip(8));
      if (b904Setup_) {
        chamberHistos[type]["lct_quartstripbit_emul"]->Fill(lct->getQuartStripBit());
        chamberHistos[type]["lct_eighthstripbit_emul"]->Fill(lct->getEighthStripBit());
      }
    }
  }
}
