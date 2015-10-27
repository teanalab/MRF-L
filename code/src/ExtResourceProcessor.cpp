//
// ExtResourceProcessor
//
// 6 Jul 2015 -- sbk
//

#include <math.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <utility>

#include "../include/ExtResourceProcessor.hpp"

indri::query::ExtResourceProcessor::ExtResourceProcessor(indri::api::Parameters& param)
    :_param ( param )
{
}

map<string, set<string> > indri::query::ExtResourceProcessor::readWikiMedicalEntities()
{
    indri::collection::Repository r;
    string repName = _param.get("index", "");
    r.openRead( repName );

    cout << "reading wikiMedicalEntities..." << endl;
    map<string, set<string> > wikiMedicalTermsExpr; // single terms vs all the expression contains them


    string fileName = _param.get("wikiMedicalEntities","");
    ifstream inFile(fileName.c_str());
    if(!inFile) throw runtime_error("error in opening STYs file: " + fileName);

    string entityStr;
    while (getline(inFile, entityStr))
    {
        vector<string> entityStrV;
        boost::split(entityStrV, entityStr, boost::is_any_of(" "));
        for (auto s: entityStrV)
        {
            wikiMedicalTermsExpr[r.processTerm(s)].insert(entityStr);
        }
    }
//    for (auto c: wikiMedicalTermsExpr)
//    {
//        cout << "Entity: " << c.first;
//        for (auto w: c.second)
//        {
//            cout << " --- " << w << endl;
//        }
//    }
    cout << "size of wikiMedicalTermsExpr: " << wikiMedicalTermsExpr.size() << endl;

    r.close();
    return wikiMedicalTermsExpr;
}


pair<map<string, string>, map<string, string> > indri::query::ExtResourceProcessor::readSTYs()
{
//    cout << "reading STYs..." << endl;
    map<string, string> STYs;
    map<string, string> styAbbrsTuis;

    string tmp;

    string fileName = _param.get("STYs","");
    ifstream inFile(fileName.c_str());
    if(!inFile) throw runtime_error("error in opening STYs file: " + fileName);

    while (std::getline( inFile, tmp ))
    {
        string styAbbr, STY, TUI;
        stringstream line_ss(tmp);
        std::getline( line_ss, styAbbr, '|' );
        std::getline( line_ss, TUI, '|' );
        std::getline( line_ss, STY, '|' );
        std::transform(STY.begin(), STY.end(), STY.begin(), ::tolower);
        STY.erase(std::remove(STY.begin(), STY.end(), ','), STY.end());

        STYs.insert(make_pair(styAbbr, STY));
        styAbbrsTuis.insert(make_pair(TUI, styAbbr));
    }

//    for (auto c: STYs)
//    {
//        cout << "STY:--> " << c.first << " --> " << c.second << endl;
//    }
    cout << "size of STYs: " << STYs.size() << endl;

    return make_pair(STYs, styAbbrsTuis);
}

map<string, vector< pair<string, int> > > indri::query::ExtResourceProcessor::read_MRREL_CUI1_CUI2()
{
    string fileName = _param.get("MRREL_CUI1_CUI2","");
    cout <<  "reading MRREL_CUI1_CUI2 file: " << fileName <<endl;
    ifstream inFile(fileName.c_str());
    map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2;
    if (!inFile)
    {
        throw runtime_error( "Error in reading file: " + _param.get("MRREL_CUI1_CUI2", "") );
    }
    string line;
    while (getline(inFile,line))
    {
        stringstream line_ss(line);
        string CUI1;
        getline(line_ss,CUI1,';');
//        line_ss >> CUI1;
        string pairs;
        vector< pair<string, int> > pairs_v;
        while (getline(line_ss,pairs,';'))
        {
            stringstream pairs_ss(pairs);
            string CUI2;
            int freq;
            pairs_ss >> CUI2 >> freq;
            pairs_v.push_back(std::make_pair(CUI2, freq));
        }
        MRREL_CUI1_CUI2.insert(pair<string, vector< pair<string, int> > >(CUI1, pairs_v));
    }
    cout << "indri::query::ExtResourceProcessor::read_MRREL_CUI1_CUI2: size of MRREL_CUI1_CUI2 = " << MRREL_CUI1_CUI2.size() << endl;
    return MRREL_CUI1_CUI2;
}


map<string, set<string> > indri::query::ExtResourceProcessor::readMRSTY_simp()
{
//	cout << "reading simplified version of MRSTY..." << endl;
    std::map<std::string, std::set<std::string> > MRSTY_simp_v;

    // get the good symantic types:
    ifstream inFile(_param.get("MRSTY_simp", "").c_str());
    if (!inFile)
    {
        throw runtime_error( "Error in reading file: " + _param.get("MRSTY_simp", "") );
    }

    string line;
    while(getline(inFile, line))
    {
        stringstream line_ss(line);
        string CUI;
        line_ss >> CUI;
        string TUI;
        set<string> TUI_s;
        while (line_ss >> TUI)
        {
            TUI_s.insert(TUI);
        }
        MRSTY_simp_v.insert(make_pair(CUI, TUI_s));
    }
//    for (auto c: MRSTY_simp_v)
//    {
//        cout << "STY:--> " << c.first << endl;
//        for (auto t: c.second)
//            cout << " " << t ;
//        cout << endl;
//    }
    std::cout << "MRSTY_simp_v.size() = " << MRSTY_simp_v.size() << std::endl;

    return MRSTY_simp_v;
}

map<string, string> indri::query::ExtResourceProcessor::readMRCONSO_simp()
{
    map<string, string> MRCONSO_simp;

    string fileName = _param.get("MRCONSO_simp", "");
    ifstream inFile(fileName.c_str());
    if (!inFile)
    {
        throw runtime_error( "Error in reading file: " + fileName );
    }

    string line;
//    while (inFile >> CUI >> STR)
    while (getline(inFile,line))
    {
//        string CUI, STR;
        vector<string> lineV;
        boost::split(lineV, line, boost::is_any_of("\t"));
        MRCONSO_simp.insert(make_pair(lineV[0], lineV[1]));
    }

    return MRCONSO_simp;
}
