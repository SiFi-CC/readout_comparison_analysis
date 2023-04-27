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
        {"klaus6b10bitLG", {1, 13, 20, 35, 1040, 1040, 1040, 1040} }
    };
    std::map<std::string, std::vector<float> > _err = {
        {"klaus6b10bitLG", {1, 1, 1, 1, 1, 1, 1, 1} }
    };

    TGraphErrors *g1 = new TGraphErrors(reference.size(), &reference[0], &_data["klaus6b10bitLG"][0], 0, &_err["klaus6b10bitLG"][0]);
    g1->SetTitle("Klaus6b 10bit LG");

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(g1, "ALP");
    mg->SetTitle(";Real rate[kHz];Measured rate[kHz]");
    TCanvas *c = new TCanvas("c", "c");
    mg->Draw("A");
    c->Draw();
    c->BuildLegend(0.58, 0.14, 0.88, 0.35);
    c->SaveAs("plot_rate_limit.pdf");
}
