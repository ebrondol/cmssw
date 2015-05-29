#include "RecoLocalTracker/SiPixelStubBuilder/plugins/SiPixelStubBuilder.h"
#include "RecoLocalTracker/SiPixelStubBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

SiPixelStubBuilder::SiPixelStubBuilder(edm::ParameterSet const& conf)
  : offlinestubsTag( conf.getParameter<std::string>( "offlinestubs" ) ),
    maxOfflinestubs(conf.getParameter<int>( "maxStubs" )),
    algoTag(conf.getParameter<std::string>( "Algorithm" )),
    clusterProducer(conf.getParameter<edm::InputTag>("Clusters")),
    readytobuild(false)
{
  produces< edmNew::DetSetVector< Phase2TrackerCluster1D > >( "ClusterAccepted" );
  produces< VectorHitCollectionNew >( offlinestubsTag + "Accepted" );
  produces< VectorHitCollectionNew >( offlinestubsTag + "Rejected" );
  setupAlgorithm(conf);
}

SiPixelStubBuilder::~SiPixelStubBuilder() {
  delete stubsBuilder;
}

void SiPixelStubBuilder::produce(edm::Event& event, const edm::EventSetup& es)
{
  std::cout << "SiPixelStubBuilder::produce() begin" << std::endl;

  // get input clusters data
  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> >  clustersHandle;
  event.getByLabel( clusterProducer, clustersHandle);

  // create the final output collection
  std::auto_ptr< edmNew::DetSetVector< Phase2TrackerCluster1D > > outputClusterAccept( new edmNew::DetSetVector< Phase2TrackerCluster1D > );
  std::auto_ptr< VectorHitCollectionNew > outputVHAccepted( new VectorHitCollectionNew() );
  std::auto_ptr< VectorHitCollectionNew > outputVHRejected( new VectorHitCollectionNew() );

  if(readytobuild)  stubsBuilder->initialize(es);
  else std::cout << "Impossible initialization of builder!!" << std::endl;

  // check on the input clusters
  stubsBuilder->printClusters(*clustersHandle);

  // running the stub building algorithm
  //ERICA::output should be moved in the different algo classes?
  run( *clustersHandle, *outputVHAccepted);//, *outputClusterAccept, *outputStubsRejected );

 // ERICA::check::Are the output clusters empty?
 // *outputClusterAccept = *ClustersHandle;
 // int numberOfDetUnits_produced = 0;
 // edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator ClusterIter_new;
 // for( ClusterIter_new = (*outputClusterAccept).begin() ; ClusterIter_new != (*outputClusterAccept).end(); ClusterIter_new++) {
 //   ++numberOfDetUnits_produced;
 // }


  // write output to file
  event.put( outputClusterAccept, "ClusterAccepted" );
  event.put( outputVHAccepted, offlinestubsTag + "Accepted" );
  event.put( outputVHRejected, offlinestubsTag + "Rejected" );

  //std::cout << "found\n" << outputVHAccepted->dataSize()   << "  stubs in mono detectors\n" ;

}

void SiPixelStubBuilder::setupAlgorithm(edm::ParameterSet const& conf) {

  if ( algoTag == "VectorHitBuilderAlgorithm" ) {
    stubsBuilder = new VectorHitBuilderAlgorithm(conf);
    readytobuild = true;
  } else {
    std::cout << " Choice " << algoTag << " is invalid.\n" ;
    readytobuild = false;
  }

}


void SiPixelStubBuilder::run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters,
  VectorHitCollectionNew output ){

  if ( !readytobuild ) {
    std::cout << " No stub builder algorithm was found - cannot run!" ;
    return;
  }

  output = stubsBuilder->run(clusters);
  //std::vector< std::pair< StackGeomDet, std::vector<Phase2TrackerCluster1D> > > groupClusterBySM;
  //groupClusterBySM = stubsBuilder->groupinginStackModules(clusters);
/*
    // Produce stubs for this DetUnit and store them in a DetSetVector
    edmNew::DetSetVector<SiPixelCluster>::FastFiller spc(output, DSViter->detId());
    _stubBuilder->clusterizeDetUnit(*DSViter, pixDet, badChannels, spc);
    if ( spc.empty() ) {
      spc.abort();
    } else {
numberOfClusters += spc.size();
    }
*/
  //ERICA::max number in total o per stack? Think about it.
  unsigned int numberOfStubs = output.size();

  if(numberOfStubs > maxOfflinestubs) {
    std::cout <<  "Limit on the number of stubs exceeded. An empty output collection will be produced instead.\n";
    VectorHitCollectionNew empty;
    empty.swap(output);
  }

  std::cout << " Executing " << algoTag << " resulted in " << numberOfStubs << "." << std::endl;


}
