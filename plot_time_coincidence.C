/**
 * plot_time_coincidence.C: ROOT Macro for plotting the coincidence time resolutions for the readout systems.
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_time_coincidence.C 
 *
 */
void plot_time_coincidence() {
    std::vector<std::string> text = {"Klaus6b 10bit LG", "Twinpeaks", "Citiroc LG", "TOFPet2b", "DRS4"};
    std::vector<float> readout = {0, 1, 2, 3, 4, 5};
    // fit gaussian and read sigma
    std::vector<float> _data = {0.084, 0.94, 0, 0.79, 1.152}; //ns
    std::vector<float> _err = {0.001, 0.04, 0, 0.017, 0.001};

    TH1F *h= new TH1F("h", ";Readout system;Coincidence time resolution[ns]", text.size(), 0, text.size() );
    for(int i=0; i < text.size(); ++i) {
        h->GetXaxis()->SetBinLabel(i+1, Form("%s", text[i].c_str() ) );
        h->SetBinContent(i+1, _data[i]);
        h->SetBinError(i+1, _err[i]);
    }
    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas("c", "c");
    h->Draw("E");
    h->SetLineWidth(2);
    c->Draw();
    c->SaveAs("plot_time_coincidence.pdf");
}
