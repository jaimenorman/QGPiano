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
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
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

class AliAnalysisTaskSounds;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(AliAnalysisTaskSounds) // classimp: necessary for root

AliAnalysisTaskSounds::AliAnalysisTaskSounds() : AliAnalysisTaskSE(), 
    fAOD(0), 
		fOutputList(0), 
		fHistPt(0),
		fStave(0x0),
		fDivision(16),
		fPHigh(10.),
		fPLow(0.),
		f2pi(TMath::Pi() * 2),
		fEventCounter(0),
		fNmaxEvents(50)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
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
		fEventCounter(0),
		fNmaxEvents(50)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
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
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use 
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

    // example of a histogram
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       // create your histogra
    fOutputList->Add(fHistPt);          // don't forget to add it to the list! the list will be written to file, so if you want
		fStave = new TNtuple("fStave","stave for an event","time:pitch:dynamic");
		fOutputList->Add(fStave);

		fStaveArray = new TClonesArray("MyNote",10000);
                                        // your histogram in the output file, add it to the list!
		fOutputStream.open("output.abc");
		fOutputStream<<"X:1\nT:Particles\nC:the ALICE collaboration\nM:4/4\nL:1/"<<fDivision<<"\nK:C\n";
			//X:1
			//T:Paddy O'Rafferty
			//C:Trad.
			//M:6/8
			//L:1/8
			//K:D
    
    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the 
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
void AliAnalysisTaskSounds::UserExec(Option_t *)
{
	if(fEventCounter>fNmaxEvents) return;
	fEventCounter++;
	// user exec
	// this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you 
    // have access to the current event. 
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar wya
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
        // example part: i'll show how to loop over the tracks in an event 
        // and extract some information from them which we'll store in a histogram
    Int_t iTracks(fAOD->GetNumberOfTracks());           // see how many tracks there are in the event
		Float_t timeNo, pitchNo, dynamicNo;
		AliInfo(Form("event counter = %i\tn tracks = %i",fEventCounter,iTracks));
    for(Int_t i(0); i < iTracks; i++) {                 // loop ove rall these tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         // get a track (type AliAODTrack) from the event
        if(!track) continue;                            // if we failed, skip this track
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
			
    PostData(1, fOutputList);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
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

	const Int_t nNotes = 19;
	TString notesTrebleClef[nNotes] = {"G,","A,","B,","C","D","E","F","G","A","B","c","d","e","f","g","a","b","c'","d'"};
	Bool_t isNoteFound = kFALSE;
	Int_t countNote = 0;
	for(Int_t ipitch=0;ipitch<nNotes;ipitch++){
		Double_t pitchAtNote = Double_t(ipitch) / Double_t(nNotes);
		if(pitch<pitchAtNote) {
			return notesTrebleClef[ipitch];
		}
	}
	return "d'";
}

void AliAnalysisTaskSounds::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
	fOutputStream.close();
}
//_____________________________________________________________________________
