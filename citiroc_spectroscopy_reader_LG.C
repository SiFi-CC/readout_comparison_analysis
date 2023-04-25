#include <iostream>
#include <fstream>
#include <stdlib.h>
//this macro takes RunXXX_list.txt file and creates histograms for each channel.
//had to use stoll() instead of stoi() bcs of getting out of range, takes a while
void citiroc_spectroscopy_reader_LG(){
    
    const int N = 1; //    number of histograms
    int nofr = 1000; // # of first run
    int bin_num = 500;
    
    // creates an array of file names of RunXXX_list.txt format
    const char *data[N];
    std::string s[N];
    for (int i =0; i < N; i++)
    {
        s[i] =  "Run" + std::to_string(i+nofr) + "_list.txt";
        data[i] = &s[i][0];
    }
    //histograms
        TCanvas *c = new TCanvas("c","c");
        c->DivideSquare(2);
        gStyle->SetOptStat(111111); // 111111 if overflow, 0 if hists on the same graph
        //gStyle->SetOptTitle(kFALSE); // for multiple hists on the same graph
        gPad->SetGrid(0,1);
        TH1F *h[N][63]; // for each channel 8,9,10,11
        TH1F *hh = new TH1F();
        for(int i  = 0; i < 64; i++)
        {
            for(int j = 0; j<N; j++)
            {
                h[j][i] = new TH1F(TString::Format("Run%d_ch%d",nofr+j,i),TString::Format("Run%d_ch%d",nofr+j,i), bin_num,-10,8000);
            }
        }
    
    for(int run = 0; run < N; run++)
    {
        cout << "loading data " << run << "/" << N <<" : " << data[run] << endl;
        std::ifstream istream;
        istream.open(data[run], std::ios::in);
    //     istream.open("/home/magda/DATA_CITIROC/Run316_list.txt", std::ios::in);
        if (!istream.is_open())
        {
            std::cerr << "Error! Could not open input file!" << std::endl;
            return false;
        }
        

        std::string csvLine;
        std::string tmp;
        std::string str_tstamp_us, str_trgID, str_brd, str_ch, str_q_lg, str_q_hg;
        float tstamp_us = 0.;
        float q_lg = 0.;
        int trgID = 0;
        int brd = 0; 
        int ch = 0;
        std::string q_hg;
        int not_spaces=0;

        istream.seekg(std::ios_base::beg);
        for (int i=0; i<9;i++) getline(istream, tmp); //skip header
        
        while(!istream.eof()){
            getline(istream, csvLine);
            for(int j=0; j<csvLine.length(); j++){
                if(csvLine[j]!=' ') not_spaces++;
            }
            if(not_spaces <= 10){
                std::stringstream stream(csvLine);
                stream >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
                brd = std::stoll(str_brd);
                ch = std::stoll(str_ch);
                q_lg = std::stof(str_q_lg);
                q_hg = str_q_hg;
            //cout <<  brd << " " << ch << " " << q_lg << " " << q_hg << endl;
            }
            if(not_spaces > 10){
                std::stringstream stream(csvLine);
                stream >> str_tstamp_us >> str_trgID >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
                tstamp_us = std::stof(str_tstamp_us);
                trgID = std::stoll(str_trgID);
                brd = std::stoi(str_brd);
                ch = std::stoi(str_ch);
                q_lg = std::stof(str_q_lg);
                q_hg = str_q_hg;
            //cout << tstamp_us << " " <<  trgID <<" " <<  brd << " " << ch << " " << q_lg << " " << q_hg << endl;
            }
            not_spaces=0;
            h[run][ch]->Fill(q_lg);
        }
        istream.close();
    }
    
//     for(int i = 0; i<2;i++) // drawing desired 2 histos
//     {
//         c->cd(i+1);
//         h[i][11+i]->Draw("hist");
//     }
    
    //saving the histograms to a file
    TList *l = new TList();
    for(int i  = 0; i < 64; i++)
    {
        for(int j = 0; j<N; j++)
        {
            l->Add(h[j][i]);
        }
    }
    TFile *f = new TFile(TString::Format("histogram_LG_list_Run%d-%d", nofr,nofr+N), "RECREATE"); 
    l->Write("histlist_LG", TObject::kSingleKey);
    f->ls();
    
    c->cd(0);
    delete hh;
}
