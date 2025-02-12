#pragma once
#include <deque>
#include "search_server.h"

using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);
    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
    vector<Document> AddFindRequest(const string& raw_query);
    int GetNoResultRequests() const;
private:
    struct QueryResult {
        int request;
        uint64_t time_line;
    };
    deque<QueryResult> requests_;
    const SearchServer& search_server_;
    int count_requests_ = 0;
    uint64_t current_time_ = 0;
    const static int minutes_in_day = 1440;
    
    void AddRequest(int num_request);
};