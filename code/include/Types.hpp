#ifndef TYPES_HPP_INCLUDED
#define TYPES_HPP_INCLUDED

#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<algorithm>

#include <boost/algorithm/string.hpp>

using namespace std;

namespace indri
{
namespace query
{

struct query_t
{
    struct greater
    {
        bool operator() ( query_t* one, query_t* two )
        {
            return one->index > two->index;
        }
    };

    query_t( int _index, std::string _number, const std::string& _text, const std::string &queryType,  std::vector<std::string> workSet,   std::vector<std::string> FBDocs) :
        index( _index ),
        number( _number ),
        text( _text ), qType(queryType), workingSet(workSet), relFBDocs(FBDocs)
    {
    }

    query_t( int _index, std::string _number, const std::string& _text ) :
        index( _index ),
        number( _number ),
        text( _text )
    {
    }

    int index;
    std::string number;
    std::string text;
    std::string qType;
    // working set to restrict retrieval
    std::vector<std::string> workingSet;
    // Rel fb docs
    std::vector<std::string> relFBDocs;
};
/*
struct ConceptCandidate
{
    ConceptCandidate(string qId_, string resource_,
        string category_, string conceptName_, double score_)
        :qId(qId_), resource(resource_),
        category(category_), conceptName(conceptName_), score(score_)
    {
    }
    string qId;
    string resource;
    string category;
    string conceptName;
    double score;
};

struct ConceptCandidates
{
    vector<ConceptCandidate> allConcCands;
    void setConceptCands(string qId_, string resource_,
        string category_, string conceptName_, double score_)
    {
        allConcCands.push_back(ConceptCandidate(qId_, resource_, category_, conceptName_, score_));
    }
};
*/
struct OneResourceConceptsParams
{
    // constructors:
    OneResourceConceptsParams(string resourceName_, indri::api::Parameters& param_)
        :resourceName(resourceName_),
         param(param_)
    {
//        cout << "OneResourceConcepts: set parameters of the resourse: " << resourceName << endl;
        if(!param.exists(resourceName))
            throw runtime_error("Types: OneResourceConceptsParams: " + resourceName + " does not exists in the parameters object.");
        string paramKeyValues = param.get(resourceName, "");
        if(paramKeyValues.size()>2)
        {
            cout << "Types: OneResourceConceptsParams: "+resourceName + ": configuration file for this tag is empty" << endl;
//            throw runtime_error("Types: OneResourceConceptsParams: "+resourceName + ": configuration file for this tag is empty");
            parseParamKeyValues(paramKeyValues);
        }
    }
    OneResourceConceptsParams(vector<pair<string, vector<pair<string, string> > > > oneResourceConcepts_,
                              string resourceName_, indri::api::Parameters& param_)
        :oneResourceConcepts(oneResourceConcepts_),
         resourceName(resourceName_),
         param(param_)
    {
//        cout << "OneResourceConcepts: set parameters of the resourse: " << resourceName << endl;
        if(!param.exists(resourceName))
            throw runtime_error("Types: OneResourceConceptsParams: "+resourceName + " does not exists in the parameters object.");
        string paramKeyValues = param.get(resourceName, "");
        if(paramKeyValues.size()>2)
        {
            cout << "Types: OneResourceConceptsParams: "+resourceName + ": configuration file for this tag is empty" << endl;
            parseParamKeyValues(paramKeyValues);
        }
    }

    vector<pair<string, vector<pair<string, string> > > > oneResourceConcepts; // all the pair of (STY, STR) of concepts selected for all the queries
    string resourceName;
    indri::api::Parameters& param;
    vector<tuple<string, double, double, double, double> > allDoubleParams;
    vector<pair<string, string> > allStringParams;

//    string paramKeyValues;

    // lambdaTR, lambdaPH, lambdaPR, ... have values that also exist in
    // param. To avoid confusion, values in param are set to be in higher
    // priority and these parameters in this struct need to be updated before
    // being accessed. The only element of these tuples that can be changed
    // during the program runtime are the first elements and thus they only
    // need to be updated
    tuple<double, double, double, double> lambdaTR;
    tuple<double, double, double, double> lambdaPH;
    tuple<double, double, double, double> lambdaPR;
    tuple<double, double, double, double> lambdaResrc;
    tuple<double, double, double, double> coeff_prf;
    tuple<double, double, double, double> coeff_idf;
    tuple<double, double, double, double> coeff_idfTpRnk;
    tuple<int, int, int, int> fbDocs;
    tuple<int, int, int, int> fbConcs;
    tuple<int, int, int, int> thrshMed; // threshold for determining whether a concept is medical or not
    string conceptDocsDirectory;

    // getter functions:

    double getLambdaTR()
    {
        // getter functions are designed to return members of this struct. However
        // because of the low proiority of this struct, they are updated before
        // getting them to avoid confusion:
        get<0>(lambdaTR) = param.get(resourceName+"_"+"lambdaTR"+"_value", 0.0);
//        cout << "Types: OneResourceConceptsParams: getLambdaTR: resourceName = " << resourceName << endl;
//        cout << "Types: OneResourceConceptsParams: getLambdaTR: get<0>(lambdaTR) = " << get<0>(lambdaTR) << endl;
        return get<0>(lambdaTR);
    }
    double getLambdaPH()
    {
        get<0>(lambdaPH) = param.get(resourceName+"_"+"lambdaPH"+"_value", 0.0);
        return get<0>(lambdaPH);
    }
    double getLambdaPR()
    {
        get<0>(lambdaPR) = param.get(resourceName+"_"+"lambdaPR"+"_value", 0.0);
        return get<0>(lambdaPR);
    }
    double getLambdaResrc()
    {
        get<0>(lambdaResrc) = param.get(resourceName+"_"+"lambdaResrc"+"_value", 0.0);
        return get<0>(lambdaResrc);
    }
    double getCoeff_prf()
    {
    		string paramName_ = resourceName+"_"+"coeff_prf"+"_value";
		if(!param.exists( paramName_ ))
				throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + paramName_ + " does not exist in parameter object.");
        get<0>(coeff_prf) = param.get(paramName_, 0.0);
        return get<0>(coeff_prf);
    }
    double getCoeff_idf()
    {
    		string paramName_ = resourceName+"_"+"coeff_idf"+"_value";
		if(!param.exists( paramName_ ))
				throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + paramName_ + " does not exist in parameter object.");
        get<0>(coeff_idf) = param.get(paramName_, 0.0);
        return get<0>(coeff_idf);
    }
    double getCoeff_idfTpRnk()
    {
    		string paramName_ = resourceName+"_"+"coeff_idfTpRnk"+"_value";
		if(!param.exists( paramName_ ))
				throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + paramName_ + " does not exist in parameter object.");
        get<0>(coeff_idfTpRnk) = param.get(paramName_, 0.0);
        return get<0>(coeff_idfTpRnk);
    }
    int getFbDocs()
    {
        get<0>(fbDocs) = param.get(resourceName+"_"+"fbDocs"+"_value", 0);
        return get<0>(fbDocs);
    }
    int getFbConcs()
    {
        get<0>(fbConcs) = param.get(resourceName+"_"+"fbConcs"+"_value", 0);
        return get<0>(fbConcs);
    }
    double getThrshMed()
    {
        get<0>(thrshMed) = param.get(resourceName+"_"+"thrshMed"+"_value", 0.0);
        return get<0>(thrshMed);
    }


    vector<pair<string, vector<pair<string, string> > > > getOneResourceConcepts()
    {
        // getter function for OneResourceConcepts that provide access to the best fbConcs concepts of all
        // queries but one resource. concepts are not supposed to be changed unless fbDocs is changed.
        // In the case of fbDocs is changed, OneResourceConcepts is reseted from a function defined outside of
        // this struct in the coordinateAscent class.
        int fbConcs_ = std::numeric_limits<int>::max();
        if(resourceName != "umlsOrigMetaMap")
            fbConcs_ = param.get(resourceName+"_"+"fbConcs"+"_value", 0);
//        cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: resourceName = " << resourceName << endl;
//        cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: fbConcs_ = " << fbConcs_ << endl;
//        cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: size of oneResourceConcepts = " << oneResourceConcepts.size() << endl;
        vector<pair<string, vector<pair<string, string> > > > tmp;
        for (auto qc: oneResourceConcepts)
        {
            vector<pair<string, string> > tmp1;
//            cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: qId = " << qc.first << endl;
//            cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: size of concepts in this query = " << qc.second.size() << endl;
            int counter = 0;
            for (auto c: qc.second)
            {
                if(counter++ > fbConcs_)
                    break;
                tmp1.push_back(make_pair(c.first,c.second));
                //cout << "Types: OneResourceConceptsParams: getOneResourceConcepts: concept = " << c.first << ", " << c.second << endl;
            }
            tmp.push_back(make_pair(qc.first, tmp1));
        }
        return tmp;
    }

    void parseParamKeyValues(string paramKeyValues_)
    {
        boost::algorithm::trim(paramKeyValues_);
        cout << "parseParamKeyValues: paramKeyValues_: " << paramKeyValues_ << endl;
        vector<string> allParamKeyValues_v;
        boost::split(allParamKeyValues_v, paramKeyValues_, boost::is_any_of("\n"), boost::token_compress_on);
        for(auto oneParamKeyValues: allParamKeyValues_v)
        {
        	cout << "parseParamKeyValues: paramKeyValues_v: " << oneParamKeyValues << endl;
            vector<string> oneParamKeyValues_v;
            boost::split(oneParamKeyValues_v, oneParamKeyValues, boost::is_any_of(":"), boost::token_compress_on);
            string paramName = oneParamKeyValues_v[0];
            string paramValues = oneParamKeyValues_v[1];
        	cout << "parseParamKeyValues: paramKeyValues_v: (" << paramName << ", " << paramValues << ")" << endl;

            vector<string> paramValues_v;
            boost::split(paramValues_v, paramValues, boost::is_any_of(","), boost::token_compress_on);
            double value = 0.0, upperThreshold = 0.0, lowerThreshold = 0.0, stepSize = 0.0;
            if(paramValues_v.size() == 4)
            {
                value = atof(paramValues_v[0].c_str());
                upperThreshold = atof(paramValues_v[1].c_str());
                lowerThreshold = atof(paramValues_v[2].c_str());
                stepSize = atof(paramValues_v[3].c_str());
                if(lowerThreshold>upperThreshold)
                    throw runtime_error("lowerThreshold > upperThreshold for resource: " + resourceName
                                        + " paramName: "+ paramName + " lowerThreshold: " + to_string(lowerThreshold)
                                        + " upperThreshold: " + to_string(upperThreshold) );
            }
//            cout << value << " " << upperThreshold << " " << lowerThreshold << " " << stepSize << endl;

            // keys like umlsOrigMetaMap_lambdaTR_value do not extractable directly from the configuration file
            // as soon as the part of the configuration file related to lambdas is parsed
            if(paramName == "lambdaTR"|| paramName == "lambdaPH"||
		       paramName == "lambdaPR"|| paramName == "lambdaResrc"||
		       paramName == "coeff_prf"|| paramName == "coeff_idf"||
		       paramName == "coeff_idfTpRnk")
            {
                param.set(resourceName+"_"+paramName+"_value", value);
                param.set(resourceName+"_"+paramName+"_upperThreshold", upperThreshold);
                param.set(resourceName+"_"+paramName+"_lowerThreshold", lowerThreshold);
                param.set(resourceName+"_"+paramName+"_stepSize", stepSize);
            }
            else if(paramName=="fbDocs" || paramName=="fbConcs")
            {
            		if(value>0)
					{
						param.set(resourceName+"_"+paramName+"_value", int(value));
						param.set(resourceName+"_"+paramName+"_upperThreshold", int(upperThreshold));
						param.set(resourceName+"_"+paramName+"_lowerThreshold", int(lowerThreshold));
						param.set(resourceName+"_"+paramName+"_stepSize", int(stepSize));
					}
            	}
            else if(paramName == "conceptDocsDirectory")
            {
                param.set(resourceName+"_"+paramName, paramValues);
            }
			else
		    {
				// it is assumed that the parameters that paramValues_v.size() = 4 and not listed above belongs to a double
				if(paramValues_v.size() == 4)
				{
						param.set(resourceName+"_"+paramName+"_value", value);
						param.set(resourceName+"_"+paramName+"_upperThreshold", upperThreshold);
                		param.set(resourceName+"_"+paramName+"_lowerThreshold", lowerThreshold);
                		param.set(resourceName+"_"+paramName+"_stepSize", stepSize);
						cout << "Types: OneResourceConceptsParams: set double parameters of: " << param.get("htPrf_thrshMed_value",0.0) << endl;
				}
				else
				{
						cout << "Types: OneResourceConceptsParams: set string parameters of: " << resourceName+"_"+paramName+": " << paramValues << endl;
						param.set(resourceName+"_"+paramName, paramValues);
				}
			}

            if(paramName == "lambdaTR")
            {
                lambdaTR = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            }
            else if(paramName == "lambdaPH")
                lambdaPH = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "lambdaPR")
                lambdaPR = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "lambdaResrc")
                lambdaResrc = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "coeff_prf")
                coeff_prf = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "coeff_idf")
                coeff_idf = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "coeff_idfTpRnk")
                coeff_idfTpRnk = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else if(paramName == "fbDocs")
                fbDocs = make_tuple(int(value), int(upperThreshold), int(lowerThreshold), int(stepSize));
            else if(paramName == "fbConcs")
                fbConcs = make_tuple(int(value), int(upperThreshold), int(lowerThreshold), int(stepSize));
            else if(paramName == "conceptDocsDirectory")
                conceptDocsDirectory = paramValues;
            else if(paramName == "thrshMed")
                thrshMed = make_tuple(value, upperThreshold, lowerThreshold, stepSize);
            else
                cout << "warning: Types::OneResourceConceptsParams: parameter key-values of \n " + paramKeyValues_ + "\nhas key: ->|" + paramName + "|<- but not included in this object. It is only applied to param object.";
                //throw runtime_error("parameter key-values of \n " + paramKeyValues_ + "\nhas key: ->|" + paramName + "|<-");
//            cout << "---" << endl;
            cout << "parseParamKeyValues: "+ resourceName+"_"+paramName+"_value: " << param.get(resourceName+"_"+paramName+"_value", 0.0) << endl;
		if(value>0)
		{
            if(resourceName+"_"+paramName+"_value" == "unigramsOrig_fbDocs_value")
            {
                param.set("fbDocs", value );
                cout << "parseParamKeyValues: fbDocs = " << param.get("fbDocs", 0.0 ) << endl;
            }
            else if(resourceName+"_"+paramName+"_value" == "unigramsOrig_fbConcs_value")
            {
                param.set("fbTerms", value );
                cout << "parseParamKeyValues: fbTerms = " << param.get("fbTerms", 0.0) << endl;
            }

        }
	}
//        cout << "paramKeyValues_ are set." << endl;
    }

};

}
}

#endif // TYPES_HPP_INCLUDED
