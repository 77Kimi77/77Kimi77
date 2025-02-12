template <typename It, typename Element>
void FindAndPrint(It container, Element part){
    auto piece = find(container.begin(), container.end(), part);
    PrintRange(container.begin(),piece);
    PrintRange(piece,container.end());
}
