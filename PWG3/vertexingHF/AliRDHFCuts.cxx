
/**************************************************************************
 * Copyright(c) 1998-2010, ALICE Experiment at CERN, All rights reserved. *
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

/////////////////////////////////////////////////////////////
//
// Base class for cuts on AOD reconstructed heavy-flavour decay
//
// Author: A.Dainese, andrea.dainese@pd.infn.it
/////////////////////////////////////////////////////////////
#include <Riostream.h>

#include "AliVEvent.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliVVertex.h"
#include "AliESDVertex.h"
#include "AliAODVertex.h"
#include "AliESDtrackCuts.h"
#include "AliAODRecoDecayHF.h"
#include "AliRDHFCuts.h"

ClassImp(AliRDHFCuts)

//--------------------------------------------------------------------------
AliRDHFCuts::AliRDHFCuts(const Char_t* name, const Char_t* title) : 
AliAnalysisCuts(name,title),
fMinVtxType(3),
fMinVtxContr(1),
fMaxVtxRedChi2(1e6),
fMinSPDMultiplicity(0),
fTriggerMask(0),
fTrackCuts(0),
fnPtBins(1),
fnPtBinLimits(1),
fPtBinLimits(0),
fnVars(1),
fVarNames(0),
fnVarsForOpt(0),
fVarsForOpt(0),
fGlobalIndex(1),
fCutsRD(0),
fIsUpperCut(0)
{
  //
  // Default Constructor
  //
}
//--------------------------------------------------------------------------
AliRDHFCuts::AliRDHFCuts(const AliRDHFCuts &source) :
  AliAnalysisCuts(source),
  fMinVtxType(source.fMinVtxType),
  fMinVtxContr(source.fMinVtxContr),
  fMaxVtxRedChi2(source.fMaxVtxRedChi2),
  fMinSPDMultiplicity(source.fMinSPDMultiplicity),
  fTriggerMask(source.fTriggerMask),
  fTrackCuts(0),
  fnPtBins(source.fnPtBins),
  fnPtBinLimits(source.fnPtBinLimits),
  fPtBinLimits(0),
  fnVars(source.fnVars),
  fVarNames(0),
  fnVarsForOpt(source.fnVarsForOpt),
  fVarsForOpt(0),
  fGlobalIndex(source.fGlobalIndex),
  fCutsRD(0),
  fIsUpperCut(0)
{
  //
  // Copy constructor
  //
  cout<<"Copy constructor"<<endl;
  if(source.GetTrackCuts()) AddTrackCuts(source.GetTrackCuts());
  /*
  if(source.GetPtBinLimits()) SetPtBins(source.fnPtBinLimits,source.GetPtBinLimits());
  if(source.GetVarNames()) SetVarNames(source.fnVars,source.GetVarNames(),source.GetIsUpperCut());
  */
  if(source.fPtBinLimits) SetPtBins(source.fnPtBinLimits,source.fPtBinLimits);
  if(source.fVarNames) SetVarNames(source.fnVars,source.fVarNames,source.fIsUpperCut);
  if(source.fCutsRD) SetCuts(source.fGlobalIndex,source.fCutsRD);
  if(source.fVarsForOpt) SetVarsForOpt(source.fnVarsForOpt,source.fVarsForOpt);
  PrintAll();
  /*
  if (source.fCutsRD){
    fCutsRD = new Float_t*[fnVars];
    
    for(Int_t iv=0; iv<fnVars; iv++) {
      
      fCutsRD[iv] = new Float_t[fnPtBins];
      
      for(Int_t ib=0; ib<fnPtBins; ib++) {
	
	fCutsRD[iv][ib] = source.fCutsRD[iv][ib];
      }
    }
  }
  */
  /*
  Float_t** cutsRD=0;
  
  cutsRD=new Float_t*[source.fnVars];
  for (Int_t iv=0;iv<source.fnVars;iv++){
    cutsRD[iv]=new Float_t[source.fnPtBins];
  }
 
  source.GetCuts(source.fnVars,source.fnPtBins,cutsRD);

  //if(source.GetCuts())SetCuts(source.fnVars,source.fnPtBins,source.GetCuts());
  
  for(Int_t iv=0; iv<source.fnVars; iv++) {

    for(Int_t ib=0; ib<source.fnPtBins; ib++) {
      cout<<"fCutsRD["<<iv<<"]["<<ib<<"] = "<<fCutsRD[iv][ib]<<endl;
      //cutsRD[iv][ib] = fCutsRD[iv][ib];
      //cout<<"cutsRD["<<iv<<"]["<<ib<<"] = "<<cutsRD[iv][ib]<<endl;
    }
  }
  
  if(cutsRD) SetCuts(source.fnVars,source.fnPtBins,cutsRD);
 */  
  //if(source.GetVarsForOpt()) SetVarsForOpt(source.fnVars,source.GetVarsForOpt());

}
//--------------------------------------------------------------------------
AliRDHFCuts &AliRDHFCuts::operator=(const AliRDHFCuts &source)
{
  //
  // assignment operator
  //
  if(&source == this) return *this;

  AliAnalysisCuts::operator=(source);

  fMinVtxType=source.fMinVtxType;
  fMinVtxContr=source.fMinVtxContr;
  fMaxVtxRedChi2=source.fMaxVtxRedChi2;
  fMinSPDMultiplicity=source.fMinSPDMultiplicity;
  fTriggerMask=source.fTriggerMask;
  fnPtBins=source.fnPtBins;
  fnVars=source.fnVars;
  fGlobalIndex=source.fGlobalIndex;
  fnVarsForOpt=source.fnVarsForOpt;

  if(source.fPtBinLimits) SetPtBins(source.fnPtBinLimits,source.fPtBinLimits);
  if(source.fVarNames) SetVarNames(source.fnVars,source.fVarNames,source.fIsUpperCut);
  if(source.fCutsRD) SetCuts(source.fGlobalIndex,source.fCutsRD);
  if(source.fVarsForOpt) SetVarsForOpt(source.fnVarsForOpt,source.fVarsForOpt);
  PrintAll();

  /*
  if(source.GetTrackCuts()) AddTrackCuts(source.GetTrackCuts());

  if(source.GetPtBinLimits()) SetPtBins(source.fnPtBinLimits,source.GetPtBinLimits());
  if(source.GetVarNames()) SetVarNames(source.fnVars,source.GetVarNames(),source.GetIsUpperCut());
  */
/*
  Float_t** cutsRD;
  cutsRD=new Float_t*[source.fnVars];
  for (Int_t iv=0;iv<source.fnVars;iv++){
    cutsRD[iv]=new Float_t[source.fnPtBins];
  }
  source.GetCuts(source.fnVars,source.fnPtBins,cutsRD);

  if(cutsRD) SetCuts(source.fnVars,source.fnPtBins,cutsRD);
*/
  //if(source.GetCuts()) SetCuts(source.fnVars,source.fnPtBins,source.GetCuts());
  //if(source.GetVarsForOpt()) SetVarsForOpt(source.fnVars,source.GetVarsForOpt());

  return *this;
}
//--------------------------------------------------------------------------
AliRDHFCuts::~AliRDHFCuts() {
  //  
  // Default Destructor
  //
  if(fPtBinLimits) {delete [] fPtBinLimits; fPtBinLimits=0;}
  if(fVarNames) {delete [] fVarNames; fVarNames=0;}
  if(fVarsForOpt) {delete [] fVarsForOpt; fVarsForOpt=0;}
  if(fCutsRD) {
    delete [] fCutsRD;
    fCutsRD=0;
  }
  if(fIsUpperCut) {delete [] fIsUpperCut; fIsUpperCut=0;}

}
//---------------------------------------------------------------------------
Bool_t AliRDHFCuts::IsEventSelected(AliVEvent *event) const {
  //
  // Event selection
  // 
  if(fTriggerMask && event->GetTriggerMask()!=fTriggerMask) return kFALSE;

  // multiplicity cuts no implemented yet

  const AliVVertex *vertex = event->GetPrimaryVertex();

  if(!vertex) return kFALSE;

  TString title=vertex->GetTitle();
  if(title.Contains("Z") && fMinVtxType>1) return kFALSE; 
  if(title.Contains("3D") && fMinVtxType>2) return kFALSE; 

  if(vertex->GetNContributors()<fMinVtxContr) return kFALSE; 

  return kTRUE;
}
//---------------------------------------------------------------------------
void AliRDHFCuts::SetPtBins(Int_t nPtBinLimits,Float_t *ptBinLimits) {
  // Set the pt bins

  if(fPtBinLimits) {
    delete [] fPtBinLimits;
    fPtBinLimits = NULL;
    printf("Changing the pt bins\n");
  }

  if(nPtBinLimits != fnPtBins+1){
    cout<<"Warning: ptBinLimits dimention "<<nPtBinLimits<<" != nPtBins+1 ("<<fnPtBins+1<<")\nSetting nPtBins to "<<nPtBinLimits-1<<endl;
    SetNPtBins(nPtBinLimits-1);
  }

  fnPtBinLimits = nPtBinLimits;
  SetGlobalIndex();
  cout<<"Changing also Global Index -> "<<fGlobalIndex<<endl;
  fPtBinLimits = new Float_t[fnPtBinLimits];
  for(Int_t ib=0; ib<nPtBinLimits; ib++) fPtBinLimits[ib]=ptBinLimits[ib];

  return;
}
//---------------------------------------------------------------------------
void AliRDHFCuts::SetVarNames(Int_t nVars,TString *varNames,Bool_t *isUpperCut){
  // Set the variable names

  if(fVarNames) {
    delete [] fVarNames;
    fVarNames = NULL;
    printf("Changing the variable names\n");
  }
  if(nVars!=fnVars){
    printf("Wrong number of variables: it has to be %d\n",fnVars);
    return;
  }
  //fnVars=nVars;
  fVarNames = new TString[nVars];
  fIsUpperCut = new Bool_t[nVars];
  for(Int_t iv=0; iv<nVars; iv++) {
    fVarNames[iv] = varNames[iv];
    fIsUpperCut[iv] = isUpperCut[iv];
  }

  return;
}
//---------------------------------------------------------------------------
void AliRDHFCuts::SetVarsForOpt(Int_t nVars,Bool_t *forOpt) {
  // Set the variables to be used for cuts optimization

  if(fVarsForOpt) {
    delete [] fVarsForOpt;
    fVarsForOpt = NULL;
    printf("Changing the variables for cut optimization\n");
  }
  
  if(nVars==0){//!=fnVars) {
    printf("%d not accepted as number of variables: it has to be %d\n",nVars,fnVars);
    return;
  } 
  
  fnVarsForOpt = 0;
  fVarsForOpt = new Bool_t[fnVars];
  for(Int_t iv=0; iv<fnVars; iv++) {
    fVarsForOpt[iv]=forOpt[iv];
    if(fVarsForOpt[iv]) fnVarsForOpt++;
  }

  return;
}
//---------------------------------------------------------------------------
void AliRDHFCuts::SetCuts(Int_t nVars,Int_t nPtBins,Float_t **cutsRD) {
  // set the cuts

  if(nVars!=fnVars) {
    printf("Wrong number of variables: it has to be %d\n",fnVars);
    return;
  } 
  if(nPtBins!=fnPtBins) {
    printf("Wrong number of pt bins: it has to be %d\n",fnPtBins);
    return;
  } 

  if(!fCutsRD)  fCutsRD = new Float_t[fGlobalIndex];
  
  Int_t iGlobal=0;
  for(Int_t iv=0; iv<fnVars; iv++) {

    for(Int_t ib=0; ib<fnPtBins; ib++) {

      fCutsRD[iGlobal] = cutsRD[iv][ib];
      //cout<<"fCutsRD["<<iGlobal<<"] = "<<fCutsRD[iGlobal]<<endl;
      iGlobal++;
      //check
      if(iGlobal>fGlobalIndex) {
	cout<<"Overflow, exit..."<<endl;
	return;
      }
    }
  }
  //check
  if(iGlobal != fGlobalIndex) {
    cout<<"Total number of entries of cut matrix "<<iGlobal<<" not expected! Check please..."<<endl;
    return;
  }

  cout<<"done"<<endl;
  return;
}
//---------------------------------------------------------------------------
void AliRDHFCuts::SetCuts(Int_t glIndex,Float_t* cutsRDGlob){
  if(glIndex != fGlobalIndex){
    cout<<"Wrong array size: it has to be "<<fGlobalIndex<<endl;
    return;
  }
  if(!fCutsRD)  fCutsRD = new Float_t[fGlobalIndex];

  for(Int_t iGl=0;iGl<fGlobalIndex;iGl++){
    fCutsRD[iGl] = cutsRDGlob[iGl];
  }
  cout<<"done"<<endl;
  return;

}


//---------------------------------------------------------------------------
void AliRDHFCuts::PrintAll() const{
  if(fVarNames){
    cout<<"Array of variables"<<endl;
    for(Int_t iv=0;iv<fnVars;iv++){
      cout<<fVarNames[iv]<<"\t";
    }
    cout<<endl;
  }
  if(fVarsForOpt){
    cout<<"Array of optimization"<<endl;
    for(Int_t iv=0;iv<fnVars;iv++){
      cout<<fVarsForOpt[iv]<<"\t";
    }
    cout<<endl;
  }
  if(fIsUpperCut){
    cout<<"Array of upper/lower cut"<<endl;
   for(Int_t iv=0;iv<fnVars;iv++){
     cout<<fIsUpperCut[iv]<<"\t";
   }
   cout<<endl;
  }
  if(fPtBinLimits){
    cout<<"Array of ptbin limits"<<endl;
    for(Int_t ib=0;ib<fnPtBinLimits;ib++){
      cout<<fPtBinLimits[ib]<<"\t";
    }
    cout<<endl;
  }
  if(fCutsRD){
    cout<<"Matrix of cuts"<<endl;
   for(Int_t iv=0;iv<fnVars;iv++){
     for(Int_t ib=0;ib<fnPtBins;ib++){
       cout<<"fCutsRD["<<iv<<"]["<<ib<<"] = "<<fCutsRD[GetGlobalIndex(iv,ib)]<<"\t";
     }
     cout<<endl;
   }
   cout<<endl;
  }
}


//---------------------------------------------------------------------------
void AliRDHFCuts::GetCuts(Float_t**& cutsRD) const{
  // get the cuts

//   if(nVars!=fnVars){
//     cout<<"Number of variables doesn't match or not defined yet"<<endl;
//     return;
//   }

//   if(nPtBins!=fnPtBins){
//     cout<<"Number of pt bins doesn't match or not defined yet"<<endl;
//     return;
//   }
  cout<<"Give back a "<<fnVars<<"x"<<fnPtBins<<" matrix."<<endl;
  //cutsRD=fCutsRD;

  if(!cutsRD) {
    cout<<"Initialization..."<<endl;
    cutsRD=new Float_t*[fnVars];
    for(Int_t iv=0; iv<fnVars; iv++) {
      cutsRD[iv] = new Float_t[fnPtBins];
    }
  }
  
  cout<<"Ptr cuts rd "<<cutsRD<<endl;
  Int_t iGlobal=0;
  for(Int_t iv=0; iv<fnVars; iv++) {
    for(Int_t ib=0; ib<fnPtBins; ib++) {
      //cout<<"fCutsRD["<<iv<<"]["<<ib<<"] = "<<fCutsRD[iv][ib]<<endl;
      cutsRD[iv][ib] = fCutsRD[iGlobal];
      //cout<<"cutsRD["<<iv<<"]["<<ib<<"] = "<<cutsRD[iv][ib]<<endl;
      iGlobal++;
    }
  }

}

//---------------------------------------------------------------------------
Int_t AliRDHFCuts::GetGlobalIndex(Int_t iVar,Int_t iPtBin) const{
  return iPtBin*fnVars+iVar;
}

//---------------------------------------------------------------------------
void AliRDHFCuts::GetVarPtIndex(Int_t iGlob, Int_t& iVar, Int_t& iPtBin) const {
  //give the index of the variable and of the pt bin from the global index

  iPtBin=(Int_t)iGlob/fnVars;
  iVar=iGlob%fnVars;

}

//---------------------------------------------------------------------------
Int_t AliRDHFCuts::PtBin(Double_t pt) const {

  //give the pt bin where the pt lies.

  Int_t ptbin=0;
  for (Int_t i=0;i<fnPtBins;i++){
    if(pt<fPtBinLimits[i+1]) {
      ptbin=i;
      break;
    }
  }
  return ptbin;
}
