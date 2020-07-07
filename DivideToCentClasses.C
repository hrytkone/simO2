void InitRootFile(float minCent, float maxCent);

#define NCLASS 20

void DivideToCentClasses(TString sInputName = "/run/media/heimarry/462E-8EB8/2020-06_ampt-data/run_cent30-40-rand-rp_n10000_2760GeV/ampt-output01.root", bool bAllClasses=1, int centBinMin=0, int centBinMax=1) {

    TFile *fIn = TFile::Open(sInputName);

    TNtuple *ntuple = (TNtuple*)fIn->Get("amptEvents");
    Int_t nEntries = (Int_t)ntuple->GetEntries();

    float evId, pid, px, py, pz, m, x, y, z, isHadron, charge, b;

    ntuple->SetBranchAddress("eventId", &evId);
    ntuple->SetBranchAddress("particleId", &pid);
    ntuple->SetBranchAddress("px", &px);
    ntuple->SetBranchAddress("py", &py);
    ntuple->SetBranchAddress("pz", &pz);
    ntuple->SetBranchAddress("m", &m);
    ntuple->SetBranchAddress("x", &x);
    ntuple->SetBranchAddress("y", &y);
    ntuple->SetBranchAddress("z", &z);
    ntuple->SetBranchAddress("isHadron", &isHadron);
    ntuple->SetBranchAddress("charge", &charge);
    ntuple->SetBranchAddress("b", &b);

    float impactParam[NCLASS+1] {0.0,3.49,4.93,6.04,6.98,7.8,8.55,9.23,9.87,10.5,
                               11,11.6,12.1,12.6,13.1,13.5,14,14.4,14.9,15.6,20.0};

    float centClass[NCLASS+1] = {0.0,5.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,45.0,
                               50.0,55.0,60.0,65.0,70.0,75.0,80.0,85.0,90.0,95.0,100.0};

    TFile *fOut[NCLASS];
    TNtuple *ntupleCent[NCLASS];

    if (bAllClasses) {
        for (int i=0; i<NCLASS; i++) {
            InitRootFile(centClass[i], centClass[i+1]);

            fOut[i] = TFile::Open(Form("cent%02i-%02i.root", (int)centClass[i], (int)centClass[i+1]), "UPDATE");
            ntupleCent[i] = (TNtuple*)fOut[i]->Get("amptEvents");
        }


    } else {
        InitRootFile(centClass[centBinMin], centClass[centBinMax]);

        fOut[0] = TFile::Open(Form("cent%02i-%02i.root", (int)centClass[centBinMin], (int)centClass[centBinMax]), "UPDATE");
        ntupleCent[0] = (TNtuple*)fOut[0]->Get("amptEvents");
    }

    int events[NCLASS] = {0};
    float prevId = -1;

    int step = nEntries/20;
    for (int i=0; i<nEntries; i++) {

        if (i%step==0) cout << "   " << (float)i/nEntries*100.0 << "% finished" << endl;

        ntuple->GetEntry(i);

        if (bAllClasses) {

            for (int j=0; j<NCLASS; j++) {
                if (impactParam[j]<=b && impactParam[j+1]>b) {

                    if (evId!=prevId) {
                        events[j]++;
                        prevId=evId;
                    }
                    ntupleCent[j]->Fill(evId, pid, px, py, pz, m, x, y, z, isHadron, charge, b);
                }
            }

        } else {
            if (impactParam[centBinMin]<=b && impactParam[centBinMax]>b) {

                if (evId!=prevId) {
                    events[0]++;
                    prevId=evId;
                }
                ntupleCent[0]->Fill(evId, pid, px, py, pz, m, x, y, z, isHadron, charge, b);
            }
        }

    }

    ofstream fCentInfo;
    fCentInfo.open ("centInfo.txt");

    if (bAllClasses) {
        for (int i=0; i<NCLASS; i++) {
            fCentInfo << "Centrality class : " << centClass[i] << "-" << centClass[i+1] << "%,   b : " << impactParam[i] << "-" << impactParam[i+1] << ",   nevents : " << events[i] << "\n";
            fOut[i]->Write("",TObject::kOverwrite);
            fOut[i]->Close();
        }
    } else {
        fCentInfo << "Centrality class : " << centClass[centBinMin] << "-" << centClass[centBinMax] << "%,   b : " << impactParam[centBinMin] << "-" << impactParam[centBinMax] << ",   nevents : " << events[0] << "\n";
        fOut[0]->Write("",TObject::kOverwrite);
        fOut[0]->Close();
    }


    fCentInfo.close();
    fIn->Close();

}

void InitRootFile(float minCent, float maxCent) {

    TString outFileName = Form("cent%02i-%02i.root", (int)minCent, (int)maxCent);
    TFile *fOut = TFile::Open(outFileName, "RECREATE");

    // Histograms
    TNtuple *ntupleCent = new TNtuple("amptEvents", Form("cent%02i-%02i.root", (int)minCent, (int)maxCent), "eventId:particleId:px:py:pz:m:x:y:z:isHadron:charge:b");

    fOut->Write("",TObject::kOverwrite);
    fOut->Close();
}
