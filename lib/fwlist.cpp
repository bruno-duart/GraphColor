#include "fwlist.h"

#include <iostream>

void ForwardListWrapper::push_front(int index_i, int undo_color, int T_iter)
{
    list.emplace_front(index_i, undo_color, T_iter);
    size_list++;
}

void ForwardListWrapper::push_back(int index_i, int undo_color, int T_iter)
{
    auto it = list.before_begin();
    for (auto &node : list)
        it++;
    list.insert_after(it, NodeData(index_i, undo_color, T_iter));
    size_list++;
}

bool ForwardListWrapper::pop_front()
{
    if (list.empty())
        return false;
    list.pop_front();
    size_list--;
    return true;
}

bool ForwardListWrapper::list_erase(int idx)
{
    auto& flist = get_list();
    auto prev = flist.before_begin();
    auto it = flist.begin();

    int position{0};

    while (it != flist.end())
    {
        if (position == idx){
            flist.erase_after(prev);
            decrease_size();
            return true;
        }
        prev = it;
        ++it;
        ++position;
    }
    return false;
}

int ForwardListWrapper::size() const
{
    return size_list;
}

void ForwardListWrapper::print() const
{
    for (const auto &node : list)
    {
        std::cout << "(" << node.index_i << ", " << node.undo_color << "; " << node.count_iter << ") -> ";
    }
    std::cout << "NULL\n";
}

std::forward_list<NodeData> &ForwardListWrapper::get_list()
{
    return list;
}

void ForwardListWrapper::decrease_size()
{
    if (size_list > 0)
        size_list--;
}