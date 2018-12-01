/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskSounds_H
#define AliAnalysisTaskSounds_H

#include "AliAnalysisTaskSE.h"
#include "TNtuple.h"
#include <iostream>
#include <fstream>


class AliAnalysisTaskSounds : public AliAnalysisTaskSE  
{
    public:
                                AliAnalysisTaskSounds();
                                AliAnalysisTaskSounds(const char *name);
        virtual                 ~AliAnalysisTaskSounds();

				//Setters
				void										SetPitchOption(Int_t option) {fPitchOption = option;} 
				void										SetOutputName(TString option) {fOutputName = option;} 
				void										SetPtHigh(Double_t option) {fPtHigh = option;}
				void										SetPtLow(Double_t option) {fPtLow = option;}
				void										SetNmaxEvents(Int_t option) {fNmaxEvents = option;}
				void										SetIsLowPtCutoff(Bool_t option) {fIsLowPtCutoff = option;}

				//Getters
				Double_t                GetPitch(Double_t pt);
				Double_t                GetDynamic(Double_t p, Double_t pt);
				Int_t										GetTime(Double_t phi);

//				TString									GetWrittenNote(Double_t time, Double_t pitch, Double_t dynamic);
				TString									PitchToString(Double_t pitch);

        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);

        enum {kAll,kEbony,kIvory};

    private:


        AliAODEvent*            fAOD;           //! input event
        TList*                  fOutputList;    //! output list
        TH1F*                   fHistPt;        //! dummy histogram

				TClonesArray *fStaveArray; //! array of notes
				TNtuple *fStave; //! ntuple
				Int_t fDivision; //! division
				Double_t fPtHigh; /// max pt
				Double_t fPtLow;	 /// min pt
				Double_t f2pi;   /// 2 pi
				Int_t fPitchOption; // pitch option
				Bool_t fIsLowPtCutoff;	 /// is the low pt limit removed from score
				Int_t fEventCounter; /// event counter
				Int_t fNmaxEvents; /// max number of events to run over
				TString fOutputName; /// output name
				ofstream fOutputStream;

        AliAnalysisTaskSounds(const AliAnalysisTaskSounds&); // not implemented
        AliAnalysisTaskSounds& operator=(const AliAnalysisTaskSounds&); // not implemented

        ClassDef(AliAnalysisTaskSounds, 1);
};

#endif

class MyNote : public TObject {
	public:
		//MyNote();
		MyNote(Int_t time, Double_t pitch, Double_t dynamic):
			fTime(time), fPitch(pitch), fDynamic(dynamic) { }
		~MyNote() { }

		//Get values
		Int_t    GetTime() {return fTime;}
		Double_t GetPitch() {return fPitch;}
		Double_t GetDynamic() {return fDynamic;}

	private:
		Int_t fTime;
		Double_t fPitch;
		Double_t fDynamic;

		ClassDef(MyNote, 1);
};

//MyNote::MyNote(Int_t time, Double_t pitch, Double_t dynamic):
//	fTime(time), fPitch(pitch), fDynamic(dynamic) { };
//
//MyNote::MyNote():
//	fTime(0), fPitch(0), fDynamic(0) { };
