/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

/* AliAnaysisTaskSounds
 *
 * class to map track properties in an event (momentum, topology) into 'abc' 
 * format, which can be converted into a musical score
 *
 * based on the analysis tutorial written by Redmer Bertens
 * 
 */

#include "TChain.h"
#include "TH1F.h"
#include "TList.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskSounds.h"
#include "TNtuple.h"
#include "TMath.h"

#include <iostream>
#include <fstream>

class AliAnalysisTaskSounds;    

using namespace std;            

ClassImp(AliAnalysisTaskSounds) 

AliAnalysisTaskSounds::AliAnalysisTaskSounds() : AliAnalysisTaskSE(), 
    fAOD(0), 
		fOutputList(0), 
		fHistPt(0),
		fStave(0x0),
		fDivision(16),
		fPHigh(10.),
		fPLow(0.),
		f2pi(TMath::Pi() * 2),
		fPitchOption(kIvory),
		fEventCounter(0),
		fNmaxEvents(50),
		fOutputName("output")
{

}
//_____________________________________________________________________________
AliAnalysisTaskSounds::AliAnalysisTaskSounds(const char* name) : AliAnalysisTaskSE(name),
		fAOD(0), 
		fOutputList(0), 
		fHistPt(0),
		fStave(0x0),
		fDivision(16),
		fPHigh(10.),
		fPLow(0.),
		f2pi(TMath::Pi() * 2),
		fPitchOption(kIvory),
		fEventCounter(0),
		fNmaxEvents(50),
		fOutputName("output")
{
    // constructor
    DefineInput(0, TChain::Class());    
                                        
                                        
    DefineOutput(1, TList::Class());    
                                        
                                        
                                        
}
//_____________________________________________________________________________
AliAnalysisTaskSounds::~AliAnalysisTaskSounds()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskSounds::UserCreateOutputObjects()
{
	//
    // create output objects
		//
    fOutputList = new TList();          
                                        
                                        
    fOutputList->SetOwner(kTRUE);       
                                        

    // example of a histogram
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       
    fOutputList->Add(fHistPt);          
		fStave = new TNtuple("fStave","stave for an event","time:pitch:dynamic");
		fOutputList->Add(fStave);

		fStaveArray = new TClonesArray("MyNote",10000);
                                        // your histogram in the output file, add it to the list!
		fOutputStream.open(Form("%s.abc",fOutputName.Data()));
		fOutputStream<<"X:1\nT:Particles\nC:the ALICE collaboration\nM:4/4\nL:1/"<<fDivision<<"\nK:C\n";
			//X:1
			//T:Paddy O'Rafferty
			//C:Trad.
			//M:6/8
			//L:1/8
			//K:D
    
    PostData(1, fOutputList);           
                                        
                                        
}
//_____________________________________________________________________________
void AliAnalysisTaskSounds::UserExec(Option_t *)
{
	if(fEventCounter>fNmaxEvents) return;
	fEventCounter++;
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    
                                                        
                                                        
    if(!fAOD) return;                                   
    Int_t iTracks(fAOD->GetNumberOfTracks());           
		Float_t timeNo, pitchNo, dynamicNo;
		AliInfo(Form("event counter = %i\tn tracks = %i",fEventCounter,iTracks));
    for(Int_t i(0); i < iTracks; i++) {                 // loop over all tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         
        if(!track) continue;                            
        fHistPt->Fill(track->Pt());                     // plot the pt value of the track in a histogram
				Float_t p = track->P();
				Float_t pt = track->Pt();
				Float_t phi = track->Phi();
				AliInfo(Form("p = %f\tpt = %f\tphi = %f",p,pt,phi));

				pitchNo = GetPitch(p);
				timeNo = GetTime(phi);
				dynamicNo = GetDynamic(p,pt);
				new((*fStaveArray)[i]) MyNote(timeNo, pitchNo, dynamicNo);
//				MyNote *note = new(  (*fStaveArray)[i]  ) MyNote(timeNo, pitchNo, dynamicNo);
				//AliInfo(Form("time from note = %i",note.GetTime()));	
				//AliInfo(Form("pitchNo = %f\ttimeNo = %i\t",pitchNo,timeNo));
				Float_t tmp[3];
				tmp[0] = timeNo;
				tmp[1] = pitchNo;
				tmp[2] = dynamicNo;
				fStave->Fill(tmp);
    }                                                   // continue until all the tracks are processed
		AliInfo(Form("Finished loop on tracks - n entries in array = %i",fStaveArray->GetEntriesFast()));
		fStaveArray->Print();
		// finish loop
		// now sort stave
		Int_t nentries = fStaveArray->GetEntriesFast();
		Float_t *timeArray = new Float_t[nentries];
		for(Int_t itime=0;itime<nentries;itime++) {
			timeNo = ((MyNote*)fStaveArray->At(itime))->GetTime();
			timeArray[itime] = timeNo;
			AliInfo(Form("Add to array - time = %f",timeNo));
		}

		// sort in order of time and save order in index
		Int_t *index = new Int_t[nentries];
		TMath::Sort(nentries,timeArray,index);
		TString bar = "";
		Int_t nRests = 0;
		bar.Append("| ["); //start bar
		//now loop over stave in order of time
		for(Int_t i=nentries-1;i>=0;i--) {
			MyNote *note = (MyNote*)fStaveArray->At(index[i]);
			AliInfo(Form("get entry %i:",index[i]));
			timeNo    = note->GetTime();
			pitchNo   = note->GetPitch();
			dynamicNo = note->GetDynamic();
			AliInfo(Form("time = %f\tpitch = %f\tdynamic = %f",timeNo,pitchNo,dynamicNo));

			// get string corresponding to pitch
			TString pitchString = PitchToString(pitchNo);
			//if(i==nentries-1)  
			bar.Append(pitchString);
			if(i!=0) {//not last note
				if(((MyNote*)fStaveArray->At(index[i]))->GetTime() !=
						((MyNote*)fStaveArray->At(index[i-1]))->GetTime()) {
					bar.Append("]");

					//add rests if needed
					Float_t timeNoNext = timeNo;
					timeNoNext = ((MyNote*)fStaveArray->At(index[i-1]))->GetTime();
					AliInfo(Form("time now = %f\ttime next = %f",timeNo,timeNoNext));
					nRests = Int_t(timeNoNext-timeNo-1);
					if(nRests!=0) bar.Append(Form("z%i",nRests));
					bar.Append("[");
				}

			}
			else bar.Append("] ");
		}
		AliInfo(bar);
		fOutputStream<<bar;

		fStaveArray->Clear();
			
    PostData(1, fOutputList);                           
                                                        
                                                        
}
//_____________________________________________________________________________

Double_t AliAnalysisTaskSounds::GetPitch(Double_t p) {
	//get pitch as fraction of total pitch
	if(p<fPLow) return 0;
	if(p>fPHigh) return 1.;
	Double_t pitch = p / fPHigh;
	return pitch;
}

Double_t AliAnalysisTaskSounds::GetDynamic(Double_t p, Double_t pt) {
	//get dynamic where it is proportional to fraction pt/p
	Double_t dynamic = pt/p;
	return dynamic;
}

Int_t AliAnalysisTaskSounds::GetTime(Double_t phi) {
	//get semi-beat for the note to fall on
	Int_t time;
	Double_t timeFraction = phi / f2pi;
	time = timeFraction*Double_t(fDivision);
	return time;

}

TString AliAnalysisTaskSounds::PitchToString(Double_t pitch) {
	//
	// convert pitch from 0 to 1 into a string corresponding to 
	// abc note format
	//

	const Int_t nNotesIvory = 19;
	TString notesTrebleClefIvory[nNotesIvory] = {"G,","A,","B,","C","D","E","F","G","A","B","c","d","e","f","g","a","b","c'","d'"};
	const Int_t nNotesEbony = 16;
	TString notesTrebleClefEbony[nNotesEbony] = {"^D,","^F,","^G,","^A,","^C","^D","^F","^G","^A","^c","^d","^f","^g","^a","^c'","^d'"};
	const Int_t nNotesAll = 32;
	TString notesTrebleClefAll[nNotesAll] = {"G,","^G,","A,","^A,","B,","C","^C","D","^D","E","F","^F","G","^G","A","^A","B","c","^c","d","^d","e","f","^f","g","^g","a","^a","b","c'","^c'","d'"};
	Bool_t isNoteFound = kFALSE;
	Int_t nNotesLoop; 
	if(fPitchOption==kAll) nNotesLoop = nNotesAll;
	if(fPitchOption==kIvory) nNotesLoop = nNotesIvory;
	if(fPitchOption==kEbony) nNotesLoop = nNotesEbony;
	for(Int_t ipitch=0;ipitch<nNotesLoop;ipitch++){
		Double_t pitchAtNote = Double_t(ipitch) / Double_t(nNotesLoop);
		if(pitch<pitchAtNote) {
			TString noteToReturn;
			if(fPitchOption==kAll) noteToReturn = notesTrebleClefAll[ipitch];
			if(fPitchOption==kIvory) noteToReturn = notesTrebleClefIvory[ipitch];
			if(fPitchOption==kEbony) noteToReturn = notesTrebleClefEbony[ipitch];
			return noteToReturn;
		}
	}
	return "d'";
}

void AliAnalysisTaskSounds::Terminate(Option_t *)
{
    // terminate
	fOutputStream.close();
}
//_____________________________________________________________________________
