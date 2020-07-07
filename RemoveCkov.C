void RemoveCkov(TString sKineFile = "/run/media/heimarry/aarrearkku/o2-sim-data-5020GeV/cent30-35/o2sim_Kine.root")
{
    TFile *fIn = TFile::Open(sKineFile);
    TTree* kineTree = (TTree*)fIn->Get("o2sim");

    std::vector<o2::MCTrack>* mctrack = nullptr;
    auto mcbr = kineTree->GetBranch("MCTrack");
    mcbr->SetAddress(&mctrack);

    UInt_t nEntries = kineTree->GetEntries();
    std::cout << "MC events : " << nEntries << std::endl;

    TFile *fOut = TFile::Open("output_nockov.root", "RECREATE");
    fOut->cd();
    auto newTree = kineTree->CloneTree(0);

    std::vector<o2::MCTrack>* mctracknew = nullptr;
    auto mcnewbr = newTree->GetBranch("MCTrack");
    mcnewbr->SetAddress(&mctracknew);

    int sum = 0;
    for (UInt_t ient = 0; ient < nEntries; ient++) {
        int notCkov = 0;

        std::cout << "\tEntry " << ient;

        mcbr->GetEntry(ient);
        mcnewbr->GetEntry(ient);

        int itrack = 0;
        UInt_t nTracks = mctrack->size();
        std::cout << ",\tMCtracks : " << nTracks;
        for (auto &track : *mctrack) {
            int pdg = track.GetPdgCode();
            if (pdg!=50000050) {
                mctracknew->push_back(track);
                notCkov++;
            }
            itrack++;
        }
        newTree->Fill();
        mctracknew->clear();

        std::cout << ",\tNon-cerenkovs : " << notCkov << std::endl;
        sum += notCkov;
    }

    std::cout << "\nSum of saved tracks : " << sum << std::endl;

    fOut->Write("",TObject::kOverwrite);
    fOut->Close();
    fIn->Close();
}
