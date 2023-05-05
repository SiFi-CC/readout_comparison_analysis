/**
 * plot_energy_resolution.C: ROOT Macro for plotting the coincidence time resolutions for the readout systems.
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_energy_resolution.C 
 *
 */
void plot_energy_resolution() {
    std::vector<std::string> text = {"Klaus6b 10bit LG", "Twinpeaks", "Citiroc LG", "TOFPet2b", "DRS4"};
    std::vector<float> readout = {0, 1, 2, 3, 4, 5};
    // fit gaussian and read sigma
    std::vector<float> _mean = {0, 535, 0, 580, 506.3};
    std::vector<float> _meanerr = {0, 0.4, 0, 1, 0.2};
    std::vector<float> _sigma = {0, 14.8, 0, 72.4, 43.3};
    std::vector<float> _sigmaerr = {0, 0.5, 0, 1.7, 0.2};
    std::vector<float> _corr = {0, 0, 0, -0.3, 0}; //correlation between mean and sigma fit values

    std::vector<float> _data(5, 0);
    std::vector<float> _err(5, 0);
    for(int i=0; i<_data.size(); ++i) {
        if(_mean[i] == 0) continue;
        _data[i] = _sigma[i] / _mean[i];
        _err[i] = _data[i] * TMath::Sqrt(TMath::Power(_sigmaerr[i]/_sigma[i], 2) + TMath::Power(_meanerr[i]/_mean[i], 2) - 2*_corr[i]/(_sigma[i]*_mean[i]) );
    }

    TH1F *h= new TH1F("h", ";Readout system;#frac{#sigma}{#mu}[keV]", text.size(), 0, text.size() );
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
    c->SaveAs("plot_energy_resolution.pdf");
}
