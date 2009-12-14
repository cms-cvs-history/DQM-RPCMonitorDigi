import FWCore.ParameterSet.Config as cms

process = cms.Process("rpcDqmClient")

################# Input ########################
process.source = cms.Source("PoolSource",
    moduleLogName = cms.untracked.string('source'),
     fileNames = cms.untracked.vstring('/store/data/Commissioning08/Cosmics/RECO/v1/000/070/664/1CE1633D-87AF-DD11-AD95-000423D98B08.root')
)

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

################# Geometry  ######################
process.load("Geometry.MuonCommonData.muonIdealGeometryXML_cfi")
process.load("Geometry.RPCGeometry.rpcGeometry_cfi")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")
process.load("CondCore.DBCommon.CondDBSetup_cfi")

################# DQM Cetral Modules ###################
from DQMServices.Core.DQMStore_cfg import *

process.load("DQMServices.Components.DQMEnvironment_cfi")
process.dqmEnv.subSystemFolder = 'RPC'

#################### RPC Tier0 Source ####################
process.load("DQM.RPCMonitorClient.RPCTier0Source_cff")


################# Quality Tests #########################
process.qTesterRPC = cms.EDAnalyzer("QualityTester",
    qtList = cms.untracked.FileInPath('DQM/RPCMonitorClient/test/RPCQualityTests.xml'),
    prescaleFactor = cms.untracked.int32(1)
)

############ EDM to ME ####################
process.load("DQMServices.Components.MEtoEDMConverter_cff")


############# Message Logger ####################
process.MessageLogger = cms.Service("MessageLogger",
     debugModules = cms.untracked.vstring('rpcdqmclient'),
     destinations = cms.untracked.vstring('cout'),
     cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO'))
)

###############Timing #################################
#process.Timing = cms.Service('Timing')

## process.options = cms.untracked.PSet(
##          wantSummary = cms.untracked.bool(True)
##          )


################# Path ###########################
process.p = cms.Path(process.dqmEnv*process.qTesterRPC*process.rpcTier0Source*process.MEtoEDMConverter)


