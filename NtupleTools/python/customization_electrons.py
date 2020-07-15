# Embed IDs for electrons
import FWCore.ParameterSet.Config as cms
from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupAllVIDIdsInModule, setupVIDElectronSelection, switchOnVIDElectronIdProducer, DataFormat, setupVIDSelection
#from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights


def preElectrons(process, eSrc, vSrc, year, isEmbedded, **kwargs):
    postfix = kwargs.pop('postfix','')

    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    myera='2018-Prompt'
    if year=="2016":
       myera='2016-Legacy'
    if year=="2017":
       myera='2017-Nov17ReReco'

    setupEgammaPostRecoSeq(process,
		       eleIDModules=[
                        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff',
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V2_cff',
                        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V2_cff'
                        ],
                       era=myera) 

    pathName = 'miniAODElectrons{0}'.format(postfix)
    path=cms.Path(process.egammaPostRecoSeq)
    setattr(process,pathName,path)
    process.schedule.append(getattr(process,pathName))

    # Embed effective areas in muons and electrons
    if not hasattr(process,'patElectronEAEmbedder'):
        process.load("FinalStateAnalysis.PatTools.electrons.patElectronEAEmbedding_cfi")
    eaModName = 'patElectronEAEmbedder{0}'.format(postfix)
    if postfix:
        setattr(process,eaModName,process.patElectronEAEmbedder.clone())
    getattr(process,eaModName).src = cms.InputTag(eSrc)
    eSrc = eaModName
    eaFile = 'RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt'
    newEaModName = 'miniAODElectronEAEmbedding{0}'.format(postfix)
    newEaMod = cms.EDProducer(
        "MiniAODElectronEffectiveAreaEmbedder",
        src = cms.InputTag(eSrc),
        label = cms.string("EffectiveArea"), # embeds a user float with this name
	year = cms.string("year"),
        configFile = cms.FileInPath(eaFile), # the effective areas file
        )
    eSrc = newEaModName
    setattr(process,newEaModName,newEaMod)
    pathName = 'ElectronEAEmbedding{0}'.format(postfix)
    path = cms.Path(getattr(process,eaModName) + getattr(process,newEaModName))
    setattr(process,pathName,path)
    process.schedule.append(getattr(process,pathName))
    
    # Embed rhos in electrons
    modName = 'miniAODElectronRhoEmbedding{0}'.format(postfix)
    mod = cms.EDProducer(
        "ElectronRhoOverloader",
        src = cms.InputTag(eSrc),
        srcRho = cms.InputTag("fixedGridRhoFastjetAll"), # not sure this is right
        userLabel = cms.string("rho_fastjet")
        )
    eSrc = modName
    setattr(process,modName,mod)
    
    pathName = 'electronRhoEmbedding{0}'.format(postfix)
    path = cms.Path(getattr(process,modName))
    setattr(process,pathName,path)
    process.schedule.append(getattr(process,pathName))

    # embed trigger filters
    modName = 'minitriggerfilterElectrons{0}'.format(postfix)
    mod = cms.EDProducer(
        "MiniAODElectronTriggerFilterEmbedder",
        src=cms.InputTag(eSrc),
        bits = cms.InputTag("TriggerResults","","HLT"),
        objects = cms.InputTag("slimmedPatTrigger"),
        #bits = cms.InputTag("TriggerResults","","SIMembedding"),
        #objects = cms.InputTag("slimmedPatTrigger","","MERGE"),
    )
    if isEmbedded:
	mod.bits=cms.InputTag("TriggerResults","","SIMembedding")
	mod.objects=cms.InputTag("slimmedPatTrigger","","MERGE")
	if year=="2016": 
	    mod.objects=cms.InputTag("slimmedPatTrigger","","PAT")
    eSrc = modName
    setattr(process,modName,mod)

    pathName = 'runTriggerFilterElectronEmbedding{0}'.format(postfix)
    modPath = cms.Path(getattr(process,modName))
    setattr(process,pathName,modPath)
    process.schedule.append(getattr(process,pathName))

    return eSrc

def postElectrons(process, eSrc, jSrc,**kwargs):

    return eSrc

