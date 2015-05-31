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
  LogDebug("SiPixelStubBuilder") << "SiPixelStubBuilder::produce() begin";

  // get input clusters data
  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> >  clustersHandle;
  event.getByLabel( clusterProducer, clustersHandle);

  // create the final output collection
  std::auto_ptr< edmNew::DetSetVector< Phase2TrackerCluster1D > > outputClusterAccept( new edmNew::DetSetVector< Phase2TrackerCluster1D > );
  std::auto_ptr< VectorHitCollectionNew > outputVHAccepted( new VectorHitCollectionNew() );
  std::auto_ptr< VectorHitCollectionNew > outputVHRejected( new VectorHitCollectionNew() );

  if(readytobuild)  stubsBuilder->initialize(es);
  else edm::LogError("SiPixelStubBuilder") << "Impossible initialization of builder!!";

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

  //LogDebug("SiPixelStubBuilder") << "found\n" << outputVHAccepted->dataSize()   << "  stubs in mono detectors\n" ;

}

void SiPixelStubBuilder::setupAlgorithm(edm::ParameterSet const& conf) {

  if ( algoTag == "VectorHitBuilderAlgorithm" ) {
    stubsBuilder = new VectorHitBuilderAlgorithm(conf);
    readytobuild = true;
  } else {
    edm::LogError("SiPixelStubBuilder") << " Choice " << algoTag << " is invalid.\n" ;
    readytobuild = false;
  }

}


void SiPixelStubBuilder::run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters, VectorHitCollectionNew output ){

  if ( !readytobuild ) {
    edm::LogError("SiPixelStubBuilder") << " No stub builder algorithm was found - cannot run!" ;
    return;
  }

  output = stubsBuilder->run(clusters);

  //max number in total
  unsigned int numberOfStubs = output.size();

  if(numberOfStubs > maxOfflinestubs) {
    edm::LogError("SiPixelStubBuilder") <<  "Limit on the number of stubs exceeded. An empty output collection will be produced instead.\n";
    VectorHitCollectionNew empty;
    empty.swap(output);
  }

  LogDebug("SiPixelStubBuilder") << " Executing " << algoTag << " resulted in " << numberOfStubs << ".";


}
