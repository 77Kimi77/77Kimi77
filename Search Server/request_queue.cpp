#include "request_queue.h"
using namespace std;


RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server),count_requests_(0), current_time_(0) { }

template <typename DocumentPredicate>
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
    const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(result.size());
    return result;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(result.size());
    return result;
}
    
vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    const auto result = search_server_.FindTopDocuments(raw_query);
    AddRequest(result.size());
    return result;
}
    
int RequestQueue::GetNoResultRequests() const {
    return minutes_in_day - count_requests_;
}

void RequestQueue::AddRequest(int num_request) {
    if (num_request != 0) {
        ++current_time_;
        ++count_requests_;
        requests_.push_back({ num_request,current_time_ });
    }
}