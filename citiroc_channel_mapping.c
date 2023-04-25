#include <iostream>
#include <fstream>
#include <stdlib.h>
//this macro takes RunXXX_list.txt file and creates channel hit map for each run.
//Usage: set N for number of runs and set nofr for number of the first run. The .root file with hit map for each run will be created.
void citiroc_channel_mapping(){
    
    const int N = 1; //    number of runs
    int nofr = 1004; // # of first run
    int treshold = 0; // treshold for channel activation
    
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
    TH2F *h[N]; // for each channel 8,9,10,11
    TH2F *hh = new TH2F();
    for(int i  = 0; i < N; i++)
    {
           
            h[i] = new TH2F(TString::Format("Run%d_ch_comparison",nofr+i),TString::Format("Run%d_ch_comparison",nofr+i), 32,0,32,32,32,64);
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
        int q[64];
        for (int l = 0; l < 64; l++)
        {	
        	q[l] = -1;	
        }

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
                q[ch] = q_lg; // filling the list with information of channel activation
            //cout <<  brd << " " << ch << " " << q_lg << " " << q_hg << endl;
            }
            if(not_spaces > 10){
            	for (int left = 0; left < 32; left++) 	// this for loop fills the histogram if within 1 time stamp
            	{						// there were multiple channels activated (coincidence)
        		for (int right = 32; right < 64; right++)
        		{
        			if(q[left] >treshold && q[right] > treshold)
        			{
        				h[run] ->Fill(left,right);
        				//cout << tstamp_us;
        			}
        		}
           	 }
            	for (int l = 0; l < 64; l++) // whenever new time stamp we clear the list
        	{	
        		q[l] = -1;	
        	}
                std::stringstream stream(csvLine);
                stream >> str_tstamp_us >> str_trgID >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
                tstamp_us = std::stof(str_tstamp_us);
                trgID = std::stoll(str_trgID);
                brd = std::stoi(str_brd);
                ch = std::stoi(str_ch);
                q_lg = std::stof(str_q_lg);
                q_hg = str_q_hg;
                q[ch] = q_lg;
            //cout << tstamp_us << " " <<  trgID <<" " <<  brd << " " << ch << " " << q_lg << " " << q_hg << endl;
            }
            not_spaces=0;
            //h[run][ch]->Fill(q_lg);
        }
        istream.close();
    }
    
    for(int i = 0; i<1;i++)
    {
        c->cd(i+1);
        h[i]->Draw("COLZ");
    }
    
    //saving the histograms to a file
    TList *l = new TList();
    for(int i  = 0; i < N; i++)
    {
            l->Add(h[i]);
    }
    TFile *f = new TFile("histogram_LG_coincidence_list.root","RECREATE");
    l->Write("histlist_coincidence_LG", TObject::kSingleKey);
    f->ls();
    
    c->cd(0);
    delete hh;
}
