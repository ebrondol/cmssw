import FWCore.ParameterSet.Config as cms
from RecoLuminosity.LumiProducer.lumiProducerFromLumiDB_cfi import *
LumiDBService=cms.Service('DBService')
lumiProducer.connect='frontier://LumiProd/CMS_LUMI_PROD'
lumiProducer.lumiversion='0001'
