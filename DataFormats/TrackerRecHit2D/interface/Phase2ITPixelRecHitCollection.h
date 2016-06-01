#ifndef DataFormats_Phase2ITPixelRecHitCollection_H
#define DataFormats_Phase2ITPixelRecHitCollection_H

#include "DataFormats/TrackerRecHit2D/interface/Phase2ITPixelRecHit.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

typedef edmNew::DetSetVector<Phase2ITPixelRecHit> Phase2ITPixelRecHitCollection;
typedef Phase2ITPixelRecHitCollection             Phase2ITPixelRecHitCollectionNew;

#endif


