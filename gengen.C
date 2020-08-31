/**
 * General macro for O2 that needs to be given a ROOT file with particle data
 * in following NTuple format:
 *
 *      eventId:particleId:px:py:pz:x:y:z:isHadron:charge
 *
 * "Pythia6Generator.cxx" and "extgen.C" from ALICE O2 framework used as
 * template
 */

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"

#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TMath.h>
#include <iostream>
#include <cstdio>
#endif

class GeneralGenerator : public FairGenerator
{
    public:

        /**
         * Standard constructor
         * @param inputFile     The input file name
         * @param inputGenType  The initial generator type
         */
        GeneralGenerator(const char* inputFile, const char* inputGenType)
            : FairGenerator("GeneralGenerator"), mFileName(inputFile),
              mInputFile(nullptr), mGenType(inputGenType)
        {
            std::cout << "Opening input file " << mFileName << std::endl;
            mInputFile = TFile::Open(mFileName);
            if (!mInputFile->IsOpen()) {
                std::cout << "cannot open the input file\n";
            }

            if (strcmp(inputGenType, "ampt") == 0) {
                std::cout << "Using data from AMPT\n";
                events = (TNtuple*)mInputFile->Get("amptEvents");
            }

            if (strcmp(inputGenType, "pythia") == 0) {
                std::cout << "Using data from Pythia\n";
                events = (TNtuple*)mInputFile->Get("pythiaEvents");
            }

            if (strcmp(inputGenType, "toyflow") == 0) {
                std::cout << "Using data from ToyFlow\n";
                events = (TNtuple*)mInputFile->Get("events");
            }

            iStartNewEvent = 0;
            previd = -1;
        };

        /**
         * Destructor
         */
        ~GeneralGenerator() override
        {
            CloseInput();
        };

        /**
         * Reads event from the input file and adds the track onto the stack.
         * @param  primGen Pointer to the PrimaryGenerator
         * @return         true if events read, false if no events any more
         */
        Bool_t ReadEvent(FairPrimaryGenerator* primGen) override
        {

            if (!mInputFile->IsOpen()) {
                std::cout << "Input file not opened!" << std::endl;
                return kFALSE;
            }

            // AMPT gives the production vertex in fm
            Double_t fm2cm = pow(10, -13);

            // Event variables
            Float_t eventid = 0;

            // Track variables
            Float_t particleid = 0;
            Float_t charge = 0;
            Float_t px = 0.0, py = 0.0, pz = 0.0, x = 0.0, y = 0.0, z = 0.0;

            events->SetBranchAddress("particleId",&particleid);
            events->SetBranchAddress("eventId",&eventid);
            events->SetBranchAddress("charge",&charge);
            events->SetBranchAddress("px",&px);
            events->SetBranchAddress("py",&py);
            events->SetBranchAddress("pz",&pz);
            events->SetBranchAddress("x",&x);
            events->SetBranchAddress("y",&y);
            events->SetBranchAddress("z",&z);

            Int_t nentries = (Int_t)events->GetEntries();
            for ( Int_t i=iStartNewEvent; i<nentries; i++ ) {

                Int_t entry = events->GetEntry(i);
                if (previd==-1) previd = eventid;
                if (entry>0) {
                    if ((Int_t)eventid==previd) {
                        if (TMath::Abs(px)==0) px = 1e-9;
                        if (TMath::Abs(py)==0) py = 1e-9;
                        if (TMath::Abs(pz)==0) pz = 1e-9;

                        primGen->AddTrack((Int_t)particleid, (Double_t)px, (Double_t)py, (Double_t)pz, fm2cm*x, fm2cm*y, fm2cm*z);
                    } else {
                        previd = eventid;
                        iStartNewEvent = i;
                        break;
                    }
                } else if (entry==0) {
                    std::cout << "No entry found" << std::endl;
                    CloseInput();
                    return kFALSE;
                } else {
                    std::cout << "I/O error occured" << std::endl;
                    CloseInput();
                    return kFALSE;
                }
            }

            std::cout << "Sent particles for event id " << eventid << std::endl;	

            return kTRUE;
        };

    private:
        const Char_t* mFileName;
        const Char_t* mGenType;
        TFile* mInputFile;
        TNtuple* events;
        Int_t previd;
        Int_t iStartNewEvent;

        /**
         * Close input file after reading
         */
        void CloseInput()
        {
            if (mInputFile->IsOpen()) {
                std::cout << "Closing input file " << mFileName << std::endl;
                mInputFile->Close();
            }
        };

        /**
         * Some particles are not recognized by Geant4 and thus need to be skipped.
         * Keep in mind that this is just an example function and does not at the
         * moment include all the particles that cause the simulation to crash.
         *
         * Geant3 recommended to be used.
         */
        bool SkipParticle(int pid)
        {
            std::vector<std::string> badPids{"411", "421", "413", "423", "415", "425",
                                             "431", "433", "435", "511", "521", "513",
                                             "523", "515", "525", "531", "533", "535",
                                             "541", "543", "545", "130", "310"};
            //std::vector<std::string> badPids{"421", "423", "425", "441", "511", "513", "515"};
            std::string str = std::to_string(TMath::Abs(pid));
            //if (str.size()<3) return false;
            //str = str.substr(str.size() - 3);
            for (auto & i : badPids) {
                if (i==str) {
                    std::cout << "Skip particle " << pid << std::endl;
                    return true;
                }
            }
            return false;
        };
};

/**
 * @param  inputFile     ROOT file that includes the data in ntuple
 * @param  inputGenType  Generator that was used to generate the data
 *                       Following generators available:
 *                          - ampt
 *                          - pythia
 *                          - toyflow
 * @return               The generator
 */
FairGenerator*
    gengen(const char* inputFile = "cent30-35_n93.root", const char* inputGenType = "ampt")
{
    std::cout << "General generator for data in NTuple format" << std::endl;
    auto gen = new GeneralGenerator(inputFile, inputGenType);
    return gen;
}
