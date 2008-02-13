/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
///////////////////////////////////////////////////////////////////////////
//  Plane Efficiency class for ITS                      
//  It is used for chip by chip efficiency (eventually with sui-bwing division 
//  along the drift direction) of the SDD,        
//  evaluated by tracks
//  (Inherits from AliITSPlaneEff)
//  Author: G.E. Bruno 
//          giuseppe.bruno@ba.infn.it
//
///////////////////////////////////////////////////////////////////////////

/* $Id$ */

#include <TMath.h>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include "AliITSPlaneEffSDD.h"
#include "AliLog.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
//#include "AliCDBRunRange.h"
#include "AliITSgeom.h"
#include "AliITSCalibrationSDD.h"
#include "AliITSsegmentationSDD.h"

ClassImp(AliITSPlaneEffSDD)	
//______________________________________________________________________
AliITSPlaneEffSDD::AliITSPlaneEffSDD():
  AliITSPlaneEff(),
  fHisResX(0),
  fHisResZ(0),
  fHisResXZ(0),
  fHisClusterSize(0),
  fHisResXclu(0),
  fHisResZclu(0),
  fProfResXvsX(0),
  fProfResZvsX(0),
  fProfClustSizeXvsX(0),
  fProfClustSizeZvsX(0){
  for (UInt_t i=0; i<kNModule*kNChip*kNWing*kNSubWing; i++){
    fFound[i]=0;
    fTried[i]=0;
  }
  // default constructor
  AliDebug(1,Form("Calling default constructor"));
}
//______________________________________________________________________
AliITSPlaneEffSDD::~AliITSPlaneEffSDD(){
    // destructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //     none.
    DeleteHistos();
}
//______________________________________________________________________
AliITSPlaneEffSDD::AliITSPlaneEffSDD(const AliITSPlaneEffSDD &s) : AliITSPlaneEff(s),
//fHis(s.fHis),
fHisResX(0),
fHisResZ(0),
fHisResXZ(0),
fHisClusterSize(0),
fHisResXclu(0),
fHisResZclu(0),
fProfResXvsX(0),
fProfResZvsX(0),
fProfClustSizeXvsX(0),
fProfClustSizeZvsX(0)
{
    //     Copy Constructor
    // Inputs:
    //    AliITSPlaneEffSDD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:

 for (UInt_t i=0; i<kNModule*kNChip*kNWing*kNSubWing; i++){
    fFound[i]=s.fFound[i];
    fTried[i]=s.fTried[i];
 }
 if(fHis) {
   InitHistos();
   for(Int_t i=0; i<kNHisto; i++) {
      s.fHisResX[i]->Copy(*fHisResX[i]);
      s.fHisResZ[i]->Copy(*fHisResZ[i]);
      s.fHisResXZ[i]->Copy(*fHisResXZ[i]);
      s.fHisClusterSize[i]->Copy(*fHisClusterSize[i]);
      for(Int_t clu=0; clu<kNclu; clu++) {  // clu=0 --> cluster size 1
        s.fHisResXclu[i][clu]->Copy(*fHisResXclu[i][clu]);
        s.fHisResZclu[i][clu]->Copy(*fHisResZclu[i][clu]);
      }
     s.fProfResXvsX[i]->Copy(*fProfResXvsX[i]);
     s.fProfResZvsX[i]->Copy(*fProfResZvsX[i]);
     s.fProfClustSizeXvsX[i]->Copy(*fProfClustSizeXvsX[i]);
     s.fProfClustSizeZvsX[i]->Copy(*fProfClustSizeZvsX[i]);
   }
 }
}
//_________________________________________________________________________
AliITSPlaneEffSDD& AliITSPlaneEffSDD::operator+=(const AliITSPlaneEffSDD &add){
    //    Add-to-me operator
    // Inputs:
    //    const AliITSPlaneEffSDD &add  simulation class to be added
    // Outputs:
    //    none.
    // Return:
    //    none
    for (UInt_t i=0; i<kNModule*kNChip*kNWing*kNSubWing; i++){
      fFound[i] += add.fFound[i];
      fTried[i] += add.fTried[i];
    }
    if(fHis && add.fHis) {
      for(Int_t i=0; i<kNHisto; i++) {
        fHisResX[i]->Add(add.fHisResX[i]);
        fHisResZ[i]->Add(add.fHisResZ[i]);
        fHisResXZ[i]->Add(add.fHisResXZ[i]);
        fHisClusterSize[i]->Add(add.fHisClusterSize[i]);
        for(Int_t clu=0; clu<kNclu; clu++) {  // clu=0 --> cluster size 1
          fHisResXclu[i][clu]->Add(add.fHisResXclu[i][clu]);
          fHisResZclu[i][clu]->Add(add.fHisResZclu[i][clu]);
        }
       fProfResXvsX[i]->Add(add.fProfResXvsX[i]);
       fProfResZvsX[i]->Add(add.fProfResZvsX[i]);
       fProfClustSizeXvsX[i]->Add(add.fProfClustSizeXvsX[i]);
       fProfClustSizeZvsX[i]->Add(add.fProfClustSizeZvsX[i]);
      }
    }
    return *this;
}
//______________________________________________________________________
AliITSPlaneEffSDD&  AliITSPlaneEffSDD::operator=(const
                                           AliITSPlaneEffSDD &s){
    //    Assignment operator
    // Inputs:
    //    AliITSPlaneEffSDD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:
 
    if(this==&s) return *this;
    s.Copy(*this);
    return *this;
}
//______________________________________________________________________
void AliITSPlaneEffSDD::Copy(TObject &obj) const {
  // protected method. copy this to obj
  AliITSPlaneEff::Copy(obj);
  AliITSPlaneEffSDD& target = (AliITSPlaneEffSDD &) obj;
  for(Int_t i=0;i<kNModule*kNChip*kNWing*kNSubWing;i++) {
      target.fFound[i] = fFound[i];
      target.fTried[i] = fTried[i];
  }
  CopyHistos(target);
  return;
}
//_______________________________________________________________________
void AliITSPlaneEffSDD::CopyHistos(AliITSPlaneEffSDD &target) const {
  // protected method: copy histos from this to target
  target.fHis  = fHis; // this is redundant only in some cases. Leave as it is.
  if(fHis) {
    target.fHisResX=new TH1F*[kNHisto];
    target.fHisResZ=new TH1F*[kNHisto];
    target.fHisResXZ=new TH2F*[kNHisto];
    target.fHisClusterSize=new TH2I*[kNHisto];
    target.fHisResXclu=new TH1F**[kNHisto];
    target.fHisResZclu=new TH1F**[kNHisto];
    target.fProfResXvsX=new TProfile*[kNHisto];
    target.fProfResZvsX=new TProfile*[kNHisto];
    target.fProfClustSizeXvsX=new TProfile*[kNHisto];
    target.fProfClustSizeZvsX=new TProfile*[kNHisto];

    for(Int_t i=0; i<kNHisto; i++) {
      target.fHisResX[i] = new TH1F(*fHisResX[i]);
      target.fHisResZ[i] = new TH1F(*fHisResZ[i]);
      target.fHisResXZ[i] = new TH2F(*fHisResXZ[i]);
      target.fHisClusterSize[i] = new TH2I(*fHisClusterSize[i]);
      target.fHisResXclu[i]=new TH1F*[kNclu];
      target.fHisResZclu[i]=new TH1F*[kNclu];
      for(Int_t clu=0; clu<kNclu; clu++) {  // clu=0 --> cluster size 1
        target.fHisResXclu[i][clu] = new TH1F(*fHisResXclu[i][clu]);
        target.fHisResZclu[i][clu] = new TH1F(*fHisResZclu[i][clu]);
      }
      target.fProfResXvsX[i]=new TProfile(*fProfResXvsX[i]);
      target.fProfResZvsX[i]=new TProfile(*fProfResZvsX[i]);
      target.fProfClustSizeXvsX[i]=new TProfile(*fProfClustSizeXvsX[i]);
      target.fProfClustSizeZvsX[i]=new TProfile(*fProfClustSizeZvsX[i]);
    }
  }
return;
}
//______________________________________________________________________
AliITSPlaneEff&  AliITSPlaneEffSDD::operator=(const
                                           AliITSPlaneEff &s){
    //    Assignment operator
    // Inputs:
    //    AliITSPlaneEffSDD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:

    if(&s == this) return *this;
    AliError("operator=: Not allowed to make a =, use default creater instead");
    return *this;
}
//_______________________________________________________________________
Int_t AliITSPlaneEffSDD::GetMissingTracksForGivenEff(Double_t eff, Double_t RelErr,
          UInt_t im, UInt_t ic, UInt_t iw, UInt_t is) const {
   
  //   Estimate the number of tracks still to be collected to attain a 
  //   given efficiency eff, with relative error RelErr
  //   Inputs:
  //         eff    -> Expected efficiency (e.g. those from actual estimate)
  //         RelErr -> tollerance [0,1] 
  //         im     -> module number [0,259]
  //         ic     -> chip number [0,3]
  //         iw     -> wing number [0,1]
  //         is     -> chip number [0,kNSubWing-1]
  //   Outputs: none
  //   Return: the estimated n. of tracks 
  //
if (im>=kNModule || ic>=kNChip || iw>=kNWing || is>=kNSubWing) 
 {AliError("GetMissingTracksForGivenEff: you asked for a non existing block");
 return -1;}
else return GetNTracksForGivenEff(eff,RelErr)-fTried[GetKey(im,ic,iw,is)];
}
//_________________________________________________________________________
Double_t  AliITSPlaneEffSDD::PlaneEff(const UInt_t im,const UInt_t ic,
                                      const UInt_t iw,const UInt_t is) const {
// Compute the efficiency for a basic block, 
// Inputs:
//        im     -> module number [0,259]
//        ic     -> chip number [0,3]
//        iw     -> wing number [0,1]
//        is     -> chip number [0,kNSubWing-1]
if (im>=kNModule || ic>=kNChip || iw>=kNWing || is>=kNSubWing) 
 {AliError("PlaneEff(UInt_t,UInt_t,UInt_t,UInt_t): you asked for a non existing block"); return -1.;}
 Int_t nf=fFound[GetKey(im,ic,iw,is)];
 Int_t nt=fTried[GetKey(im,ic,iw,is)];
return AliITSPlaneEff::PlaneEff(nf,nt);
}
//_________________________________________________________________________
Double_t  AliITSPlaneEffSDD::ErrPlaneEff(const UInt_t im,const UInt_t ic,
                                         const UInt_t iw,const UInt_t is) const {
    // Compute the statistical error on efficiency for a basic block,
    // using binomial statistics 
    // Inputs:
    //        im     -> module number [0,259]
    //        ic     -> chip number [0,3]
    //        iw     -> wing number [0,1]
    //        is     -> chip number [0,kNSubWing-1]
if (im>=kNModule || ic>=kNChip || iw>=kNWing || is>=kNSubWing) 
 {AliError("ErrPlaneEff(UInt_t,UInt_t,UInt_t,UInt_t): you asked for a non existing block"); return -1.;}
Int_t nf=fFound[GetKey(im,ic,iw,is)];
Int_t nt=fTried[GetKey(im,ic,iw,is)];
return AliITSPlaneEff::ErrPlaneEff(nf,nt);
} 
//_________________________________________________________________________
Bool_t AliITSPlaneEffSDD::UpDatePlaneEff(const Bool_t Kfound,
                                         const UInt_t im, const UInt_t ic,
                                         const UInt_t iw, const UInt_t is) {
  // Update efficiency for a basic block
if (im>=kNModule || ic>=kNChip || iw>=kNWing || is>=kNSubWing) 
 {AliError("UpDatePlaneEff: you asked for a non existing block"); return kFALSE;}
 fTried[GetKey(im,ic,iw,is)]++;
 if(Kfound) fFound[GetKey(im,ic,iw,is)]++;
 return kTRUE;
}
//_________________________________________________________________________
void AliITSPlaneEffSDD::ChipAndWingFromAnode(const UInt_t anode, UInt_t& chip,
                                      UInt_t& wing) const {
  // Retun the chip number [0,3] and the wing number [0,1] given the anode number
  // input: anode number [0,511]
if(anode>=kNAnode*kNChip*kNWing)
 {AliError("ChipAndWingFromAnode: you asked for a non existing anode"); 
  chip=999;
  wing=99;
  return;}
wing=0;
chip=anode/kNAnode;
if(anode>=kNChip*kNAnode) wing=1;
if(wing==1) chip-=kNChip;
return;
}
//_________________________________________________________________________
UInt_t AliITSPlaneEffSDD::ChipFromAnode(const UInt_t anode) const {
  // Retun the chip number [0,3] given the anode number 
  // input: anode number [0,511]
if(anode>=kNAnode*kNChip*kNWing) 
 {AliError("ChipFromAnode: you asked for a non existing anode"); return 999;}
Int_t wing=0;
Int_t chip=anode/kNAnode;
if(anode>=kNChip*kNAnode) wing=1;
if(wing==1)chip-=kNChip;
return chip;
}
//_________________________________________________________________________
UInt_t AliITSPlaneEffSDD::WingFromAnode(const UInt_t anode) const {
  // return the wing number [0,1] given the anode number 
  // input: anode number [0,511]
if(anode>=kNAnode*kNChip*kNWing) 
 {AliError("WingFromAnode: you asked for a non existing anode"); return 99;}
Int_t wing=0;
if(anode>=kNChip*kNAnode) wing=1;
return wing;
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetKey(const UInt_t mod, const UInt_t chip,
                                 const UInt_t wing, const UInt_t subw) const {
  // get key given a basic block
if(mod>=kNModule || chip>=kNChip || wing>= kNWing || subw>=kNSubWing)
  {AliError("GetKey: you asked for a non existing block"); return 99999;}
return mod*kNChip*kNWing*kNSubWing+chip*kNWing*kNSubWing+wing*kNSubWing+subw;
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetModFromKey(const UInt_t key) const {
  // get mod. from key
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetModFromKey: you asked for a non existing key"); return 9999;}
return key/(kNChip*kNWing*kNSubWing);
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetChipFromKey(const UInt_t key) const {
  // retrieves chip from key
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetChipFromKey: you asked for a non existing key"); return 999;}
return (key%(kNChip*kNWing*kNSubWing))/(kNWing*kNSubWing);
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetWingFromKey(const UInt_t key) const {
  // retrieves wing from key
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetWingFromKey: you asked for a non existing key"); return 99;}
return ((key%(kNChip*kNWing*kNSubWing))%(kNWing*kNSubWing))/(kNSubWing);
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetSubWingFromKey(const UInt_t key) const {
  // retrieves sub-wing from key
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetSubWingFromKey: you asked for a non existing key"); return 9;}
return ((key%(kNChip*kNWing*kNSubWing))%(kNWing*kNSubWing))%(kNSubWing);
}
//__________________________________________________________________________
void AliITSPlaneEffSDD::GetAllFromKey(const UInt_t key,UInt_t& mod,UInt_t& chip,
                                      UInt_t& wing,UInt_t& subw) const {
  // get module, chip, wing and subwing from a key
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetAllFromKey: you asked for a non existing key"); 
  mod=9999;
  chip=999;
  wing=99;
  subw=9;
  return;}
mod=GetModFromKey(key);
chip=GetChipFromKey(key);
wing=GetWingFromKey(key);
subw=GetSubWingFromKey(key);
return;
}
//____________________________________________________________________________
Double_t AliITSPlaneEffSDD::LivePlaneEff(UInt_t key) const {
  // returns plane efficieny after adding the fraction of sensor which is bad
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("LivePlaneEff: you asked for a non existing key");
   return -1.;}
Double_t leff=AliITSPlaneEff::LivePlaneEff(0); // this just for the Warning
leff=PlaneEff(key)+GetFracBad(key);
return leff>1?1:leff;
}
//____________________________________________________________________________
Double_t AliITSPlaneEffSDD::ErrLivePlaneEff(UInt_t key) const {
  // returns error on live plane efficiency
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("ErrLivePlaneEff: you asked for a non existing key");
   return -1.;}
Int_t nf=fFound[key];
Double_t triedInLive=GetFracLive(key)*fTried[key];
Int_t nt=TMath::Max(nf,TMath::Nint(triedInLive));
return AliITSPlaneEff::ErrPlaneEff(nf,nt); // for the time being: to be checked
}
//_____________________________________________________________________________
Double_t AliITSPlaneEffSDD::GetFracLive(const UInt_t key) const {
  // returns the fraction of the sensor which is OK
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetFracLive: you asked for a non existing key");
   return -1.;}
    // Compute the fraction of bad (dead+noisy) detector 
UInt_t bad=0;
GetBadInBlock(key,bad);
Double_t live=bad;
live/=(kNAnode);
return 1.-live;
}
//_____________________________________________________________________________
void AliITSPlaneEffSDD::GetBadInBlock(const UInt_t key, UInt_t& nrBadInBlock) const {
  // Compute the number of bad (dead+noisy) anodes inside a block
  // (it depends on the chip, not on the sub-wing)
nrBadInBlock=0;
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetBadInBlock: you asked for a non existing key");
   return;}
//
if(!fInitCDBCalled) 
  {AliError("GetBadInBlock: CDB not inizialized: call InitCDB first");
   return;};
AliCDBManager* man = AliCDBManager::Instance();
// retrieve map of dead Pixel 
AliCDBEntry *cdbSDD = man->Get("ITS/Calib/CalibSDD", fRunNumber);
TObjArray* sddEntry;
if(cdbSDD) {
  sddEntry = (TObjArray*)cdbSDD->GetObject();
  if(!sddEntry) 
  {AliError("GetBadInBlock: SDDEntry not found in CDB");
   return;}
} else {
  AliError("GetBadInBlock: Did not find Calib/CalibSDD");
  return;
}
//
UInt_t mod=GetModFromKey(key);
UInt_t chip=GetChipFromKey(key);
UInt_t wing=GetWingFromKey(key);
// count number of dead
AliITSCalibrationSDD* calibSDD=(AliITSCalibrationSDD*) sddEntry->At(mod);
UInt_t nrBad = calibSDD-> GetDeadChannels();
for (UInt_t index=0; index<nrBad; index++) {
  if(ChipFromAnode(calibSDD->GetBadChannel(index))==chip &&
     WingFromAnode(calibSDD->GetBadChannel(index))==wing    ) nrBadInBlock++;
}
return;
}
//_____________________________________________________________________________
Double_t AliITSPlaneEffSDD::GetFracBad(const UInt_t key) const {
  // returns 1-fractional live
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliError("GetFracBad: you asked for a non existing key");
   return -1.;}
return 1.-GetFracLive(key);
}
//_____________________________________________________________________________
Bool_t AliITSPlaneEffSDD::WriteIntoCDB() const {
// write onto CDB
if(!fInitCDBCalled)
  {AliError("WriteIntoCDB: CDB not inizialized: call InitCDB first");
   return kFALSE;}
// to be written properly: now only for debugging 
  AliCDBMetaData *md= new AliCDBMetaData(); // metaData describing the object
  md->SetObjectClassName("AliITSPlaneEff");
  md->SetResponsible("Giuseppe Eugenio Bruno");
  md->SetBeamPeriod(0);
  md->SetAliRootVersion("head 02/01/08"); //root version
  AliCDBId id("ITS/PlaneEff/PlaneEffSDD",0,AliCDBRunRange::Infinity()); 
  AliITSPlaneEffSDD eff; 
  eff=*this;
  Bool_t r=AliCDBManager::Instance()->GetDefaultStorage()->Put(&eff,id,md);
  delete md;
  return r;
}
//_____________________________________________________________________________
Bool_t AliITSPlaneEffSDD::ReadFromCDB() {
// read from CDB
if(!fInitCDBCalled)
  {AliError("ReadFromCDB: CDB not inizialized: call InitCDB first");
   return kFALSE;}
//if(!AliCDBManager::Instance()->IsDefaultStorageSet()) {
//    AliCDBManager::Instance()->SetDefaultStorage("local://$ALICE_ROOT");
//  }
AliCDBEntry *cdbEntry = AliCDBManager::Instance()->Get("ITS/PlaneEff/PlaneEffSDD",fRunNumber);
AliITSPlaneEffSDD* eff= (AliITSPlaneEffSDD*)cdbEntry->GetObject();
if(this==eff) return kFALSE;
if(fHis) CopyHistos(*eff); // If histos already exist then copy them to eff
eff->Copy(*this);          // copy everything (statistics and histos) from eff to this
return kTRUE;
}
//_____________________________________________________________________________
UInt_t AliITSPlaneEffSDD::GetKeyFromDetLocCoord(Int_t ilay, Int_t idet,
                                                Float_t locx, Float_t locz) const {
// method to locate a basic block from Detector Local coordinate (to be used in tracking)
//
// If kNSubWing = 1, i.e. no sub-wing subdivision, then the numbering scheme of the 
// unique key is the following, e.g. for the first detector (idet=0,  ilayer=2)
//
//			      ^x_loc (cm)
//			      |
//   _________________________|__________________________ 3.5085
//  |            |            |            |            |
//  |            |            |            |            |
//  |            |            |            |            |
//  |   key=1    |   key=3    |   key=5    |   key=7    |
//  |            |            |            |            |
//  |____________|____________|____________|____________|_0_____\  local z (cm)
//  |            |            |            |            |       /
//  |            |            |            |            |
//  |   key=0    |   key=2    |   key=4    |   key=6    |
//  |            |            |            |            |
//  |            |            |            |            |
//  |____________|____________|____________|____________| -3.5085
//-3.7632     -1.8816         0          1.1186      3.7632 
//
// for the second detector (idet=2, ilay=2), first key is 8 (bottom-left), 
// last one is 15 (upper-right), and so on. 
//
// If subwing division has been applied, then you count in each wing, starting from 
// the one with negative local ,  from the anode side (outer part) towards the 
// cathod strip (center). 
//                    first column: 
//                      bottom wing   (from below): 0,1,..,kNSubWing-1, 
//                      upper wing    (from up):    kNSubWing, ... , 2*kNSubWing-1
//                    2nd column  
//                      bottom wing   (from below): 2*kNSubWing, .. , 3*kNSubWing-1
//                      upper wing    (from up):    3*kNSubWing, ... ,4*kNSubWing-1
//                      ... 
//                    4nd (last) column :   
//                      bottom wing   (from below): 6*kNSubWing, .. , 7*kNSubWing-1
//                      upper wing    (from up):    7*kNSubWing, ... ,8*kNSubWing-1
//
// E.g. kNSubWing=2.
//
//			      ^x_loc (cm)
//		              |   
//   _________________________|__________________________ 3.5085
//  |            |            |            |            |
//  |   key=2    |   key=6    |   key=10   |   key=14   |
//  |____________|____________|____________|____________|
//  |            |            |            |            |
//  |   key=3    |   key=7    |   key=11   |   key=15   |
//  |____________|____________|____________|____________|_0_____\  local z (cm)
//  |            |            |            |            |       /
//  |   key=1    |   key=5    |   key=9    |   key=13   |
//  |____________|____________|____________|____________|
//  |            |            |            |            |
//  |   key=0    |   key=4    |   key=8    |   key=12   |
//  |____________|____________|____________|____________| -3.5085
//-3.7632     -1.8816         0          1.1186      3.7632 
//
//___________________________________________________________________________
//
UInt_t key=999999;
if(ilay<2 || ilay>3)
  {AliError("GetKeyFromDetLocCoord: you asked for a non existing layer");
   return key;}
if(ilay==2 && (idet<0 || idet>83))
 {AliError("GetKeyFromDetLocCoord: you asked for a non existing detector");
   return key;}
if(ilay==3 && (idet<0 || idet>175))
 {AliError("GetKeyFromDetLocCoord: you asked for a non existing detector");
   return key;}
UInt_t mod=idet;
if(ilay==3) mod+=84;
UInt_t chip=0,wing=0,subw=0;
ChipAndWingAndSubWingFromLocCoor(locx,locz,chip,wing,subw);
key=GetKey(mod,chip,wing,subw);
return key;
}
//_____________________________________________________________________________
void AliITSPlaneEffSDD::ChipAndWingAndSubWingFromLocCoor(Float_t xloc, Float_t zloc, 
                                UInt_t& chip, UInt_t& wing, UInt_t& subw) const {
AliITSgeom* geom=NULL;
//AliITSsegmentationSDD* sdd=new AliITSsegmentationSDD(geom);
AliITSsegmentationSDD sdd=AliITSsegmentationSDD(geom);
sdd.SetDriftSpeed(sdd.GetDriftSpeed()); // this only for setting fSetDriftSpeed=kTRUE !!!
Int_t ix,iz;
Int_t ntb;
UInt_t anode=0;
if(sdd.LocalToDet(xloc,zloc,ix,iz)) {  
  anode+=iz; 
  ChipAndWingFromAnode(anode,chip,wing);
  if(sdd.LocalToDet(0.,0.,ntb,iz)) {  // in this way the sub-division along time coordinate
    subw=SubWingFromTimeBin(ix,ntb); }  // is purely geometrical one and it does not 
  else {				// depen on the drift-velocity. 
    AliError("ChipAndWingAndSubWingFromLocCoor: cannot calculate n. of time bins for SubWing.");
    subw=9;
  }
} else {
  AliError("ChipAndWingAndSubWingFromLocCoor: cannot calculate anode number and time bin."); 
  chip=999; 
  wing=99; 
  subw=9;
}
delete geom;
}
//__________________________________________________________________________________
UInt_t AliITSPlaneEffSDD::SubWingFromTimeBin(const Int_t tb, const Int_t ntb) const {
if(tb<0 || tb>ntb || ntb<0) {
 AliError(Form("SubWingFromTimeBin: you asked time bin = %d with %d n. of bins",tb,ntb));
 return 9;
}
//AliDebug(Form("tb = %d, ntb= %d , NSubWing = %d",tb,ntb,kNSubWing));
Float_t h=tb;
 h/=ntb;
 h*=(kNSubWing-1);
return TMath::Nint(h);
}
//________________________________________________________
Bool_t AliITSPlaneEffSDD::GetBlockBoundaries(const UInt_t key, Float_t& xmn,Float_t& xmx,
                                             Float_t& zmn,Float_t& zmx) const {
//
//  This method return the geometrical boundaries of the active volume of a given
//  basic block, in the detector reference system.
//  Input: unique key to locate a basic block.
//
//  Output: Ymin, Ymax, Zmin, Zmax of a basic block (chip for SPD)
//  Return: kTRUE if computation was succesfully, kFALSE otherwise
//
// the following scheemes will help in following the method implementation
// E.g. kNSubWing=1
//                            ^x_loc (cm)
//     for all: subw=0        |
//   _________________________|__________________________ 3.5085
//  |   wing=1   |   wing=1   |   wing=1   |   wing=1   |
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   |
//  |   key=1    |   key=3    |   key=5    |   key=7    |
//  |____________|____________|____________|____________|_0_____\  local z (cm)
//  |   wing=0   |   wing=0   |   wing=0   |   wing=0   |       /
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   |
//  |   key=0    |   key=2    |   key=4    |   key=6    |
//  |____________|____________|____________|____________| -3.5085
//-3.7632     -1.8816         0          1.1186      3.7632
//
// E.g. kNSubWing=2
//                            ^x_loc (cm)
//                            |
//   _________________________|__________________________ 3.5085
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   |
//  |   key=2    |   key=6    |   key=10   |   key=14   | subw=0
//  |____________|____________|____________|____________|        wing=1
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   | subw=1
//  |   key=3    |   key=7    |   key=11   |   key=15   |
//  |____________|____________|____________|____________|_0________\  local z (cm)
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   |          /
//  |   key=1    |   key=5    |   key=9    |   key=13   | subw=1
//  |____________|____________|____________|____________|        wing=0
//  |   chip=0   |   chip=1   |   chip=2   |   chip=3   | subw=0  
//  |   key=0    |   key=4    |   key=8    |   key=12   | 
//  |____________|____________|____________|____________| -3.5085
//-3.7632     -1.8816         0          1.1186      3.7632
//
if(key>=kNModule*kNChip*kNWing*kNSubWing)
  {AliWarning("GetBlockBoundaries: you asked for a non existing key"); return kFALSE;}
//
const Float_t kDxDefault = 35085.; // For Plane Eff. purpouses, default values
const Float_t kDzDefault = 75264.; // are precise enough !!!
const Float_t kconv = 1.0E-04;  //converts microns to cm.
UInt_t chip=GetChipFromKey(key);
UInt_t wing=GetWingFromKey(key);
UInt_t subw=GetSubWingFromKey(key);
zmn=kconv*(kDzDefault/kNChip*chip-0.5*kDzDefault);
zmx=kconv*(kDzDefault/kNChip*(chip+1)-0.5*kDzDefault);
if(wing==0) { // count from below
xmn=kconv*(kDxDefault/kNSubWing*subw-kDxDefault);
xmx=kconv*(kDxDefault/kNSubWing*(subw+1)-kDxDefault);
}
else if(wing==1) { // count from top
xmx=kconv*(kDxDefault-kDxDefault/kNSubWing*subw);
xmn=kconv*(kDxDefault-kDxDefault/kNSubWing*(subw+1));
}
else {AliError("GetBlockBoundaries: you got wrong n. of wing"); return kFALSE;}
return kTRUE;
}
//__________________________________________________________
void AliITSPlaneEffSDD::InitHistos() {
  // for the moment let's create the histograms 
  // module by  module
  TString histnameResX="HistResX_mod_",aux;
  TString histnameResZ="HistResZ_mod_";
  TString histnameResXZ="HistResXZ_mod_";
  TString histnameClusterType="HistClusterType_mod_";
  TString histnameResXclu="HistResX_mod_";
  TString histnameResZclu="HistResZ_mod_";
  TString profnameResXvsX="ProfResXvsX_mod_";
  TString profnameResZvsX="ProfResZvsX_mod_";
  TString profnameClustSizeXvsX="ProfClustSizeXvsX_mod_";
  TString profnameClustSizeZvsX="ProfClustSizeZvsX_mod_";
//
  fHisResX=new TH1F*[kNHisto];
  fHisResZ=new TH1F*[kNHisto];
  fHisResXZ=new TH2F*[kNHisto];
  fHisClusterSize=new TH2I*[kNHisto];
  fHisResXclu=new TH1F**[kNHisto];
  fHisResZclu=new TH1F**[kNHisto];
  fProfResXvsX=new TProfile*[kNHisto];
  fProfResZvsX=new TProfile*[kNHisto];
  fProfClustSizeXvsX=new TProfile*[kNHisto];
  fProfClustSizeZvsX=new TProfile*[kNHisto];

  for (Int_t nhist=0;nhist<kNHisto;nhist++){
    aux=histnameResX;
    aux+=nhist;
    fHisResX[nhist]=new TH1F("histname","histname",1500,-0.15,0.15); // +- 1500 micron; 1 bin=2 micron
    fHisResX[nhist]->SetName(aux.Data());
    fHisResX[nhist]->SetTitle(aux.Data());

    aux=histnameResZ;
    aux+=nhist;
    fHisResZ[nhist]=new TH1F("histname","histname",500,-0.05,0.05); // +-500 micron; 1 bin=2 micron
    fHisResZ[nhist]->SetName(aux.Data());
    fHisResZ[nhist]->SetTitle(aux.Data());

    aux=histnameResXZ;
    aux+=nhist;
    fHisResXZ[nhist]=new TH2F("histname","histname",50,-0.1,0.1,30,-0.03,0.03); // binning:
                                                                                   // 40 micron in x; 
                                                                                   // 20 micron in z; 
    fHisResXZ[nhist]->SetName(aux.Data());
    fHisResXZ[nhist]->SetTitle(aux.Data());

    aux=histnameClusterType;
    aux+=nhist;
    fHisClusterSize[nhist]=new TH2I("histname","histname",10,0.5,10.5,10,0.5,10.5);
    fHisClusterSize[nhist]->SetName(aux.Data());
    fHisClusterSize[nhist]->SetTitle(aux.Data());

    fHisResXclu[nhist]=new TH1F*[kNclu];
    fHisResZclu[nhist]=new TH1F*[kNclu];
    for(Int_t clu=0; clu<kNclu; clu++) {  // clu=0 --> cluster size 1
      aux=histnameResXclu;
      aux+=nhist;
      aux+="_clu_";
      aux+=clu+1; // clu=0 --> cluster size 1
      fHisResXclu[nhist][clu]=new TH1F("histname","histname",1500,-0.15,0.15);// +- 1500 micron; 1 bin=2 micron
      fHisResXclu[nhist][clu]->SetName(aux.Data());
      fHisResXclu[nhist][clu]->SetTitle(aux.Data());

      aux=histnameResZclu;
      aux+=nhist;
      aux+="_clu_";
      aux+=clu+1; // clu=0 --> cluster size 1
      fHisResZclu[nhist][clu]=new TH1F("histname","histname",500,-0.05,0.05); // +-500 micron; 1 bin=2 micron
      fHisResZclu[nhist][clu]->SetName(aux.Data());
      fHisResZclu[nhist][clu]->SetTitle(aux.Data());
    }

    aux=profnameResXvsX;
    aux+=nhist;
    fProfResXvsX[nhist]=new TProfile("histname","histname",140,-3.5,-3.5);
    fProfResXvsX[nhist]->SetName(aux.Data());
    fProfResXvsX[nhist]->SetTitle(aux.Data());

    aux=profnameResZvsX;
    aux+=nhist;
    fProfResZvsX[nhist]=new TProfile("histname","histname",140,-3.5,-3.5);
    fProfResZvsX[nhist]->SetName(aux.Data());
    fProfResZvsX[nhist]->SetTitle(aux.Data());

    aux=profnameClustSizeXvsX;
    aux+=nhist;
    fProfClustSizeXvsX[nhist]=new TProfile("histname","histname",140,-3.5,-3.5);
    fProfClustSizeXvsX[nhist]->SetName(aux.Data());
    fProfClustSizeXvsX[nhist]->SetTitle(aux.Data());

    aux=profnameClustSizeZvsX;
    aux+=nhist;
    fProfClustSizeZvsX[nhist]=new TProfile("histname","histname",140,-3.5,-3.5);
    fProfClustSizeZvsX[nhist]->SetName(aux.Data());
    fProfClustSizeZvsX[nhist]->SetTitle(aux.Data());

  }
return;
}
//__________________________________________________________
void AliITSPlaneEffSDD::DeleteHistos() {
  if(fHisResX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResX[i];
    delete [] fHisResX; fHisResX=0;
  }
  if(fHisResZ) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResZ[i];
    delete [] fHisResZ; fHisResZ=0;
  }
  if(fHisResXZ) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResXZ[i];
    delete [] fHisResXZ; fHisResXZ=0;
  }
  if(fHisClusterSize) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisClusterSize[i];
    delete [] fHisClusterSize; fHisClusterSize=0;
  }
  if(fHisResXclu) {
    for (Int_t i=0; i<kNHisto; i++ ) {
      for (Int_t clu=0; clu<kNclu; clu++) if (fHisResXclu[i][clu]) delete fHisResXclu[i][clu];
      delete [] fHisResXclu[i];
    }
    delete [] fHisResXclu;
    fHisResXclu = 0;
  }
  if(fHisResZclu) {
    for (Int_t i=0; i<kNHisto; i++ ) {
      for (Int_t clu=0; clu<kNclu; clu++) if (fHisResZclu[i][clu]) delete fHisResZclu[i][clu];
      delete [] fHisResZclu[i];
    }
    delete [] fHisResZclu;
    fHisResZclu = 0;
  }
  if(fProfResXvsX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fProfResXvsX[i];
    delete [] fProfResXvsX; fProfResXvsX=0;
  }
  if(fProfResZvsX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fProfResZvsX[i];
    delete [] fProfResZvsX; fProfResZvsX=0;
  }
  if(fProfClustSizeXvsX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fProfClustSizeXvsX[i];
    delete [] fProfClustSizeXvsX; fProfClustSizeXvsX=0;
  }
  if(fProfClustSizeZvsX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fProfClustSizeZvsX[i];
    delete [] fProfClustSizeZvsX; fProfClustSizeZvsX=0;
  }

return;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSDD::FillHistos(UInt_t key, Bool_t found, 
                                     Float_t tXZ[2], Float_t cXZ[2], Int_t ctXZ[2]) {
// this method fill the histograms
// input: - key: unique key of the basic block 
//        - found: Boolean to asses whether a cluster has been associated to the track or not 
//        - tXZ[2] local X and Z coordinates of the track prediction
//        - cXZ[2] local X and Z coordinates of the cluster associated to the track
//        - ctXZ[2] cluster size in X and Z (min =1 for real cluster)
// output: kTRUE if filling was succesfull kFALSE otherwise
// side effects: updating of the histograms. 
//
  if (!fHis) {
    AliWarning("FillHistos: histograms do not exist! Call SetCreateHistos(kTRUE) first");
    return kFALSE;
  }
  if(key>=kNModule*kNChip*kNWing*kNSubWing)
    {AliWarning("FillHistos: you asked for a non existing key"); return kFALSE;}
  Int_t id=GetModFromKey(key);
  if(id>=kNHisto) 
    {AliWarning("FillHistos: you want to fill a non-existing histos"); return kFALSE;}
  if(found) {
    Float_t resx=tXZ[0]-cXZ[0];
    Float_t resz=tXZ[1]-cXZ[1];
    fHisResX[id]->Fill(resx);
    fHisResZ[id]->Fill(resz);
    fHisResXZ[id]->Fill(resx,resz);
    fHisClusterSize[id]->Fill((Double_t)ctXZ[0],(Double_t)ctXZ[1]);
    if(ctXZ[0]>0 &&  ctXZ[0]<=kNclu) fHisResXclu[id][ctXZ[0]-1]->Fill(resx);
    if(ctXZ[1]>0 &&  ctXZ[1]<=kNclu) fHisResZclu[id][ctXZ[1]-1]->Fill(resx);
    fProfResXvsX[id]->Fill(cXZ[0],resx);
    fProfResZvsX[id]->Fill(cXZ[0],resz);
    fProfClustSizeXvsX[id]->Fill(cXZ[0],(Double_t)ctXZ[0]);
    fProfClustSizeZvsX[id]->Fill(cXZ[0],(Double_t)ctXZ[1]);
  }
  return kTRUE;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSDD::WriteHistosToFile(TString filename, Option_t* option) {
  //
  // Saves the histograms into a tree and saves the trees into a file
  //
  if (!fHis) return kFALSE;
  if (filename.Data()=="") {
     AliWarning("WriteHistosToFile: null output filename!");
     return kFALSE;
  }
  char branchname[30];
  TFile *hFile=new TFile(filename.Data(),option,
                         "The File containing the TREEs with ITS PlaneEff Histos");
  TTree *SDDTree=new TTree("SDDTree","Tree whith Residuals and Cluster Type distributions for SDD");
  TH1F *histZ,*histX;
  TH2F *histXZ;
  TH2I *histClusterType;
  TH1F *histXclu[kNclu];
  TH1F *histZclu[kNclu];
  TProfile *profileResXvsX, *profileResZvsX, *profileClSizXvsX, *profileClSizZvsX;

  histZ=new TH1F();
  histX=new TH1F();
  histXZ=new TH2F();
  histClusterType=new TH2I();
  for(Int_t clu=0;clu<kNclu;clu++) {
    histXclu[clu]=new TH1F();
    histZclu[clu]=new TH1F();
  }
  profileResXvsX=new TProfile();
  profileResZvsX=new TProfile();
  profileClSizXvsX=new TProfile();
  profileClSizZvsX=new TProfile();

  SDDTree->Branch("histX","TH1F",&histX,128000,0);
  SDDTree->Branch("histZ","TH1F",&histZ,128000,0);
  SDDTree->Branch("histXZ","TH2F",&histXZ,128000,0);
  SDDTree->Branch("histClusterType","TH2I",&histClusterType,128000,0);
  for(Int_t clu=0;clu<kNclu;clu++) {
    sprintf(branchname,"histXclu_%d",clu+1);
    SDDTree->Branch(branchname,"TH1F",&histXclu[clu],128000,0);
    sprintf(branchname,"histZclu_%d",clu+1);
    SDDTree->Branch(branchname,"TH1F",&histZclu[clu],128000,0);
  }
  SDDTree->Branch("profileResXvsX","TProfile",&profileResXvsX,128000,0);
  SDDTree->Branch("profileResZvsX","TProfile",&profileResZvsX,128000,0);
  SDDTree->Branch("profileClSizXvsX","TProfile",&profileClSizXvsX,128000,0);
  SDDTree->Branch("profileClSizZvsX","TProfile",&profileClSizZvsX,128000,0);

  for(Int_t j=0;j<kNHisto;j++){
    histX=fHisResX[j];
    histZ=fHisResZ[j];
    histXZ=fHisResXZ[j];
    histClusterType=fHisClusterSize[j];
    for(Int_t clu=0;clu<kNclu;clu++) {
      histXclu[clu]=fHisResXclu[j][clu];
      histZclu[clu]=fHisResZclu[j][clu];
    }
    profileResXvsX=fProfResXvsX[j];
    profileResZvsX=fProfResZvsX[j];
    profileClSizXvsX=fProfClustSizeXvsX[j];
    profileClSizZvsX=fProfClustSizeZvsX[j];

    SDDTree->Fill();
  }
  hFile->Write();
  hFile->Close();
return kTRUE;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSDD::ReadHistosFromFile(TString filename) {
  //
  // Read histograms from an already existing file 
  //
  if (!fHis) return kFALSE;
  if (filename.Data()=="") {
     AliWarning("ReadHistosFromFile: incorrect output filename!");
     return kFALSE;
  }
  char branchname[30];

  TH1F *h  = 0;
  TH2F *h2 = 0;
  TH2I *h2i= 0;
  TProfile *p = 0;

  TFile *file=TFile::Open(filename.Data(),"READONLY");

  if (!file || file->IsZombie()) {
    AliWarning(Form("Can't open %s !",filename.Data()));
    delete file;
    return kFALSE;
  }
  TTree *tree = (TTree*) file->Get("SDDTree");

  TBranch *histX = (TBranch*) tree->GetBranch("histX");
  TBranch *histZ = (TBranch*) tree->GetBranch("histZ");
  TBranch *histXZ = (TBranch*) tree->GetBranch("histXZ");
  TBranch *histClusterType = (TBranch*) tree->GetBranch("histClusterType");
   
  TBranch *histXclu[kNclu], *histZclu[kNclu];
  for(Int_t clu=0; clu<kNclu; clu++) {
    sprintf(branchname,"histXclu_%d",clu+1);
    histXclu[clu]= (TBranch*) tree->GetBranch(branchname);
    sprintf(branchname,"histZclu_%d",clu+1);
    histZclu[clu]= (TBranch*) tree->GetBranch(branchname);
  }
  TBranch *profileResXvsX = (TBranch*) tree->GetBranch("profileResXvsX");
  TBranch *profileResZvsX = (TBranch*) tree->GetBranch("profileResZvsX");
  TBranch *profileClSizXvsX = (TBranch*) tree->GetBranch("profileClSizXvsX");
  TBranch *profileClSizZvsX = (TBranch*) tree->GetBranch("profileClSizZvsX");

  gROOT->cd();

  Int_t nevent = (Int_t)histX->GetEntries();
  if(nevent!=kNHisto) 
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histX->SetAddress(&h);
  for(Int_t j=0;j<kNHisto;j++){
    delete h; h=0;
    histX->GetEntry(j);
    fHisResX[j]->Add(h);
  }

  nevent = (Int_t)histZ->GetEntries();
  if(nevent!=kNHisto) 
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histZ->SetAddress(&h);
  for(Int_t j=0;j<kNHisto;j++){
    delete h; h=0;
    histZ->GetEntry(j);
    fHisResZ[j]->Add(h);
  }

  nevent = (Int_t)histXZ->GetEntries();
  if(nevent!=kNHisto) 
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histXZ->SetAddress(&h2);
  for(Int_t j=0;j<kNHisto;j++){
    delete h2; h2=0;
    histXZ->GetEntry(j);
    fHisResXZ[j]->Add(h2);
  }

  nevent = (Int_t)histClusterType->GetEntries();
  if(nevent!=kNHisto) 
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histClusterType->SetAddress(&h2i);
  for(Int_t j=0;j<kNHisto;j++){
    delete h2i; h2i=0;
    histClusterType->GetEntry(j);
    fHisClusterSize[j]->Add(h2i);
  }

  for(Int_t clu=0; clu<kNclu; clu++) {

    nevent = (Int_t)histXclu[clu]->GetEntries();
    if(nevent!=kNHisto)
      {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
    histXclu[clu]->SetAddress(&h);
    for(Int_t j=0;j<kNHisto;j++){
      delete h; h=0;
      histXclu[clu]->GetEntry(j);
      fHisResXclu[j][clu]->Add(h);
    }

   nevent = (Int_t)histZclu[clu]->GetEntries();
    if(nevent!=kNHisto)
      {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
    histZclu[clu]->SetAddress(&h);
    for(Int_t j=0;j<kNHisto;j++){
      delete h; h=0;
      histZclu[clu]->GetEntry(j);
      fHisResZclu[j][clu]->Add(h);
    }
  }

  nevent = (Int_t)profileResXvsX->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  profileResXvsX->SetAddress(&p);
  for(Int_t j=0;j<kNHisto;j++){
    delete p; p=0;
    profileResXvsX->GetEntry(j);
    fProfResXvsX[j]->Add(p);
  }

  nevent = (Int_t)profileResZvsX->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  profileResZvsX->SetAddress(&p);
  for(Int_t j=0;j<kNHisto;j++){
    delete p; p=0;
    profileResZvsX->GetEntry(j);
    fProfResZvsX[j]->Add(p);
  }

  nevent = (Int_t)profileClSizXvsX->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  profileClSizXvsX->SetAddress(&p);
  for(Int_t j=0;j<kNHisto;j++){
    delete p; p=0;
    profileClSizXvsX->GetEntry(j);
    fProfClustSizeXvsX[j]->Add(p);
  }

  nevent = (Int_t)profileClSizZvsX->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  profileClSizZvsX->SetAddress(&p);
  for(Int_t j=0;j<kNHisto;j++){
    delete p; p=0;
    profileClSizZvsX->GetEntry(j);
    fProfClustSizeZvsX[j]->Add(p);
  }

  delete h;   h=0;
  delete h2;  h2=0;
  delete h2i; h2i=0;
  delete p;   p=0;

  if (file) {
    file->Close();
  }
return kTRUE;
}

