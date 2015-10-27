//
// CoordinateAscent
//
// 30 Jun 2015 -- sbk
//

#include <math.h>
#include <stdio.h>
#include <sstream>
#include <functional>
#include <cctype>
#include <locale>

#include "../include/CoordinateAscent.hpp"

indri::query::CoordinateAscent::CoordinateAscent(indri::api::Parameters& param)
    :_param ( param ), _ruleParam ( _param ), _stepSizeParams( _param )
{
    wsuIr::expander::Utility::parameters_parse(_ruleParam, _param["rule"] );
//    wsuIr::expander::Utility::findDocumentNames(_param);

    extResourceProcessor = new indri::query::ExtResourceProcessor(param);
    wikiMedicalTermsExpr = extResourceProcessor->readWikiMedicalEntities();
    pair<map<string, string>, map<string, string> > STYsTUIs = extResourceProcessor->readSTYs();
    STYs = STYsTUIs.first;
    styAbbrsTuis = STYsTUIs.second;
    MRSTY_simp = extResourceProcessor->readMRSTY_simp();

    MRCONSO_simp = extResourceProcessor->readMRCONSO_simp();

    parseSetResourceNames();

    for (string resrc: resourceNames)
    {
        cout << "indri::query::CoordinateAscent::CoordinateAscent: pushing back OneResourceConceptsParams object into allResourceConceptsParams for: " << resrc << endl;
        allResourceConceptsParams.push_back(new OneResourceConceptsParams(resrc, _param));
    }
    // if it is necessary to read MRREL_CUI1_CUI2
    if(std::find(resourceNames.begin(), resourceNames.end(), "umlsRelated")!=resourceNames.end())
        MRREL_CUI1_CUI2 = extResourceProcessor->read_MRREL_CUI1_CUI2();

    queryReformulator = new indri::query::QueryReformulator(_param, allResourceConceptsParams);

    run();

}

void indri::query::CoordinateAscent::parseSetResourceNames()
{
    vector<string> secondaryResourceNames;
    vector<string> optResourceNames_tmp;

    string primaryResourceNames_ = _param.get("primaryResourceNames", "");
    string secondaryResourceNames_ = _param.get("secondaryResourceNames", "");
    string optResourceNames_ = _param.get("optResourceNames", "");

    boost::trim(primaryResourceNames_);
    boost::trim(secondaryResourceNames_);
    boost::trim(optResourceNames_);

    if(primaryResourceNames_.size()>0) boost::split(primaryResourceNames, primaryResourceNames_, boost::is_any_of("\n"), boost::token_compress_on);
    if(secondaryResourceNames_.size()>0) boost::split(secondaryResourceNames, secondaryResourceNames_, boost::is_any_of("\n"), boost::token_compress_on);
    if(optResourceNames_.size()>0) boost::split(optResourceNames_tmp, optResourceNames_, boost::is_any_of("\n"), boost::token_compress_on);

    for(auto optResrc: optResourceNames_tmp)
    {
        vector<string> optResrce_tmp;
        if(optResrc.size()>0) boost::split(optResrce_tmp, optResrc, boost::is_any_of(":"), boost::token_compress_on);
        string resrcName = optResrce_tmp[0];
        if(optResrce_tmp.size()>1)
        {
            string optParams = optResrce_tmp[1];
            boost::trim(optParams);
            if(optParams.size()>0)
            {
                vector<string> optParams_tmp;
                boost::split(optParams_tmp, optParams, boost::is_any_of(","), boost::token_compress_on);
                optResourceNamesParams.push_back(make_pair(resrcName, optParams_tmp));
            }
        }
    }

    resourceNames = primaryResourceNames;

    resourceNames.insert(resourceNames.end(), secondaryResourceNames.begin(), secondaryResourceNames.end());

    for(string resrc: secondaryResourceNames)
        cout << "indri::query::CoordinateAscent::parseSetResourceNames: secondaryResourceNames: " << resrc << endl;
    for(string resrc: primaryResourceNames)
        cout << "indri::query::CoordinateAscent::parseSetResourceNames: primaryResourceNames: " << resrc << endl;
    for(string resrc: resourceNames)
        cout << "indri::query::CoordinateAscent::parseSetResourceNames: resourceNames: " << resrc << endl;
    for(auto oneResrcParams: optResourceNamesParams)
        for(string oneResrcParam: oneResrcParams.second)
            cout << "indri::query::CoordinateAscent::parseSetResourceNames: optResourceNamesParams: " << oneResrcParams.first
                    << ": " << oneResrcParam << endl;

}

indri::query::CoordinateAscent::~CoordinateAscent()
{
    delete queryReformulator;
    delete extResourceProcessor;
    for (auto rsrc: allResourceConceptsParams)
        delete rsrc;
//    delete bigramsPrfTermMatchConceptSelector;
//    delete umlsPrfConceptSelector;
}

void indri::query::CoordinateAscent::QueryThread_(vector<string> resourceNames_, vector<pair<string, string> > queriesTexts)
{
    string textOrigParam;
    _param.write(  textOrigParam );

    size_t threadCount = _param.get( "threads", 1 );
    bool trecFormat = _param.get("trecFormat", false);
    string rule = _param.get("rule", "");
    size_t count_ = _param.get("count", 0);
    size_t fbDocs = _param.get("fbDocs", 0);
    size_t fbTerms = _param.get("fbTerms", 0);

    if( _param.get( "version", 0 ) )
        std::cout << INDRI_DISTRIBUTION << std::endl;

    if( !_param.exists( "query" ) )
        LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify at least one query." );

    if( !_param.exists("index") && !_param.exists("server") )
        LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a server or index to query against." );

    if (_param.exists("baseline") && _param.exists("rule"))
        LEMUR_THROW( LEMUR_BAD_PARAMETER_ERROR, "Smoothing rules may not be specified when running a baseline." );

    // push all queries onto a queue
    indri::api::Parameters parameterQueries = _param[ "query" ];
    int queryOffset = _param.get( "queryOffset", 0 );

    std::queue< indri::query::query_t* > _queries_;
    wsuIr::expander::Utility::push_queue_crv( _queries_, parameterQueries, queryOffset, _param );

    int queryCount = (int)_queries_.size();

    cout << "indri::query::CoordinateAscent::QueryThread_: queryCount = " << queryCount << endl;

    if(queryCount == 0)
        throw runtime_error("there is no query. (queryCount = 0)");

    if(queriesTexts.size() == 0) // in the default case, the size of this vector is zero
    {
        queryReformulator->setQueries(_queries_);
        queriesTexts = queryReformulator->reformulateQuery(resourceNames_, STYs, wikiMedicalTermsExpr, MRREL_CUI1_CUI2, MRCONSO_simp, MRSTY_simp, styAbbrsTuis );
    }

    string queryTextTemp = "";
    queryTextTemp += "<parameters>\n";
    queryTextTemp += "<index>"+_param.get("index","")+"</index>\n";

    cout << "indri::query::CoordinateAscent::QueryThread_: queryCount: " << queryCount << endl;
    int counter = 0;
    for (int i = 0; i < queryCount; i++)
    {
        query_t* query_t_ = _queries_.front();
        _queries_.pop();

        string text_ = queriesTexts[counter].second;
        if (query_t_->text.size()<3)
            continue;


        if(query_t_->number != queriesTexts[counter].first)
            throw runtime_error("indri::query::CoordinateAscent::QueryThread_: query_t_->number != queriesTexts[counter].first : " + query_t_->number + " != " + queriesTexts[counter].first );

        queryTextTemp += "<query>\n<number>" + query_t_->number + "</number>\n";
        queryTextTemp += "<text>\n" + text_ + "</text>\n</query>\n";
        counter++;
        _queries_.push(query_t_);
    }
    queryTextTemp += "<rule>" + rule + "</rule>\n";
    queryTextTemp += "<count>" + to_string(count_) + "</count>\n";
    queryTextTemp += "<fbDocs>" + to_string(fbDocs) + "</fbDocs>\n";
    queryTextTemp += "<fbTerms>" + to_string(fbTerms) + "</fbTerms>\n";

    if(trecFormat)
    {
        queryTextTemp += "<trecFormat>true</trecFormat>\n";
    }
    else
    {
        queryTextTemp += "<trecFormat>false</trecFormat>\n";
    }
    queryTextTemp += "<threads>" + to_string(threadCount) + "</threads>\n";
    queryTextTemp += "\n</parameters>\n";
    cout << queryTextTemp;

//    string textOrigParam;
//    _param.write(  textOrigParam );

    _param.clear();
    _param.load( queryTextTemp );

    if( _param.get( "version", 0 ) )
        std::cout << INDRI_DISTRIBUTION << std::endl;

    if( !_param.exists( "query" ) )
        LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify at least one query." );

    if( !_param.exists("index") && !_param.exists("server") )
        LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a server or index to query against." );

    if (_param.exists("baseline") && _param.exists("rule"))
        LEMUR_THROW( LEMUR_BAD_PARAMETER_ERROR, "Smoothing rules may not be specified when running a baseline." );

    threadCount = _param.get( "threads", 1 );

    std::queue< indri::query::query_t* > queries;
    std::priority_queue< indri::query::query_t*, std::vector< indri::query::query_t* >, indri::query::query_t::greater > output;
    std::vector< indri::thread::QueryThread* > threads;
    indri::thread::Mutex queueLock;
    indri::thread::ConditionVariable queueEvent;

    indri::api::Parameters parameterQueriesTemp = _param[ "query" ];
    queryOffset = _param.get( "queryOffset", 0 );
    wsuIr::expander::Utility::push_queue( queries, parameterQueriesTemp, queryOffset );
    queryCount = (int)queries.size();

    for( size_t i=0; i<threadCount; i++ )
    {
        threads.push_back( new indri::thread::QueryThread( queries, output, queueLock, queueEvent, _param) );
        threads.back()->start();
    }

    cout << "indri::query::CoordinateAscent::QueryThread_: calculating precision..." << endl;
    map<string, double> _trec_eval_m = wsuIr::expander::Utility::get_trec_eval();

    int query = 0;

    bool inexFormat = _param.exists( "inex" );
    if( inexFormat )
    {
        std::string participantID = _param.get( "inex.participantID", "1");
        std::string runID = _param.get( "runID", "indri" );
        std::string inexTask = _param.get( "inex.task", "CO.Thorough" );
        std::string inexTopicPart = _param.get( "inex.topicPart", "T" );
        std::string description = _param.get( "inex.description", "" );
        std::string queryType = _param.get("inex.query", "automatic");
        std::cout << "<inex-submission participant-id=\"" << participantID
                << "\" run-id=\"" << runID
                << "\" task=\"" << inexTask
                << "\" query=\"" << queryType
                << "\" topic-part=\"" << inexTopicPart
                << "\">" << std::endl
                << "  <description>" << std::endl << description
                << std::endl << "  </description>" << std::endl;
    }

    // acquire the lock.
    queueLock.lock();

    // process output as it appears on the queue
    while( query < queryCount )
    {
        indri::query::query_t* result = NULL;

        // wait for something to happen
        queueEvent.wait( queueLock );

        while( output.size() && output.top()->index == query )
        {
            result = output.top();
            output.pop();

            queueLock.unlock();

            std::cout << result->text;
            delete result;
            query++;

            queueLock.lock();
        }
    }
    queueLock.unlock();

    if( inexFormat )
    {
        std::cout << "</inex-submission>" << std::endl;
    }

    // join all the threads
    for( size_t i=0; i<threads.size(); i++ )
        threads[i]->join();

    // we've seen all the query output now, so we can quit
    indri::utility::delete_vector_contents( threads );

    _param.clear();
    _param.load( textOrigParam );

}

template <typename T>
tuple<string, string, string, T, T, T, T> indri::query::CoordinateAscent::makeTuple(string resourceName_, string dType, string paramName)
{
    T stepSize = _param.get(resourceName_+"_"+paramName+"_stepSize", static_cast<T>(0.0));
    T value = _param.get(resourceName_+"_"+paramName+"_value", static_cast<T>(0.0));
    T lowerThreshold = _param.get(resourceName_+"_"+paramName+"_lowerThreshold", static_cast<T>(0.0));
    T upperThreshold = _param.get(resourceName_+"_"+paramName+"_upperThreshold", static_cast<T>(0.0));
    return make_tuple("param", resourceName_+"_"+paramName+"_value", dType,
            stepSize,
            value,
            lowerThreshold,
            upperThreshold
                     );
}

void indri::query::CoordinateAscent::run()
{

    if (_param.get( "train", false ))
    {
        cout << "run: training step..." << endl;
        // generating vector of tuples of parameters that are subject to be involoved in the
        // optimization:
        vector< tuple< string, string, string, double, double, double, double> > vt;
        for(auto oneResourceNameParams: optResourceNamesParams)
        {
            string resrcName = oneResourceNameParams.first;
            for(auto oneParam: oneResourceNameParams.second)
            {
                if(resrcName == "fbDocs" || resrcName == "fbConcs" )
                    vt.push_back(makeTuple<double>(resrcName, "int", oneParam));
                else
                    vt.push_back(makeTuple<double>(resrcName, "double", oneParam));
            }

        }


        double trec_eval_avg_=0;
        // the direction is chosen in the sequence vector vt is stored (it is a non-random predefined way):
        optimization(vt, trec_eval_avg_);
        cout << "run: tuples:" << endl;
        for(auto v: vt)
            cout << "indri::query::CoordinateAscent::run: the params obtaied after one step of optimization for one parameters."
                    << get<0>(v) << " " << get<1>(v) << " " << get<2>(v) << " "
                    << get<3>(v) << " " << get<4>(v) << " " << get<5>(v) << " "
                    << get<6>(v) << endl;
        cout << endl;
    }
    else
    {
        cout << "run: testing step..." << endl;
        // primaryResourceNames are first applied to obtain top-ranked documents:
        runAndCalcPrecision(primaryResourceNames);
        runAndCalcPrecision(resourceNames);
    }
}

double indri::query::CoordinateAscent::runAndCalcPrecision(vector<string> resourceNames_)
{
    for (string resrc: resourceNames_)
        cout << "indri::query::CoordinateAscent::runAndCalcPrecision: resource name = " + resrc << endl;

    QueryThread_(resourceNames_);

    wsuIr::expander::Utility::calcTrecPrecisionInfNdcg(_param);

    double trec_eval_avg_ = wsuIr::expander::Utility::trec_eval_avg();

    cout << "runAndCalcPrecision: trec_eval_avg_ = " << trec_eval_avg_ << endl;

    return trec_eval_avg_;
}

void indri::query::CoordinateAscent::optimization( vector< tuple<string, string, string, double, double, double, double> >& vt,
        double& trec_eval_avg_)
{

//    int counter = 0;
    double trec_eval_avg_tmp1 = 0.05;
    double trec_eval_avg_tmp2 = 0;

//    vector<string> primaryResourceNames_ = vector<string>{"unigramsOrig"};
//    double trec_eval_avg_base = runAndCalcPrecision(vector<string>{"unigramsOrig"});
    cout << "indri::query::CoordinateAscent::optimization: obtaining the top-ranked documents to later use in the PRF process:" << endl;
//    runAndCalcPrecision(vector<string>{"unigramsOrig"});
    runAndCalcPrecision(primaryResourceNames);
    double threshold = _param.get("optThreshold", 0.001);

    cout << "indri::query::CoordinateAscent::optimization: obtaining the precision for the initial values given in the configuration file." << endl;
    tuple<string, string, string, double, double, double, double> t = make_tuple("", "", "", 0.0, 0.0, 0.0, 0.0);
    map<double, double> precisionsHist;
    trec_eval_avg_ = optimization_(t, 0, precisionsHist, resourceNames); // with no change, because of direction = 0

    vector<double> precisionsRecord;
    while( trec_eval_avg_tmp1 - trec_eval_avg_tmp2 > threshold ) // the second stopping criteria
    {
//        trec_eval_avg_tmp2 = trec_eval_avg_;
        trec_eval_avg_tmp2 = trec_eval_avg_tmp1;
        for(auto it_t = vt.begin(); it_t != vt.end(); it_t++)
        {
            cout << "optimization_before_dir_finding: tuples:" << endl;
            for(auto v: vt)
                cout << get<0>(v) << " " << get<1>(v) << " " << get<2>(v) << " "
                        << get<3>(v) << " " << get<4>(v) << " " << get<5>(v) << " "
                        << get<6>(v) << endl;

            precisionsHist.clear();
            //int direction = findDirection( *it_t , trec_eval_avg_, precisionsHist, resourceNames );
            int direction = findDirection( *it_t, precisionsHist, resourceNames );
            tuple<string, string, string, double, double, double, double> t_tmp1 = *it_t;
            tuple<string, string, string, double, double, double, double> t_tmp2 = *it_t;
            cout << "direction = " << direction << endl;
            cout << "trec_eval_avg_ = " << trec_eval_avg_ << endl;
            cout << "trec_eval_avg_tmp1 = " << trec_eval_avg_tmp1 << endl;
            cout << "trec_eval_avg_tmp2 = " << trec_eval_avg_tmp2 << endl;
//            cout << "optimization_after_dir_finding: tuples:" << endl;

            if (direction != 0)
            {

                cout << "optimization_after_dir_finding: tuples:" << endl;
                for(auto v: vt)
                    cout << get<0>(v) << " " << get<1>(v) << " " << get<2>(v) << " "
                            << get<3>(v) << " " << get<4>(v) << " " << get<5>(v) << " "
                            << get<6>(v) << endl;

                precisionsRecord.clear(); // clear it for the new parameters
                while( ( (trec_eval_avg_ - trec_eval_avg_tmp1) > -0.2 ) && // is the precision increasing
                        ( std::get<4>(t_tmp1) + direction*std::get<3>(t_tmp1) <= std::get<6>(t_tmp1) ) &&// is the value below higher threshold
                        ( std::get<4>(t_tmp1) + direction*std::get<3>(t_tmp1) >= std::get<5>(t_tmp1) ) && // is the value above the lower threshold
//                        ( std::get<3>(t_tmp1) > 0 ) &&
                        derivativeWatchDog(precisionsRecord, direction)
                     ) // the first stopping criteria
                {
//
//                    cout << "value, uTh = " << std::get<4>(t_tmp1) << " <= " <<  std::get<6>(t_tmp1) << endl;
//                    cout << "value, lTh = " << std::get<4>(t_tmp1) << " >= " <<  std::get<5>(t_tmp1) << endl;
//                    cout << "( std::get<4>(t_tmp1) <= std::get<6>(t_tmp1) ) = " << ( std::get<4>(t_tmp1) <= std::get<6>(t_tmp1) ) << endl;
//                    cout << "( std::get<4>(t_tmp1) >= std::get<5>(t_tmp1) ) = " << ( std::get<4>(t_tmp1) >= std::get<5>(t_tmp1) ) << endl;

                    if(trec_eval_avg_ > trec_eval_avg_tmp1)
                    {
                        *it_t = t_tmp1; // it is in approperate track, so save it
                        t_tmp2 = t_tmp1;
                        trec_eval_avg_tmp1 = trec_eval_avg_;
                    }

                    trec_eval_avg_ = optimization_( t_tmp1, direction, precisionsHist, resourceNames );
                    precisionsRecord.push_back(trec_eval_avg_);
                    cout << "optimization_after_one_step_of_optimization: tuples:" << endl;
                    for(auto v: vt)
                        cout << get<0>(v) << " " << get<1>(v) << " " << get<2>(v) << " "
                                << get<3>(v) << " " << get<4>(v) << " " << get<5>(v) << " "
                                << get<6>(v) << endl;
                }
                apply_t_2_param( t_tmp2 ); // update param
            }
        }
        break;
    }
}

bool indri::query::CoordinateAscent::derivativeWatchDog(vector<double> precisionsRecord, int direction)
{
    int size_ = precisionsRecord.size();
    cout << "precisionsRecord.size() = " << size_ << endl;
    // https://en.wikipedia.org/wiki/Finite_difference_coefficient
    if(size_ > 7)
    {
        double derivate = direction * (
                1/6.0     * precisionsRecord[size_-7] - 6/5.0  * precisionsRecord[size_-6]
                + 15/4.0  * precisionsRecord[size_-5] - 20/3.0 * precisionsRecord[size_-4]
                + 15/2.0  * precisionsRecord[size_-3] - 6.0    * precisionsRecord[size_-2]
                + 49/20.0 * precisionsRecord[size_-1] );
        cout << "precision record (up to 7. the first one is the current value) = ";
        for (int i = 1; i < 6; i++ ) cout << precisionsRecord[size_-i] << " ";
        cout << "\nderivate = " << derivate << endl;
        if (derivate <= 0.00001)
            return false;
    }
    return true;
}

//int indri::query::CoordinateAscent::findDirection(tuple<string, string, string, double, double, double, double> & t, double & trec_eval_avg, map<double, double> & precisionsHist, vector<string> resourceNames_)
int indri::query::CoordinateAscent::findDirection(tuple<string, string, string, double, double, double, double> & t, map<double, double> & precisionsHist, vector<string> resourceNames_)
{
    vector<int> directions = {-3, -2, -1, 1, 2, 3};
    vector<double> precisions;

    int direction_opt = 0;
    for(auto direction: directions)
    {
        tuple<string, string, string, double, double, double, double> t_ = t;
        if ( ( std::get<4>(t_) + direction*std::get<3>(t_) <= std::get<6>(t_) ) &&// is the value below higher threshold
                ( std::get<4>(t_) + direction*std::get<3>(t_) >= std::get<5>(t_) ) ) // is the value above the lower threshold
        {
            double trec_eval_avg_ = optimization_(t_, direction, precisionsHist, resourceNames_);
            double paramValue = std::get<4>(t_);
            precisions.push_back(trec_eval_avg_);
            precisionsHist.insert(make_pair(paramValue, trec_eval_avg_));
            direction_opt = direction;
        }
        else
        {
            std::get<4>(t) += direction_opt*std::get<3>(t);
            apply_t_2_param( t );
            cout << "Warning: It is suggested to choose initial values for the parameter \"" + std::get<1>(t_) + "\" that is at least " + to_string(std::get<5>(t_)+3*std::get<3>(t_)) + " and at most " + to_string(std::get<6>(t_)-3*std::get<3>(t_)) << endl;
            if ( direction > 0 ) return -1;
            else if( direction < 0 ) return 1;
            else return 0;

        }
    }
    // https://en.wikipedia.org/wiki/Finite_difference_coefficient
    double direction_tmp = -1/60.0*precisions[0]   + 3/20.0*precisions[1]
            -3/4.0*precisions[2]  + 3/4.0*precisions[3]
            -3/20.0*precisions[4] + 1/60.0*precisions[5];

    cout << "direction = " << direction_tmp << endl;

    if ( direction_tmp > 0.00001 ) return 1;
    else if( direction_tmp < -0.00001 ) return -1;
    else return 0;

    // TODO: return calculated precisions to avoid recalculating them
}

void indri::query::CoordinateAscent::apply_t_2_param(tuple<string, string, string, double, double, double, double> t)
{
    string paramOrRuleParams = std::get<0>(t);
    string paramName         = std::get<1>(t);
    string intOrDouble       = std::get<2>(t);
    double paramValue        = std::get<4>(t);
    cout << "apply_t_2_param: " << intOrDouble << " " << paramOrRuleParams << " " << paramName << " " << paramValue << endl;
    assignValue2Param(intOrDouble, paramOrRuleParams, paramName, paramValue, t);
}

double indri::query::CoordinateAscent::optimization_( tuple<string, string, string, double, double, double, double> & t,
        int direction, map<double, double> & precisionsHist, vector<string> resourceNames_ )
{
//    vector<indri::query::OneResourceConceptsParams> allResourceConceptsParams={
//        OneResourceConceptsParams("umlsPrfMetaMap", _param),
//        OneResourceConceptsParams("bigramsPrfTermMatch", _param),
//        };

    string paramOrRuleParams = "";
    string paramName         = "";
    string intOrDouble       = "";
    double stepSize          = 0;
    double paramValue        = 0;


    if (direction != 0)
    {
        paramOrRuleParams = std::get<0>(t);
        paramName         = std::get<1>(t);
        intOrDouble       = std::get<2>(t);
        stepSize          = std::get<3>(t);
        paramValue        = std::get<4>(t);
//        cout << "indri::query::CoordinateAscent::optimization_: paramName = " << paramName << endl;
//        cout << "indri::query::CoordinateAscent::optimization_: paramValue (before) = " << paramValue << endl;
        paramValue += (direction * stepSize);
        std::get<4>(t) = paramValue;
        assignValue2Param(intOrDouble, paramOrRuleParams, paramName, paramValue, t);
    }
    double trec_eval_avg_;

    auto itFound = precisionsHist.find(paramValue);
    if(itFound != precisionsHist.end())
    {
        trec_eval_avg_ = itFound->second;
        cout << paramValue << " already calculated: " << trec_eval_avg_ << endl;
    }
    else
    {
//        QueryThread_();
        runAndCalcPrecision(resourceNames_);
//    string text_trec_rel_ = indri::query::ConceptSelector::read_trec_rel(_param);
//    string text_results_ = "";
//    vector<string> trec_eval_argv = {" ", "-m", "map", text_trec_rel_, text_results_};
        wsuIr::expander::Utility::calcTrecPrecisionInfNdcg(_param);

        trec_eval_avg_ = wsuIr::expander::Utility::trec_eval_avg();
        cout << paramValue << " were not calculated: " << trec_eval_avg_ << endl;
        for (auto pH: precisionsHist)
        {
            cout << "precisionsHist: " << pH.first << " " << pH.second << endl;
        }
        precisionsHist.insert(make_pair(paramValue, trec_eval_avg_));
    }
    cout << "paramOrRuleParams = " << paramOrRuleParams << endl
            << " paramName = " << paramName << endl
            << " intOrDouble = " << intOrDouble << endl
            << " stepSize = " << stepSize << endl
            << " paramValue (after) = " << paramValue << endl
            << " paramValue (from param) = " << _param.get(paramName, 0.8989) << endl
            << " trec_eval_avg_ = " << trec_eval_avg_ << endl
            << " direction = " << direction << endl;

    vector<tuple<string, double, double> > optimizationHistoryTmp = {make_tuple(paramName,
                                                                     paramValue, trec_eval_avg_)
                                                                    };
    optimizationHistory.insert( optimizationHistory.end(),
            optimizationHistoryTmp.begin(), optimizationHistoryTmp.end() );

//    indri::query::ConceptSelector::print_trec_eval();

    printHistoryLce();

//    indri::query::ConceptSelector::initialize();
    return trec_eval_avg_;
}

void indri::query::CoordinateAscent::assignValue2Param(string intOrDouble, string paramOrRuleParams,
        string paramName, double paramValue, tuple<string, string, string, double, double, double, double> t)
{
    cout << "indri::query::CoordinateAscent::assignValue2Param: params: " << intOrDouble << " " << paramOrRuleParams << " " << paramName << ": " << paramValue << endl;

    if ( intOrDouble == "int")
        _param.set(paramName, int( paramValue ) );
    else
        _param.set(paramName, paramValue );

    if( paramName == "unigramsOrig_fbDocs_value" )
        _param.set("fbDocs", paramValue );
    else if( paramName == "unigramsOrig_fbConcs_value" )
        _param.set("fbTerms", paramValue );

    cout << "indri::query::CoordinateAscent::assignValue2Param: _param.get(paramName, 0 ) = " << _param.get(paramName, 0.0 ) << endl;

    queryReformulator->applyNewParam2OneResourceConceptsParams(t);
}


void indri::query::CoordinateAscent::printHistoryLce()
{
//    int fbDocs = _param.get("fbDocs", 50);
//    int fbTerms = _param.get("fbTerms", 15);
//    double lambdaT = _param.get("lambdaT", 0.2);
//    double lambdaPh = _param.get("lambdaPh", 0.2);
//    double lambdaPr = _param.get("lambdaPr", 0.8);
//    double lambdaMrf = _param.get("lambdaMrf", 0.1);
//    double fbMu = _param.get("fbMu", 0.1);
//    double fbOrigWeight = _param.get("fbOrigWeight", 0.1);
//    double mu = _param.get("mu", 2500);
//    double lambda = _param.get("lambda", 0.4);

    cout << "print history: size: " << optimizationHistory.size() << endl;
    for(auto oHist: optimizationHistory)
    {
        cout << std::get<0>(oHist) << " " << std::get<1>(oHist) << " " << std::get<2>(oHist) << endl;
    }
//    cout << "fbDocs = " << fbDocs << " fbTerms = " << fbTerms << " lambdaT = " << lambdaT
//         << " lambdaPh = " << lambdaPh << " lambdaPr = " << lambdaPr << " lambdaMrf = " << lambdaMrf
//         << " fbMu = " << fbMu << " fbOrigWeight = " << fbOrigWeight
//         << " mu = " << mu << " lambda = " << lambda
//         << endl;
}

void indri::query::CoordinateAscent::printHistoryMedPqe()
{
//    int fbDocs = _param.get("fbDocs", 0);
//    int fbTerms = _param.get("fbTerms", 0);
//    double fbOrigWeight = _param.get("fbOrigWeight", 0.0);
//    double mu = _ruleParam.get("mu", 0.0);
//    double T_D = _ruleParam.get("T_D", 0.0);
//    double O_D = _ruleParam.get("O_D", 0.0);
//    double U_D = _ruleParam.get("U_D", 0.0);
//    double gamma1 = _ruleParam.get("gamma1", 0.0);
//    double gamma2 = _ruleParam.get("gamma2", 0.0);
//    double gamma3 = _ruleParam.get("gamma3", 0.0);

    cout << "print history: size: " << optimizationHistory.size() << endl;
    for(auto oHist: optimizationHistory)
    {
        cout << std::get<0>(oHist) << " " << std::get<1>(oHist) << " " << std::get<2>(oHist) << " " << endl;
    }
//    cout << "fbDocs = " << fbDocs << " fbTerms = " << fbTerms << " fbOrigWeight = " << fbOrigWeight << " mu = " << mu
//         << " T_D = " << T_D << " O_D = " << O_D << " U_D = " << U_D << " gamma1 = "
//         << gamma1 << " gamma2 = "<< gamma2 << " gamma3 = "<< gamma3 << endl;
}
