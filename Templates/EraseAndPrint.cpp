template <typename Container>
void EraseAndPrint(Container& container, int position, int semi_interval, int end_interval){
    container.erase(container.begin() + position);
    PrintRange(container.begin(), container.end());
    container.erase(container.begin()+semi_interval, container.begin()+end_interval);
    PrintRange(container.begin(), container.end());
}
