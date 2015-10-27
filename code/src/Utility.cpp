#include "../include/Utility.hpp"

//map<string, lemur::api::DOCID_T> wsuIr::expander::Utility::documentNamesMap = map<string, lemur::api::DOCID_T>();
map<string, double> wsuIr::expander::Utility::trec_res_precision = map<string, double>();
map<int, string> wsuIr::expander::Utility::qidOutputStr = map<int, string>();
map<vector<pair<string, std::string> >, map<int, string> > wsuIr::expander::Utility::QueryThread_storeResults
    = map<vector<pair<string, std::string> >, map<int, string> >();
map<pair<vector<pair<string, std::string> >, vector<lemur::api::DOCID_T> >, vector<indri::api::ScoredExtentResult> >
wsuIr::expander::Utility::runQuery_results
    = map<pair<vector<pair<string, std::string> >, vector<lemur::api::DOCID_T> >, vector<indri::api::ScoredExtentResult> >();
bool wsuIr::expander::Utility::trecWaitEnable = false;


wsuIr::expander::Utility::Utility()
{
    //ctor
}

wsuIr::expander::Utility::~Utility()
{
    //dtor
}


// ---> cross validation
pair< vector<string>, vector<string> > wsuIr::expander::Utility::trainTestVecs(vector<string> allQuNums, size_t foldCount, size_t foldNum, bool trainTestRevrs)
{
    vector<string> trainQuNums;
    vector<string> testQuNums;

    if(foldNum >= foldCount)
        throw runtime_error("wsuIr::expander::Utility::trainTestVecs: not approperiate values for foldNum and foldCount: error in: foldNum >= foldCount : " + to_string(foldNum) + " >= " + to_string(foldCount));

    size_t qCount = allQuNums.size();

    size_t qIndLeft  = qCount/foldCount*foldNum;
    size_t qIndRight = qCount/foldCount*(foldNum+1);

    cout << "wsuIr::expander::Utility::trainTestVecs: qCount = " << qCount << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: foldCount = " << foldCount << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: foldNum = " << foldNum << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: qIndLeft = " << qIndLeft << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: qIndRight = " << qIndRight << endl;

    for (size_t i = 0; i < qCount; i++)
    {
        if( (i >= qIndLeft) && (i < qIndRight) )
        {
            testQuNums.push_back(allQuNums[i]);
        }
        else
        {
            trainQuNums.push_back(allQuNums[i]);
        }
    }
    cout << "wsuIr::expander::Utility::trainTestVecs: allQuNums: " << endl << "\t";
    for (auto q: allQuNums)
        cout << q << " ";
    cout << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: trainQuNums: " << endl << "\t";
    for (auto q: trainQuNums)
        cout << q << " ";
    cout << endl;
    cout << "wsuIr::expander::Utility::trainTestVecs: testQuNums: " << endl << "\t";
    for (auto q: testQuNums)
        cout << q << " ";
    cout << endl;

    if(trainTestRevrs)
       return make_pair(testQuNums, trainQuNums);
	else
       return make_pair(trainQuNums, testQuNums);
}

pair<vector<string>, vector<string> > wsuIr::expander::Utility::selectCrossValdQuNums( indri::api::Parameters& param, size_t foldCount, size_t foldNum )
{

    int queryOffset = param.get( "queryOffset", 0 );

    indri::api::Parameters parameterQueries = param[ "query" ];

    size_t queryCount = parameterQueries.size();

    cout << "wsuIr::expander::Utility::selectCrossValdQuNums: queryCount = " << queryCount << endl;

    vector<string> allQuNums;
    for( size_t i=0; i<queryCount; i++ )
    {
        allQuNums.push_back(wsuIr::expander::Utility::queryIndex2queryNumber(parameterQueries, queryOffset, i));
    }
    shuffle(allQuNums);

    return trainTestVecs(allQuNums, foldCount, foldNum, param.get("trainTestRevrs",false));
}

// http://stackoverflow.com/questions/20734774/random-array-generation-with-no-duplicates
void wsuIr::expander::Utility::shuffle(vector<string> & v)
{
    size_t n = v.size();
    if (n > 1)
    {
        srand(0);
        for (size_t i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            swap(v[i],v[j]);
        }
    }
    /*
    for (auto i: v)
    {
        cout << i << " ";
    }
    cout << endl;
    */
}
// <--- cross validation

void wsuIr::expander::Utility::QueryThread_results_store(vector<pair<string, std::string> > queriesText)
{
    QueryThread_storeResults.insert(make_pair(queriesText, qidOutputStr));
    cout << "QueryThread_results_store: size of QueryThread stored results: " << QueryThread_storeResults.size() << endl;
}

void wsuIr::expander::Utility::QueryThread_results_update(vector<pair<string, std::string> > queriesText)
{
    cout << "QueryThread_results_update:" << QueryThread_storeResults.size() << endl;
//    initialize();
    qidOutputStr = QueryThread_storeResults.at(queriesText);
}

bool wsuIr::expander::Utility::QueryThread_results_isExist(vector<pair<string, std::string> > queriesText)
{
    return (QueryThread_storeResults.find(queriesText) != QueryThread_storeResults.end());
}


bool wsuIr::expander::Utility::runQuery_results_isExist(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds)
{
    pair<vector<pair<string, std::string> >, vector<lemur::api::DOCID_T> > tmp = make_pair(queriesText, topDocIds);
    return (runQuery_results.find(tmp) != runQuery_results.end());
}

std::vector< indri::api::ScoredExtentResult > wsuIr::expander::Utility::runQuery_results_get(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds)
{
    pair<vector<pair<string, std::string> >, vector<lemur::api::DOCID_T> > tmp = make_pair(queriesText, topDocIds);
    vector<indri::api::ScoredExtentResult> runQuery_result = runQuery_results.at(tmp);
    return runQuery_result;
}

void wsuIr::expander::Utility::runQuery_results_store(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds, std::vector< indri::api::ScoredExtentResult > runQuery_result)
{
    runQuery_results.insert(make_pair(make_pair(queriesText, topDocIds), runQuery_result));
}


map<string, lemur::api::DOCID_T> wsuIr::expander::Utility::findDocumentNames(indri::api::Parameters& param, string index_s)
{
    cout << "mapping document IDs to document names..." << endl;
//    documentNamesMap.clear();
    map<string, lemur::api::DOCID_T> documentNamesMap;

    string repName = param.get(index_s, "");
//    string repName = param.get("indexFielded", "");
    indri::collection::Repository r;
    r.openRead( repName );
    indri::server::LocalQueryServer local(r);
    indri::collection::Repository::index_state state = r.indexes();
    indri::index::Index* indexFielded = (*state)[0];
    indri::collection::CompressedCollection* collection = r.collection();

    for (lemur::api::DOCID_T docId =  0; docId < static_cast<lemur::api::DOCID_T>(indexFielded->documentCount()); docId++)
    {
        std::string documentName = collection->retrieveMetadatum( docId, "docno" );
        documentNamesMap.insert(make_pair(documentName, docId));
    }
    cout << "size of documentNamesMap = " << documentNamesMap.size() << endl;
    r.close();
    return documentNamesMap;
}

void wsuIr::expander::Utility::parameters_parse( indri::api::Parameters& converted, const std::string& spec )
{
    int nextComma = 0;
    int nextColon = 0;
    signed int  location = 0;
    for( location = 0; location < static_cast<signed int>(spec.length()); )
    {
        nextComma = spec.find( ',', location );
        nextColon = spec.find( ':', location );

        std::string key = spec.substr( location, nextColon-location );
        std::string value = spec.substr( nextColon+1, nextComma-nextColon-1 );
        converted.set( key, value );

        if( nextComma > 0 )
            location = nextComma+1;
        else
            location = spec.size();
    }
}



string wsuIr::expander::Utility::getTextOfADocument(string documentName, indri::api::Parameters& param, indri::api::QueryEnvironment &  )
{

//    lemur::api::DOCID_T docId = documentNamesMap[documentName];

    //lemur::api::DOCID_T docId = (env.documentIDsFromMetadata("docno", vector<string>{documentName}))[0];
    //cout << "getTextOfADocument: top-ranked document: (docId, documentName) = (" << docId << ", " << documentName << ")\n";

    string repName = param.get("indexFielded", "");
//    cout << "getTextOfADocument: repName = " << repName << endl;
    indri::collection::Repository r;
    r.openRead( repName );
    //indri::server::LocalQueryServer local(r);
    indri::api::QueryEnvironment envFielded;
    envFielded.addIndex( repName );
//    indri::collection::Repository::index_state state = r.indexes();
//    indri::index::Index* indexFielded = (*state)[0];
    lemur::api::DOCID_T docId = (envFielded.documentIDsFromMetadata("docno", vector<string>{documentName}))[0];
    cout << "wsuIr::expander::Utility::getTextOfADocument: top-ranked document: (docId, documentName) = (" << docId << ", " << documentName << ")\n";

	//vector<string> fieldContent_ = envFielded.documentMetadata (vector<lemur::api::DOCID_T>{docId},"ABSTRACT");
/*
	for(auto s: fieldContent_)
	{
		cout << "wsuIr::expander::Utility::getTextOfADocument: field: " << s << endl;
	}
*/
//    std::string documentName = collection->retrieveMetadatum( atoi( number ), "docno" );

    string fieldString = "#TEXT#";

//    int fieldId = indexFielded->field(fieldString);

    int documentID = docId;
    indri::collection::CompressedCollection* collection = r.collection();
    indri::api::ParsedDocument* document = collection->retrieve( documentID );

//    cout << "getTextOfADocument: size of document metadata: " << document->metadata.size() << endl;

    auto documentMetadata  = document->metadata[3];

    if (documentMetadata.key != fieldString)
        throw runtime_error("the second key of meta-data is different from: " + fieldString + " != " + documentMetadata.key);

    string fieldContent = (const char*) documentMetadata.value;

    string tagContent = wsuIr::expander::Utility::GetTag(fieldContent, "<ABSTRACT>", "</ABSTRACT>");
//    cout << "getTextOfADocument: tagContent = " << tagContent << endl;
    const int maxAbstractLength = 5000;
    string text = "";
    for ( int i = 0; i < min(maxAbstractLength, int(tagContent.size())); i++ )
        text += tagContent[i];

    r.close();
    return text;
}

// from: http://stackoverflow.com/questions/17257254/getting-a-substring-between-two-tags-c-c
std::string wsuIr::expander::Utility::GetTag(const std::string &str, const std::string &tagBegin, const std::string &tagEnd )
{
    std::string::size_type start = str.find(tagBegin);
    if (start != str.npos)
    {
        std::string::size_type end = str.find(tagEnd, start + 1);
        if (end != str.npos)
        {
            start += tagBegin.size();
            std::string::size_type count = end - start;
            return str.substr(start, count);
        }
    }
    return "";
}

string wsuIr::expander::Utility::queryIndex2queryNumber(indri::api::Parameters& queries, int queryOffset, size_t index_)
{
    string queryNumber = "";
    if( queries[index_].exists( "number" ) )
    {
        queryNumber = (std::string) queries[index_]["number"];
    }
    else
    {
        int thisQuery=queryOffset + int(index_);
        std::stringstream s;
        s << thisQuery;
        queryNumber = s.str();
    }
    return queryNumber;
}

void wsuIr::expander::Utility::push_queue( std::queue< indri::query::query_t* >& q, indri::api::Parameters& queries,
        int queryOffset )
{
	size_t size1 = queries.size();
    for( size_t i=0; i<size1; i++ )
    {
        std::string queryNumber = wsuIr::expander::Utility::queryIndex2queryNumber(queries, queryOffset, i);
        std::string queryText;
        std::string queryType = "indri";
        if( queries[i].exists( "type" ) )
            queryType = (std::string) queries[i]["type"];
        if (queries[i].exists("text"))
            queryText = (std::string) queries[i]["text"];
        if (queryText.size() == 0)
            queryText = (std::string) queries[i];

        // working set and RELFB docs go here.
        // working set to restrict retrieval
        std::vector<std::string> workingSet;
        // Rel fb docs
        std::vector<std::string> relFBDocs;
        wsuIr::expander::Utility::copy_parameters_to_string_vector( workingSet, queries[i], "workingSetDocno" );
        wsuIr::expander::Utility::copy_parameters_to_string_vector( relFBDocs, queries[i], "feedbackDocno" );

        q.push( new indri::query::query_t( i, queryNumber, queryText, queryType, workingSet, relFBDocs ) );

    }
}

void wsuIr::expander::Utility::push_queue_crv( std::queue< indri::query::query_t* >& q, indri::api::Parameters& queries,
        int queryOffset, indri::api::Parameters& param )
{
    if(!param.exists("foldCount"))
        throw runtime_error("wsuIr::expander::Utility::push_queue: foldCount does not exists in the parameters object.");
    if(!param.exists("foldNum"))
        throw runtime_error("wsuIr::expander::Utility::push_queue: foldNum does not exists in the parameters object.");
    if(!param.exists("train"))
        throw runtime_error("wsuIr::expander::Utility::push_queue: train does not exists in the parameters object.");
    size_t foldCount = param.get("foldCount", 0);
    size_t foldNum = param.get("foldNum", 0);
    bool train = param.get("train", false);
    cout << "wsuIr::expander::Utility::push_queue: queries.size() = " << queries.size() << endl;

    pair<vector<string>, vector<string> > crossValQuNums = wsuIr::expander::Utility::selectCrossValdQuNums( param, foldCount, foldNum );

	size_t size1 = queries.size();
    for( size_t i=0; i<size1; i++ )
    {
        std::string queryNumber = wsuIr::expander::Utility::queryIndex2queryNumber(queries, queryOffset, i);

        if(train) // if cross-validation is in its training step
        {
            // if this query number does not exists in the list of query numbers specified for training:
            if(find (crossValQuNums.first.begin(), crossValQuNums.first.end(), queryNumber) == crossValQuNums.first.end())
			{
            	queries[i].clear();
            	//queries.erase (queries.begin()+i);
            	//i--;
            	//size--;
            		continue;
        	}
		}
        else // if cross-validation is in its testing step
        {
            if(find (crossValQuNums.second.begin(), crossValQuNums.second.end(), queryNumber) == crossValQuNums.second.end())
		    {
		    	queries[i].clear();
            	//queries.erase (queries.begin()+i);
            	//i--;
                continue;
            }
        }

        std::string queryText;
        std::string queryType = "indri";
        if( queries[i].exists( "type" ) )
            queryType = (std::string) queries[i]["type"];
        if (queries[i].exists("text"))
            queryText = (std::string) queries[i]["text"];
        if (queryText.size() == 0)
            queryText = (std::string) queries[i];

        // working set and RELFB docs go here.
        // working set to restrict retrieval
        std::vector<std::string> workingSet;
        // Rel fb docs
        std::vector<std::string> relFBDocs;
        wsuIr::expander::Utility::copy_parameters_to_string_vector( workingSet, queries[i], "workingSetDocno" );
        wsuIr::expander::Utility::copy_parameters_to_string_vector( relFBDocs, queries[i], "feedbackDocno" );

        q.push( new indri::query::query_t( i, queryNumber, queryText, queryType, workingSet, relFBDocs ) );

    }
}

string wsuIr::expander::Utility::read_trec_rel(indri::api::Parameters& param_)
{
    string trec_rel_ = param_.get( "trec_rel", "" );
    ifstream is (trec_rel_.c_str());
    if (!is)
        throw std::runtime_error("Could not open trec_rel file: " + trec_rel_);
    string line;
    stringstream lines;
    while(getline(is,line))
    {
        lines << line << '\n';
    }
    return lines.str();
}

bool wsuIr::expander::Utility::copy_parameters_to_string_vector( std::vector<std::string>& vec, indri::api::Parameters p, const std::string& parameterName )
{
    if( !p.exists(parameterName) )
        return false;

    indri::api::Parameters slice = p[parameterName];

    for( size_t i=0; i<slice.size(); i++ )
    {
        vec.push_back( slice[i] );
    }

    return true;
}

double wsuIr::expander::Utility::trec_eval_avg()
{
    double sum = 0;
    int size_ = 0;
    cout << "trec_res_precision.size() = " << trec_res_precision.size() << endl;
    for (auto m: trec_res_precision)
    {
        string qNumber = m.first;
        for (int i = 0; qNumber[i]; i++)
            qNumber[i] = tolower(qNumber[i]);

        if (qNumber != "all")
        {
            sum += m.second;
            size_++;
        }
    }
    return sum/size_;
}

void wsuIr::expander::Utility::update_trec_eval(string queryNumber, double trec_res)
{
    cout << "update_trec_eval: " << queryNumber << " " << trec_res << endl;
    if (trec_res >=0 ) // thraw away those flagged with the number -100 (queries with no relvenace judgemnts like query 64 in AP)
    {
        trec_res_precision.insert(pair<string, double>(queryNumber,trec_res));
    }
    print_trec_eval();
}

void wsuIr::expander::Utility::print_trec_eval()
{
    cout << "print_trec_eval: size: " << trec_res_precision.size() << endl;
    for (auto i: trec_res_precision)
    {
        cout << "trec_res_precision: " << i.first << " " << i.second << endl;
    }
}
map<string, double> wsuIr::expander::Utility::get_trec_eval()
{
    return trec_res_precision;
}
void wsuIr::expander::Utility::waitForTrecFree()
{
    while(trecWaitEnable) {}
    cout << "stop waiting -s2" << endl;
}
void wsuIr::expander::Utility::startWaiting()
{
    cout << "start waiting " << endl;
    trecWaitEnable = true;
}
void wsuIr::expander::Utility::stopWaiting()
{
    cout << "stop waiting -s1" << endl;
    trecWaitEnable = false;
}
void wsuIr::expander::Utility::concatenateOutputStr(string qNumber, string outputStr_)
{
    // convert string to int to be in the correct sequence. However, the presults precision should be the same
    cout << "wsuIr::expander::Utility::concatenateOutputStr: size of qidOutputStr = " << qidOutputStr.size() << endl;
    if(qidOutputStr.find(atoi(qNumber.c_str())) != qidOutputStr.end()) // if this qnumber exists in the record, it is time to clear it
        wsuIr::expander::Utility::initialize();
    qidOutputStr.insert(pair<int, string>(atoi(qNumber.c_str()), outputStr_));
}

string wsuIr::expander::Utility::getRunQuery_(string qNumber_, map<int, string> & qidOutputStr_)
{
//    cout << "getQidOutputStr: qNumber_ = " <<  qNumber_ << endl;
    return qidOutputStr_.at(atoi(qNumber_.c_str()));
}


vector< pair<string, double> > wsuIr::expander::Utility::parseRunQueryStr(string runQuery_, int fbDocsL2)
{
//    cout << "getting QidOutputDocuments..." << endl;
    vector< pair<string, double> > qidOutputDocuments;
    stringstream runQuery_SS(runQuery_);
    string tmp, docId;
    double score;
    int fbDocsL2_ = 0;
    while(runQuery_SS >> tmp >> tmp >> docId >> tmp >> score >> tmp)
    {
        cout << "wsuIr::expander::Utility::parseRunQueryStr: docId = " << docId << " score = " << score << endl;
        qidOutputDocuments.push_back(make_pair(docId, score));
        if(++fbDocsL2_ == fbDocsL2)
            break;
    }
    //cout << "size of qidOutputDocuments: " << qidOutputDocuments.size() << endl;
    return qidOutputDocuments;

}


vector<pair<string, vector<string> > > wsuIr::expander::Utility::getTopDocsNames(signed int fbDocs)
{
    cout << "getting Qid-Output pairs for All the Documents..." << endl;
//    int fbDocs_umlsPrf = param.get("fbDocs_umlsPrf",10);
//    cout << "fbDocs_umlsPrf = " << fbDocs_umlsPrf << endl;
    cout << "wsuIr::expander::Utility::getQidOutputAllDocuments: size of qidOutputStr = " << qidOutputStr.size() << endl;
    vector<pair<string, vector<string> > > qidOutputAllDocuments;
    for(auto qid_OutputStr: qidOutputStr)
    {
        string qId = to_string(qid_OutputStr.first);
        string runQueryStr = qid_OutputStr.second;

        vector< pair<string, double> > qidOutputDocuments = wsuIr::expander::Utility::parseRunQueryStr(runQueryStr, fbDocs);
        vector<string> qidOutputDocuments_;
        for (auto d:qidOutputDocuments) qidOutputDocuments_.push_back(d.first);
        qidOutputAllDocuments.push_back(make_pair(qId, qidOutputDocuments_));
    }
    cout << "size of qidOutputAllDocuments = " << qidOutputAllDocuments.size() << endl;
    return qidOutputAllDocuments;
}

std::string wsuIr::expander::Utility::exec(const char* cmd)
{
    cout << "cmd: " << cmd << endl;
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void wsuIr::expander::Utility::calcTrecPrecisionInfNdcg(indri::api::Parameters& param)
{
    cout << "calcualting infNDCG precision..." << endl;
    string _outputStr_ = "";
    for(auto ostr: qidOutputStr)
    {
        _outputStr_ += ostr.second;
    }
    string sample_eval = param.get("sample_eval", "");
    string trec_rel = param.get("trec_rel", "");
    srand ( time(NULL) );
    string fileName = "/tmp/sb" + to_string(rand());
    ofstream File(fileName.c_str());
    File << _outputStr_;
    File.close();
    string cmd =  sample_eval + " -q " + trec_rel + " " + fileName + " |grep infNDCG | awk '{print $2, $3}'";
    try
    {
        if (sample_eval=="" || trec_rel=="")
        {
            throw std::runtime_error("must specify parameters for sample_eval and trec_rel ");
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error: " << e.what () << '\n';
    }
    string res = exec(cmd.c_str());
    stringstream res_ss(res);
    while(!res_ss.eof())
    {
        string qNumber;
        double infNDCG;
        res_ss >> qNumber >> infNDCG;
        cout << qNumber << "\t" << infNDCG << endl;
        trec_res_precision.insert(pair<string, double>(qNumber, infNDCG));
    }
    cmd = "rm " + fileName;
    cout << exec(cmd.c_str()) << endl;
}

void wsuIr::expander::Utility::calcTrecPrecisionMap(vector<string> _trec_eval_argv_v)
{
    char *_trec_eval_argv_[5];
    int ii = 0;
    for (auto it_ = _trec_eval_argv_v.begin(); it_ != _trec_eval_argv_v.end(); it_++)
    {
        _trec_eval_argv_[ii++] = &(*it_)[0];
    }
    for(auto ostr: qidOutputStr)
    {
        string qNumber = to_string(ostr.first);
        string _outputStr_ = ostr.second;
        char* text_results = &_outputStr_[0];
        _trec_eval_argv_[4] = text_results;
        cout << "qNumber = " << qNumber << endl;
        cout << "eval measure = " << _trec_eval_argv_[2] << endl;
        double trec_eval_ = 0;

        if (trec_eval_ >=0 ) // thraw away those flagged with the number -100 (queries with no relvenace judgemnts like query 64 in AP)
        {
            cout << "qNumber = " << qNumber << " trec_eval_ = " << trec_eval_ << endl;
            trec_res_precision.insert(pair<string, double>(qNumber, trec_eval_));
        }
    }
}
void wsuIr::expander::Utility::initialize()
{
//    cout << "wsuIr::expander::Utility::initialize" << endl;
    trec_res_precision.clear();
    qidOutputStr.clear();
}

vector<string> wsuIr::expander::Utility::readGoodSTYs(indri::api::Parameters& param)
{
    std::vector<std::string> goodSTYs_v;

    // get the good symantic types:
    string goodSTYs = param.get("goodSTYs", "all");
    goodSTYs = boost::algorithm::trim_copy_if(goodSTYs, boost::algorithm::is_any_of(" "));

    std::transform(goodSTYs.begin(), goodSTYs.end(), goodSTYs.begin(), ::tolower);
    goodSTYs.erase(std::remove(goodSTYs.begin(), goodSTYs.end(), ','), goodSTYs.end());

    boost::split(goodSTYs_v, goodSTYs, boost::is_any_of("\n"), boost::token_compress_on);
//    std::cout << "goodSTYs_v.size() = " << goodSTYs_v.size() << std::endl;

    return goodSTYs_v;
}
