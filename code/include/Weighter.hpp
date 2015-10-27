
//
// Weighter
//
// 13 Jul 2015 -- sbk
//

#ifndef WEIGHTER_HPP
#define WEIGHTER_HPP

#include <string>
#include <vector>
#include <map>

#include "indri/QueryEnvironment.hpp"
#include "indri/Parameters.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
using namespace std;

namespace indri
{
namespace query
{

class Weighter
{
public:
    Weighter( indri::api::QueryEnvironment * env, indri::api::Parameters& params,
            map<string, vector< pair<string, int> > > & MRREL_CUI1_CUI2, indri::index::Index* index);
    ~Weighter();
/*
    double calcLambda_k(string qTerm,
                    const map<string, map<int, int> > & trDocCount,
                    const map<string, map<int, int> > & exprCountsUw,
                    const map<string, map<int, int> > & exprCountsOd);


    map<int, int> term_positions( const std::string& termString,
                                    const map<string, map<int, int> > & trDocCount);

    void calcOccur(map<string, map<int, int> > & trDocCount, vector<string> queryVector_);
    void calcOccur(vector<string> queryVector_,
                    map<string, map<int, int> > & trDocCount,
                    map<string, map<int, int> > & exprCountsUw,
                    map<string, map<int, int> > & exprCountsOd);
    map<int, int> expression_list( const std::string& expression,
                    const map<string, map<int, int> > & trDocCount,
                    const map<string, map<int, int> > & exprCountsUw,
                    const map<string, map<int, int> > & exprCountsOd);
*/

private:
    indri::api::QueryEnvironment * _environment;
    indri::api::Parameters& _parameters;
    map<string, vector< pair<string, int> > > & _MRREL_CUI1_CUI2;
    indri::index::Index* _index;

private:

    vector<pair<string, vector<tuple<string, string, string> > > > metamapConverted;
};
}
}

#endif
