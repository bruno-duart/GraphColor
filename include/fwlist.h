#ifndef HEADERS_FWLIST
#define HEADERS_FWLIST

#include <forward_list>

class NodeData
{
public:
    int count_iter;
    int index_i;
    int undo_color;

    NodeData(int index_i, int undo_color, int T_iter)
        : count_iter(T_iter), index_i(index_i), undo_color(undo_color) {}
};

class ForwardListWrapper
{
private:
    std::forward_list<NodeData> list;
    int size_list{0};

public:
    void push_front(int index_i, int undo_color, int T_iter);
    void push_back(int index_i, int undo_color, int T_iter);
    bool pop_front();
    bool list_erase(int index);
    int size() const;
    void print() const;
    std::forward_list<NodeData>& get_list() ;
    void decrease_size();
};
#endif