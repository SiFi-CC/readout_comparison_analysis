/**
 * plot_rate_limit.C: ROOT Macro for plotting the collection of measured vs real data rates.
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_rate_limit.C 
 *
 */
void plot_rate_limit() {
    std::vector<float> reference = {0.1, 0.2, 0.4, 0.75, 1, 10, 20, 30, 60, 100, 200, 500};
    std::map<std::string, std::vector<float> > _measured = {
        {"klaus6b10bitLG", {0.1, 0.2, 0.4, 0.75, 1, 10, 20, 30, 58.5, 70.5, 71.1, 71.6} },
        {"twinpeaks", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }, //missing one data point
        {"citirocLG", {0.1, 0.2, 0.4, 0.75, 1, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7} },
        {"tofpet2b", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        {"drs4", {0.1, 0.2, 0.4, 0.75, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8} }
    };
    std::map<std::string, std::vector<float> > _err = {
        {"klaus6b10bitLG", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        {"twinpeaks", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        {"citirocLG", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        {"tofpet2b", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        {"drs4", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
    };

    std::map<std::string, std::vector<float> > _data;
    for(std::map<std::string, std::vector<float> >::iterator it = _measured.begin(); it != _measured.end(); ++it) {
        for(Int_t i=0; i<it->second.size(); ++i) {
            _data[it->first].push_back(_measured[it->first][i]/reference[i]);
        }
    }

    TGraphErrors *g1 = new TGraphErrors(reference.size(), &reference[0], &_data["klaus6b10bitLG"][0], 0, &_err["klaus6b10bitLG"][0]);
    g1->SetTitle("Klaus6b 10bit LG");
    g1->SetLineWidth(2);
    g1->SetLineColor(kRed);
    TGraphErrors *g2 = new TGraphErrors(reference.size(), &reference[0], &_data["twinpeaks"][0], 0, &_err["twinpeaks"][0]);
    g2->SetTitle("TwinPeaks");
    g2->SetLineWidth(2);
    g2->SetLineColor(kBlue);
    TGraphErrors *g3 = new TGraphErrors(reference.size(), &reference[0], &_data["citirocLG"][0], 0, &_err["citirocLG"][0]);
    g3->SetTitle("Citiroc LG");
    g3->SetLineWidth(2);
    g3->SetLineColor(kGreen+3);
    TGraphErrors *g4 = new TGraphErrors(reference.size(), &reference[0], &_data["tofpet2b"][0], 0, &_err["tofpet2b"][0]);
    g4->SetTitle("TOFPET2b");
    g4->SetLineWidth(2);
    g4->SetLineColor(kMagenta);
    TGraphErrors *g5 = new TGraphErrors(reference.size(), &reference[0], &_data["drs4"][0], 0, &_err["drs4"][0]);
    g5->SetTitle("DRS4");
    g5->SetLineWidth(2);
    g5->SetLineColor(kBlack);

    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(";Real rate[kHz];#frac{Measured}{Real}");
    mg->Add(g1, "ALP");
    mg->Add(g2, "ALP");
    mg->Add(g3, "ALP");
    mg->Add(g4, "ALP");
    mg->Add(g5, "ALP");
    mg->GetXaxis()->SetLimits(0.09, 600);
    TCanvas *c = new TCanvas("c", "c");
    mg->Draw("A");
    c->Draw();
    c->SetLogx();
    TLegend *legend = c->BuildLegend(0.12, 0.16, 0.42, 0.37);
    legend->SetLineWidth(0);
    c->SaveAs("plot_rate_limit.pdf");
}
