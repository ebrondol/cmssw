import FWCore.ParameterSet.Config as cms

MRHKFUpdatorESProducer = cms.ESProducer("MRHKFUpdatorESProducer",
    ComponentName = cms.string('MRHKFUpdator'),
    simpleUpdator = cms.string('KFUpdator')
)


