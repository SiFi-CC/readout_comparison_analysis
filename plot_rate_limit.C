/**
 * plot_rate_limit.C: ROOT Macro for plotting the collection of measured vs real data rates.
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_rate_limit.C 
 *
 */
void plot_rate_limit() {
    std::vector<float> reference = {1, 10, 20, 30, 60, 100, 200, 500};
    std::map<std::string, std::vector<float> > _data = {
        {"klaus6b10bitLG", {1, 13, 20, 35, 1040, 1040, 1040, 1040} },
        {"twinpeaks", {0, 0, 0, 0, 0, 0, 0, 0} }, //missing one data point
        {"citirocLG", {1, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7} },
        {"tofpet2b", {0, 0, 0, 0, 0, 0, 0, 0} },
        {"desktopdigitizer", {0, 0, 0, 0, 0, 0, 0, 0} }
    };
    std::map<std::string, std::vector<float> > _err = {
        {"klaus6b10bitLG", {1, 1, 1, 1, 1, 1, 1, 1} },
        {"twinpeaks", {1, 1, 1, 1, 1, 1, 0, 1} },
        {"citirocLG", {1, 1, 1, 1, 1, 1, 1, 1} },
        {"tofpet2b", {0, 0, 0, 0, 0, 0, 0, 0} },
        {"desktopdigitizer", {0, 0, 0, 0, 0, 0, 0, 0} }
    };

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
    TGraphErrors *g5 = new TGraphErrors(reference.size(), &reference[0], &_data["desktopdigitizer"][0], 0, &_err["desktopdigitizer"][0]);
    g5->SetTitle("CAEN Desktop Digitizer");
    g5->SetLineWidth(2);
    g5->SetLineColor(kBlack);

    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(";Real rate[kHz];Measured rate[kHz]");
    mg->Add(g1, "AP");
    mg->Add(g2, "AP");
    mg->Add(g3, "AP");
    mg->Add(g4, "AP");
    mg->Add(g5, "AP");
    TCanvas *c = new TCanvas("c", "c");
    mg->Draw("A");
    c->Draw();
    TLegend *legend = c->BuildLegend(0.13, 0.66, 0.43, 0.87);
    c->SaveAs("plot_rate_limit.pdf");
}
