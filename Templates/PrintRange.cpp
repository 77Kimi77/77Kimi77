template <typename It>
void PrintRange(It range_begin, It range_end){
        for(auto iterator = range_begin; range_begin!= range_end; ++iterator, ++range_begin){
            cout<< *iterator<<" "s;
        }
        cout<<endl;
}
