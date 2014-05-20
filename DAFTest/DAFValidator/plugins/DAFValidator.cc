#include "DAFTest/DAFValidator/plugins/DAFValidator.h"

#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajAnnealing.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/ProjectedSiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TSiTrackerMultiRecHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
//#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

using namespace std;
using namespace edm;
typedef TransientTrackingRecHit::ConstRecHitPointer ConstRecHitPointer;


int event = 0;

DAFValidator::DAFValidator(const edm::ParameterSet& iConfig):
  theConf_(iConfig),
  tracksTag_(iConfig.getUntrackedParameter<edm::InputTag>("tracks")),
  trackingParticleTag_(iConfig.getParameter<edm::InputTag>("trackingParticleLabel")),
  associatorTag_(iConfig.getParameter<edm::InputTag>("associator"))
{
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  annealing_weight = fs->make<TH2F>("AnnWeight","Changing of weights as a funct of annealing", 1000, 0.0, 1.0, 90, 0, 90);
  annealing_weight_tgraph = fs->make<TGraph>();
 
  //histos 
  histo_maxweight = fs->make<TH1F>("Weight", "max weight of the mrh components ", 110, 0, 1.1);
  processtype_withassociatedsimhit_merged = fs->make<TH1F>("Process_Type_merged", "type of the process(merged_simhit)", 20, 0, 20);
  processtype_withassociatedsimhit = fs->make<TH1F>("Process_Type", "type of the process", 20, 0, 20);
  Hit_Histo = fs->make<TH1F>("Momentum_of_Hit", "momentum of the hit", 100, 0, 100);
  MergedHisto = fs->make<TH1F>("Histogram_Hit_Merged", "type of the hit", 5, 0, 5);
  NotMergedHisto = fs->make<TH1F>("Histogram_Hit_NotMerged", "type of the hit", 5, 0, 5);
  weight_vs_processtype_merged = fs->make<TH2F>("WeightVsProcessType", "weight vs proc type", 20, 0, 20,  110, 0, 1.1 );
  weight_vs_processtype_notmerged = fs->make<TH2F>("WeightVsProcessTypeNotMerged", "weight vs proc type not merged", 20, 0, 20,  110, 0, 1.1 );
  pull_vs_weight = fs->make<TH2F>("PullVsWeight", "pull vs weight", 100, 0, 20,  110, 0, 1.1 );
  Merged_vs_weight = fs->make<TH2F>("HitsMergedVsWeight", "hits vs weight", 5, 0, 5,  110, 0, 1.1 );
  NotMerged_vs_weight = fs->make<TH2F>("HitsNotMergedVsWeight", "hits vs weight", 5, 0, 5,  110, 0, 1.1 );
  NotMergedPos = fs->make<TH2F>("BadHitsPositionNotMerged", "badposition", 600, -30, 30,  2400, -1200, 1200);
  MergedPos = fs->make<TH2F>("BadHitsPositionMerged()", "badposition", 600, -30, 30,  2400, -1200, 1200);
  
}

DAFValidator::~DAFValidator()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

// ------------ method called for each event  ------------
void
DAFValidator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace reco;
  event++;
  cout << "Number of Event: " << event << endl;

  //ERICA version :: get the track collection
//  Handle<TrackCollection> trackCollection;
//  iEvent.getByLabel(tracksTag_, trackCollection);

  //get the track collection
  Handle<View<Track>>  trackCollection;
  iEvent.getByLabel(tracksTag_, trackCollection);

  //get the trajectory annealing collection
  Handle<TrajAnnealingCollection>  trajAnnCollection;
  iEvent.getByLabel(tracksTag_, trajAnnCollection);

  //get the association map between Traj and Track
  Handle<TrajTrackAssociationCollection> assoMap;
  iEvent.getByLabel(tracksTag_, assoMap);

  for(TrajAnnealingCollection::const_iterator iTA = trajAnnCollection->begin() ; 
      iTA != trajAnnCollection->end(); iTA++){
    //iTA->Debug();
    std::vector<float> Weights = iTA->weights();
    float ann;
    ann = iTA->getAnnealing();
    for(unsigned int i = 0; i < Weights.size(); i++, nHitsAnn++)
    {
      annealing_weight_tgraph->SetPoint(nHitsAnn,Weights.at(i),ann);

      annealing_weight->Fill(Weights.at(i),ann);
    }
  }


  //CMSSW_5_1_3
  //get tracker geometry
  ESHandle<TrackerGeometry> tkgeom;
  iSetup.get<TrackerDigiGeometryRecord>().get(tkgeom);

  //get the tracking particle collection
  //ERICA: file config deve contenerli!! Cercali o chiedi
  Handle<TrackingParticleCollection> trackingParticleCollection;
  iEvent.getByLabel(trackingParticleTag_, trackingParticleCollection);
  //  float SimTracknum=trackingParticleCollection->size();

  //hit associator :: CHECK IT
  TrackerHitAssociator hitAssociate(iEvent, theConf_);//.getParameter<ParameterSet>("HitAssociatorPSet"));

  //track associator (tracking particles to the reco track): first try
  ESHandle<TrackAssociatorBase> associatorHandle;
  iSetup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits",associatorHandle);
  //ERICA:: NEED Handle<View<reco::Track>> 
  reco::RecoToSimCollection RecsimColl = associatorHandle->associateRecoToSim(trackCollection, trackingParticleCollection, &iEvent, &iSetup);

  //track associator (tracking particles to the reco track): second try
  //ERICA: problem "trackingParticleRecoTrackAsssociation" module not found
  //Handle<reco::RecoToSimCollection> RecsimCollHandle;
  //iEvent.getByLabel("trackingParticleRecoTrackAsssociation", RecsimCollHandle);
  //reco::RecoToSimCollection RecsimColl = *RecsimCollHandle;


  //loop over the recotrack looking for corresponding trackingparticle
  int i = 0;
  int nSimu = 0;

  for(TrajTrackAssociationCollection::const_iterator it = assoMap->begin();it != assoMap->end(); ++it){

    std::map<reco::TrackRef,unsigned int> trackid;

    const edm::Ref<std::vector<Trajectory> > traj = it->key;

    const reco::TrackRef trackref = it->val;
    trackid.insert(make_pair(trackref,i));
    edm::RefToBase<reco::Track> track(trackCollection,i);
    i++;
    vector<pair<TrackingParticleRef, double> > simTracks;
    TrackingParticleRef matchedSimTrack;
    int nSim = 0;

    if(RecsimColl.find(track) != RecsimColl.end()){

      simTracks=RecsimColl[track];
      float fractionmax=0;
      for(vector<pair<TrackingParticleRef, double> >::const_iterator it = simTracks.begin(); it != simTracks.end(); ++it)
      {
          TrackingParticleRef simTrack = it->first;
          float fraction = it->second;

          //pick the trackingparticle with the highest fraction of hishared hits 
          if(fraction > fractionmax)
            {
              matchedSimTrack = simTrack;
              fractionmax=fraction;
              nSim++; }
      }

      analyzeHits(matchedSimTrack.get(), track.get(), hitAssociate, traj, tkgeom.product(), event);
      nSimu++;
      
    }
  }
}
//--------------------------------------------------------------------------------------
void DAFValidator::analyzeHits(const TrackingParticle* tpref,
                               const reco::Track* rtref,
                               TrackerHitAssociator& hitassociator,
                               const edm::Ref<std::vector<Trajectory> > traj_iterator,
                               const TrackerGeometry* geom,
                               int event) {


  if (!tpref || !rtref) {
    cout << "something wrong: tpref = " << tpref << " rtref = " << rtref << endl;
    return;
  }

  //loop over the reco track rec hits, associate the simhits
  trackingRecHit_iterator iter;
  std::vector<TrajectoryMeasurement> measurements =traj_iterator->measurements();
  std::vector<TrajectoryMeasurement>::iterator traj_mes_iterator;

  for(traj_mes_iterator=measurements.begin();traj_mes_iterator!=measurements.end();traj_mes_iterator++){
    const TrackingRecHit* ttrh=traj_mes_iterator->recHit()->hit();

    const SiTrackerMultiRecHit* mrh = dynamic_cast<const SiTrackerMultiRecHit*>(ttrh);
    const TrackingRecHit* rechit=0;
    float maxweight = 0;
    if (mrh){
      vector<const TrackingRecHit*> components = mrh->recHits();
      vector<const TrackingRecHit*>::const_iterator icomp;
      int hitcounter=0;
      for (icomp = components.begin(); icomp != components.end(); icomp++) {
          if((*icomp)->isValid())
            {
              cout << "weight: " << mrh->weight(hitcounter) << endl;
              //extract the hit with the max weight from the multirechit 
              weight = mrh->weight(hitcounter);
              if(weight > maxweight) {
                rechit=*icomp;
                maxweight=weight;
              }
            }

          hitcounter++;
        }
    }

    else{
      if(ttrh->isValid()){
        rechit=ttrh;
        maxweight=1;
      }
    }
std::cout << "rechit: " << rechit << " with Type: " << getType(rechit) <<std::endl; 
std::vector<const TrackingRecHit*> hits_prova = rechit->recHits();
std::cout << "rechit->recHits() components" << hits_prova.size() << std::endl;
     if(rechit){
 
      if (getType(rechit)==2.)
      {
          std::vector<const TrackingRecHit*> hits = rechit->recHits();

          for(std::vector<const TrackingRecHit*>::iterator iterhits=hits.begin();iterhits!=hits.end();iterhits++)
          {
              const TrackingRecHit* rechit1 = *iterhits;

              LocalPoint pos;
              if(rechit1->isValid()) pos=rechit1->localPosition();
              //unsigned int detid=rechit->geographicalId().rawId();

              TrajectoryStateOnSurface tsos = traj_mes_iterator->updatedState();

              AlgebraicVector tsospos(2);
              tsospos[0]=tsos.localPosition().x();
              tsospos[1]=tsos.localPosition().y();
 
              AlgebraicVector hitposition(2);
              hitposition[0]=pos.x();
              hitposition[1]=pos.y();

              AlgebraicVector tsoserr(3);
              tsoserr[0] = tsos.localError().positionError().xx();
              tsoserr[1] = tsos.localError().positionError().yy();
              tsoserr[2] = tsos.localError().positionError().xy();

              AlgebraicVector hiterr(3);
              hiterr[0] = rechit1->localPositionError().xx();
              hiterr[1] = rechit1->localPositionError().yy();
              hiterr[2] = rechit1->localPositionError().xy();
 
              tsoslocalx = tsospos[0];
              tsoslocaly = tsospos[1];
 
              hitlocalx = hitposition[0];
              hitlocaly = hitposition[1];
 
              tsoslocalsigmax = tsoserr[0];
              tsoslocalsigmay = tsoserr[1];
              tsoslocalcov = tsoserr[2];

              hitlocalsigmax = hiterr[0];
              hitlocalsigmay = hiterr[1];
              hitlocalcov = hiterr[2];

              nevent=event;
              weight=maxweight;
              GlobalPoint point=getGlobalPosition(rechit1,geom);
              r=point.perp();
              zeta=point.z();
              phi=point.phi();
              hittype=getType(rechit1);
              detId=rechit->geographicalId().rawId();

	      //ERICA :: fixed  hit associator !! 
              //do the association between rechits and simhits 
	      vector<PSimHit> matchedhits;
	      vector<SimHitIdpr> simhitids; 
              matchedhits = hitassociator.associateHit(*rechit1);
              simhitids = hitassociator.associateHitId(*rechit1);
              fillDAFHistos(matchedhits, maxweight, rechit1, geom);
              fillPHistos(matchedhits);
          
              if(matchedhits.size()!=1){
                notmergedtype=0;
                mergedtype=fillMergedHisto(simhitids,matchedhits,tpref,maxweight,geom);
             
              }

              else{
      
                mergedtype=0;
                notmergedtype=fillNotMergedHisto(simhitids,matchedhits,tpref,maxweight,geom);
                
              }

              mrhit->Fill();
            }
        }
 
      else {
        LocalPoint pos;
        if(rechit->isValid()) pos=rechit->localPosition();
        //unsigned int detid=rechit->geographicalId().rawId();

        TrajectoryStateOnSurface tsos = traj_mes_iterator->updatedState();

        AlgebraicVector tsospos(2);
        tsospos[0]=tsos.localPosition().x();
        tsospos[1]=tsos.localPosition().y();

        AlgebraicVector hitposition(2);
        hitposition[0]=pos.x();
        hitposition[1]=pos.y();

        AlgebraicVector tsoserr(3);
        tsoserr[0] = tsos.localError().positionError().xx();
        tsoserr[1] = tsos.localError().positionError().yy();
        tsoserr[2] = tsos.localError().positionError().xy();

        AlgebraicVector hiterr(3); 
        hiterr[0] = rechit->localPositionError().xx();
        hiterr[1] = rechit->localPositionError().yy();
        hiterr[2] = rechit->localPositionError().xy();
 
        tsoslocalx = tsospos[0];
        tsoslocaly = tsospos[1];

        hitlocalx = hitposition[0];
        hitlocaly = hitposition[1];

        tsoslocalsigmax = tsoserr[0];
        tsoslocalsigmay = tsoserr[1];
        tsoslocalcov = tsoserr[2]; 

        hitlocalsigmax = hiterr[0];
        hitlocalsigmay = hiterr[1];
        hitlocalcov = hiterr[2];

        nevent=event;
        weight=maxweight;
        GlobalPoint point=getGlobalPosition(rechit,geom);
        r=point.perp();
        zeta=point.z();
        phi=point.phi();
        hittype=getType(rechit);
        detId=rechit->geographicalId().rawId();

        //ERICA :: fixed  hit associator !! 
        //do the association between rechits and simhits 
        vector<PSimHit> matchedhits;
        vector<SimHitIdpr> simhitids;
        matchedhits = hitassociator.associateHit(*rechit);
        simhitids = hitassociator.associateHitId(*rechit);
        fillDAFHistos(matchedhits, maxweight, rechit, geom);
        fillPHistos(matchedhits);

        if(matchedhits.size()!=1){
          notmergedtype=0;
          mergedtype=fillMergedHisto(simhitids,matchedhits,tpref,maxweight,geom);
        }
 
        else{
          mergedtype=0;
          notmergedtype=fillNotMergedHisto(simhitids,matchedhits,tpref,maxweight,geom);
       }
 
        mrhit->Fill();
 
      }

    }

  }
 
}

// ------------ method called once each job just before starting event loop  ------------
void DAFValidator::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
void DAFValidator::endJob()
{
  
}

// ------------ method called when starting to processes a run  ------------
void DAFValidator::beginRun(edm::Run const&, edm::EventSetup const&)
{
  int bufsize = 64000;
  mrhit = new TTree("Ntuple","Ntuple");
  mrhit->Branch("mergedtype" , &mergedtype , "mergedtype/I" , bufsize);
  mrhit->Branch("notmergedtype" , &notmergedtype , "notmergedtype/I" , bufsize);
  mrhit->Branch("weight" , &weight , "weight/F" , bufsize);
  mrhit->Branch("detId" , &detId , "detId/I" , bufsize);
  mrhit->Branch("r" , &r , "r/F" , bufsize);
  mrhit->Branch("zeta" , &zeta , "zeta/F" , bufsize);
  mrhit->Branch("phi" , &phi , "phi/F" , bufsize);
  mrhit->Branch("hittyipe" , &hittype , "hittype/F" , bufsize);
  mrhit->Branch("event" , &nevent , "nevent/I" , bufsize);



  mrhit->Branch("hitlocalx" , &hitlocalx , "hitlocalx/F" , bufsize);
  mrhit->Branch("hitlocaly" , &hitlocaly , "hitlocaly/F" , bufsize);
  mrhit->Branch("hitlocalsigmax" , &hitlocalsigmax , "hitlocalsigmax/F" , bufsize);
  mrhit->Branch("hitlocalsigmay" , &hitlocalsigmay , "hitlocalsigmay/F" , bufsize);
  mrhit->Branch("hitlocalcov" , &hitlocalcov , "hitlocalcov/F" , bufsize);
  mrhit->Branch("tsoslocalx" , &tsoslocalx , "tsoslocalx/F" , bufsize);
  mrhit->Branch("tsoslocaly" , &tsoslocaly , "tsoslocaly/F" , bufsize);
  mrhit->Branch("tsoslocalsigmax" , &tsoslocalsigmax , "tsoslocalsigmax/F" , bufsize);
  mrhit->Branch("tsoslocalsigmay" , &tsoslocalsigmay , "tsoslocalsigmay/F" , bufsize);
  mrhit->Branch("tsoslocalcov" , &tsoslocalcov , "tsoslocalcov/F" , bufsize);
  mrhit->Branch("RecoTracknum" , &tsoslocalcov , "tsoslocalcov/F" , bufsize);
  mrhit->Branch("SimTracknum" , &tsoslocalcov , "tsoslocalcov/F" , bufsize);

  event=0;
  mergedtype=0;
  notmergedtype=0;
  nHitsAnn=0;

}


// ------------ method called when ending the processing of a run  ------------
void DAFValidator::endRun(edm::Run const&, edm::EventSetup const&)
{
  //drawing
  TCanvas* c = new TCanvas();
  c -> cd();
  c -> SetGridx();
  c -> SetGridy();
/*  annealing_weight -> GetXaxis() -> SetTitle("weights");
  annealing_weight -> GetYaxis() -> SetTitle("annealing value");
  annealing_weight -> GetXaxis() -> SetTitleSize(0.05);
  annealing_weight -> GetYaxis() -> SetTitleSize(0.05);
  annealing_weight -> GetXaxis() -> SetLabelSize(0.04);
  annealing_weight -> GetYaxis() -> SetLabelSize(0.04);
  annealing_weight->Draw("P");
*/
  annealing_weight_tgraph -> GetXaxis() -> SetTitle("weights");
  annealing_weight_tgraph -> GetYaxis() -> SetTitle("annealing value");
  annealing_weight_tgraph -> GetXaxis() -> SetTitleSize(0.04);
  annealing_weight_tgraph -> GetYaxis() -> SetTitleSize(0.04);
  annealing_weight_tgraph -> GetXaxis() -> SetLabelSize(0.03);
  annealing_weight_tgraph -> GetYaxis() -> SetLabelSize(0.03);
  annealing_weight_tgraph -> SetMarkerStyle(29);
//  annealing_weight_tgraph -> SetMarkerSize(2);
  annealing_weight_tgraph -> Draw("AP");
  c->Print("annweighttgraphPlot.pdf", "pdf");
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void DAFValidator::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}
//------------------------------------------------------------------------------------------------------
void DAFValidator::fillDAFHistos(std::vector<PSimHit>& matched,
                                 float weight,
                                 const TrackingRecHit* rechit,
                                 const TrackerGeometry* geom){
  //check the hit validity
  if (!matched.size()){
    edm::LogError("DAFValidator") << "fillDAFHistos: this multirechit has no corresponding simhits";
    return;
  }

  unsigned short ptype;
  
  if (matched.size()==1)
    {
      
      float pull=calculatepull(rechit, matched.front(), geom);
      pull_vs_weight->Fill(pull,weight);
     
      ptype = matched.front().processType();
      processtype_withassociatedsimhit->Fill(ptype);
      weight_vs_processtype_notmerged->Fill(ptype, weight);

    }
  
  else if (matched.size()>1)
    {
    
      for(vector<PSimHit>::iterator imatched=matched.begin(); imatched!=matched.end(); imatched++ )
        {
          float pull=calculatepull(rechit, (*imatched), geom);
          pull_vs_weight->Fill(pull,weight);
   
          ptype = imatched->processType();
          processtype_withassociatedsimhit_merged->Fill(ptype);
          weight_vs_processtype_merged->Fill(ptype, weight);
        }
    }
  
  histo_maxweight->Fill(weight);
  
}  
//------------------------------------------------------------------------------------------------------
float DAFValidator::calculatepull(const TrackingRecHit* hit,
                                  PSimHit simhit,
                                  const TrackerGeometry* geom){

  //perform the calculation of the pull
  AlgebraicVector reccoor(2);
  AlgebraicVector simcoor(2);
  AlgebraicVector diffcoor(2);

  //reccoor[0] = getGlobalPositionRec(hit, geom).x();
  //reccoor[1] = getGlobalPositionRec(hit, geom).y();

  //simcoor[0] = getGlobalPositionSim(simhit, geom).x();
  //simcoor[1] = getGlobalPositionSim(simhit, geom).y();

  reccoor[0] = hit->localPosition().x();
  reccoor[1] = hit->localPosition().y();

  simcoor[0] = simhit.localPosition().x();
  simcoor[1] = simhit.localPosition().y();

  diffcoor = reccoor-simcoor;
  float diff = sqrt(diffcoor[0]*diffcoor[0]+diffcoor[1]*diffcoor[1]);

  float sigma = sqrt((hit->localPositionError().xx() + hit->localPositionError().yy()+ hit->localPositionError().xy()));
  float pull = diff/sigma;

  return pull;
  //return diff;
}
//------------------------------------------------------------------------------------------------------
void DAFValidator::fillPHistos(std::vector<PSimHit>& components){
  //check the hit validity
  if (!components.size()){
    edm::LogError("DAFValidator") << "fillPHistos: this multirechit has no hits";
    return;
  }

  for(vector<PSimHit>::iterator icomp=components.begin(); icomp!=components.end(); icomp++ )
    {
      float pabs = icomp->pabs();
      Hit_Histo->Fill(pabs);
    }
}
//------------------------------------------------------------------------------------------------------
int DAFValidator::fillMergedHisto(const std::vector<SimHitIdpr>& simhitids, const std::vector<PSimHit>& simhits,
                                  const TrackingParticle* tpref, float weight, const TrackerGeometry* geom) const
{

  if (simhitids.empty()) {cout << "something wrong" << endl;}
  GlobalPoint point;
  //unsigned int simcount=0;
  for (TrackingParticle::g4t_iterator g4T = tpref -> g4Track_begin(); g4T !=  tpref -> g4Track_end(); ++g4T){
    //vector<SimHitIdpr>::const_iterator isimid;
    vector<PSimHit>::const_iterator isimid;
    //    unsigned int simcount=0;
    //for (isimid = simhitids.begin(); isimid != simhitids.end(); isimid++){
    //in case of merged hits we have to make a for cicle 
    for(isimid = simhits.begin(); isimid != simhits.end(); isimid++){

      if (((*g4T).trackId() == (*isimid).trackId()) || (isimid->processType() == 2)){
       return 2;
      }
      else continue;
    }
  }
  return 1;
}
//------------------------------------------------------------------------------------------------------
int DAFValidator::fillNotMergedHisto(const std::vector<SimHitIdpr>& simhitids, const std::vector<PSimHit>& simhits,
                                     const TrackingParticle* tpref, float weight, const TrackerGeometry* geom) const
{
  if (simhitids.empty()) {cout << "something wrong" << endl;}

  vector<PSimHit>::const_iterator isimid = simhits.begin();
  //int simcount=0; 
  if ( isimid->processType() == 2)
    {
      for (TrackingParticle::g4t_iterator g4T = tpref -> g4Track_begin(); g4T !=  tpref -> g4Track_end(); ++g4T){
        //vector<SimHitIdpr>::const_iterator isimid;
        //      int simcount=0;
        if ((*g4T).trackId()==isimid->trackId()) return 3;
      }
    }

  else {
    for (TrackingParticle::g4t_iterator g4T = tpref -> g4Track_begin(); g4T !=  tpref -> g4Track_end(); ++g4T){
      //vector<SimHitIdpr>::const_iterator isimid;
      //      int simcount=0;
      if ((*g4T).trackId()==isimid->trackId()) return 2;
    }
  }

  return 1;
}
//------------------------------------------------------------------------------------------------------


float DAFValidator::getType(const TrackingRecHit* hit)  const {
  if (!hit->isValid()){
    throw cms::Exception("DAFValidator") << "This hit is invalid, cannot be casted as any type of tracker hit (strip or pixel)! ";
  }
  const SiPixelRecHit* pixhit = dynamic_cast<const SiPixelRecHit*>(hit);
  const SiStripRecHit2D* stripmono = dynamic_cast<const SiStripRecHit2D*>(hit);
  const SiStripMatchedRecHit2D* stripmatched = dynamic_cast<const SiStripMatchedRecHit2D*>(hit);
  const ProjectedSiStripRecHit2D* stripprojected = dynamic_cast<const ProjectedSiStripRecHit2D*>(hit);
  if(pixhit) return 0.;
  else if (stripmono) return 1.;
  else if (stripmatched) return 2.;
  else if (stripprojected) return 3.;
  else throw cms::Exception("DAFValidator") << "Rec Hits of type " << typeid(*hit).name() << " should not be present at this stage ";
}
//---------------------------------------------------------------------------------------------------------
GlobalPoint DAFValidator::getGlobalPositionSim(const PSimHit hit, const TrackerGeometry* geom) const{

  cout << "detid" << hit << endl;
  DetId detid = DetId(hit.detUnitId());

  //const GeomDet* gdet = geom->idToDet(hit.detUnitId());
  const GeomDet* gdet = geom->idToDet(detid);
  GlobalPoint global = gdet->toGlobal(hit.localPosition());
  return global;
}
//---------------------------------------------------------------------------------------------------------
GlobalPoint DAFValidator::getGlobalPosition(const TrackingRecHit* hit, const TrackerGeometry* geom) const{
  const GeomDet* gdet = geom->idToDet(hit->geographicalId());
  GlobalPoint global = gdet->toGlobal(hit->localPosition());
  return global;
}

//---------------------------------------------------------------------------------------------------------
std::pair<float, std::vector<float> > DAFValidator::getAnnealingWeight( const TrackingRecHit& aRecHit ) const {

  if (!aRecHit.isValid()) {
    cout << "Invalid RecHit passed ";
  }
 
  TSiTrackerMultiRecHit const & mHit = dynamic_cast<TSiTrackerMultiRecHit const &>(aRecHit);  
//  float annealing = mHit.getAnnealingFactor();

/*  TransientTrackingRecHit::ConstRecHitContainer components = mHit.transientHits();
  for (TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter = components.begin(); iter != components.end(); iter++){              
    cout << "Hit with weight " << (*iter)->weight() << "\t"; 
  }     
*/
  return make_pair(mHit.getAnnealingFactor(), mHit.weights());

}
//
DEFINE_FWK_MODULE(DAFValidator);


