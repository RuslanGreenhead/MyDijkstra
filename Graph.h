//
// Graph class implementation (+ Node class implementation)
//

#pragma once
#include <map>
#include <tuple>
#include "Matrix.h"

template<typename Key, typename Value, typename Weight=double>
class Graph{
public:
    // Nested class (implements a node of graph)
    class Node;

    //-------------------------------Feature types----------------------------------//
    using key_type = Key;
    using value_type = Value;
    using weight_type = Weight;
    using iterator = typename std::map<key_type, Node>::iterator;
    using const_iterator = typename std::map<key_type, Node>::const_iterator;

    //-------------------------------Constructors-----------------------------------//
    Graph() = default;
    Graph(const Graph& obj) = default;
    Graph(Graph&& obj) noexcept = default;
    Graph(std::initializer_list<std::pair<key_type, value_type>> init_list): m_nodes(init_list.begin(), init_list.end()){};

    template<typename Iterable> // Iterable obj must be random-access (vector, array, deque...)
    Graph(const Iterable& nodes, const Matrix<weight_type>& matr){
        if((matr.num_rows() != matr.num_cols()) || (matr.num_rows() != nodes.size())){
            throw runtime_error("Adjacency matrix constructor failed!");
        }
        m_nodes = {nodes.cbegin(), nodes.cend()};
        for(int i = 0; i < matr.num_rows(); ++i){
            for(int j = 0; j < matr.num_cols(); ++j){
                if(matr(i, j) != 0) insert_edge({nodes[i].first, nodes[j].first}, matr(i, j));
            }
        }
    };

    //--------------------------------Assignments-----------------------------------//
    Graph& operator = (const Graph& obj) {m_nodes = obj.m_nodes;}
    Graph& operator = (Graph&& obj) noexcept {m_nodes = obj.m_nodes;}

    //-----------------------------Support methods----------------------------------//
    [[nodiscard]] bool empty() const { return m_nodes.empty();}
    [[nodiscard]] size_t size() const { return m_nodes.size();}
    void clear() noexcept { m_nodes.clear();}
    void swap(Graph& obj) noexcept { std::swap(m_nodes, obj.m_nodes);}

    //-------------------------------Iterator methods-------------------------------//
    const_iterator cbegin() const { return m_nodes.cbegin();}
    const_iterator cend() const {return m_nodes.cend();}
    const_iterator begin() const {return cbegin();}
    const_iterator end() const {return cend();}
    iterator begin() {return m_nodes.begin();}
    iterator end() {return m_nodes.end();}

    //-------------------------------Node features----------------------------------//
    size_t degree_in(const key_type& key) const {
        if(m_nodes.find(key) == cend()) throw std::out_of_range("No such key!");
        size_t res = 0;
        for(auto it = cbegin(); it != cend(); ++it){
            if(it->second.m_edges.find(key) != it->second.end()) ++res;
        }
        return res;
    }
    size_t degree_out(const key_type& key) const { return m_nodes.at(key).size();}
    bool loop(const key_type& key) const {
        for(auto it = m_nodes.at(key).cbegin(); it != m_nodes.at(key).cend(); ++it){
            if (it->first == key) return true;
        }
        return false;
    }

    //-------------------------------Accessors--------------------------------------//
    value_type& operator [] (const key_type& key){ return m_nodes[key].m_value;}
    value_type& at(const key_type& key){ return m_nodes.at(key).m_value;}
    const value_type& at(const key_type& key) const { return m_nodes.at(key).m_value;}
    const Node& get_node(const key_type& key) const {return m_nodes.at(key);}

    //-------------------------------Insertion & erasing nodes-------------------------------//
    std::pair<iterator, bool> insert_node(const key_type& key, const value_type& value){
        return m_nodes.insert(std::make_pair(key, Node(value)));
    }
    std::pair<iterator, bool> insert_or_assign_node(const key_type& key, const value_type& value){
        return m_nodes.insert_or_assign(key, Node(value));
    }

    std::pair<typename Node::iterator, bool> insert_edge(const std::pair<key_type, key_type>& key_pair, const weight_type& weight){
        return m_nodes.at(key_pair.first).insert_edge(key_pair.second, weight);
    }
    std::pair<typename Node::iterator, bool> insert_or_assign_edge(const std::pair<key_type, key_type>& key_pair, const weight_type& weight){
        return m_nodes.at(key_pair.first).insert_or_assign_edge(key_pair.second, weight);
    }

    void clear_edges(){
        for(auto it = begin(); it != end(); ++it){
            it->second.m_edges.clear();
        }
    }
    bool erase_edges_go_from(const key_type& key){
        try{
            m_nodes.at(key).m_edges.clear();
        } catch (...){
            return false;
        }
        return true;
    }
    bool erase_edges_go_to(const key_type& key){
        try{
            for(auto it = begin(); it != end(); ++it){
                it->second.erase_edge(key);
            }
        } catch (...){
            return false;
        }
        return true;
    }
    bool erase_node(const key_type& key){
        for(auto it = begin(); it != end(); ++it){
            it->second.erase_edge(key);
        }
        return (m_nodes.erase(key) == 1);
    }

private:
    std::map<key_type, Node> m_nodes;
};


template<typename Key, typename Value, typename Weight>
class Graph<Key, Value, Weight>::Node{
public:
    friend class Graph;

    //-------------------------------Feature types----------------------------------//
    using key_type = Key;
    using value_type = Value;
    using weight_type = Weight;
    using iterator = typename std::map<key_type, weight_type>::iterator;
    using const_iterator = typename std::map<key_type, weight_type>::const_iterator;

    //-------------------------------Constructors-----------------------------------//
    Node() = default;
    Node(const Node& obj) = default;
    Node(Node&& obj) noexcept = default;
    Node(const value_type& value) : m_value(value) {};

    //--------------------------------Assignments-----------------------------------//
    Node& operator = (const Node& obj){
        m_value = obj.m_value;
        m_edges = obj.m_edges;
        return *this;
    }
    Node& operator = (Node&& obj) noexcept{
        m_value = std::move(obj.m_value);
        m_edges = std::move(obj.m_edges);
        return *this;
    }

    //-----------------------------Support methods----------------------------------//
    [[nodiscard]] bool empty() const { return m_edges.empty();}
    [[nodiscard]] size_t size() const { return m_edges.size();}
    value_type& value(){ return m_value;}
    const value_type& value() const { return m_value;}
    void clear(){ m_edges.clear();}

    //-------------------------------Iterator methods-------------------------------//
    const_iterator cbegin() const { return m_edges.cbegin();}
    const_iterator cend() const {return m_edges.cend();}
    const_iterator begin() const {return cbegin();}
    const_iterator end() const {return cend();}
    iterator begin() {return m_edges.begin();}
    iterator end() {return m_edges.end();}

    //-------------------------------Insertion & erasing edges-------------------------------//
    std::pair<iterator, bool> insert_edge(const key_type& key, const weight_type& weight){
        return m_edges.insert(std::make_pair(key, weight));
    }
    std::pair<iterator, bool> insert_or_assign_edge(const key_type& key, const weight_type& weight){
        return m_edges.insert_or_assign(key, weight);
    }
    bool erase_edge(const key_type& key){
        return (m_edges.erase(key) == 1);
    }

private:
    value_type m_value;
    std::map<key_type, weight_type> m_edges;
};

template<typename key, typename value, typename weight>
void swap(Graph<key, value, weight> obj_1, Graph<key, value, weight> obj_2) noexcept {
    obj_1.swap(obj_2);
}