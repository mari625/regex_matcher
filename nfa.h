#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <cstdint>


struct NFANode {
    std::unordered_map<char, std::vector<int>> letter_edges;
    std::vector<int> epsilon_edges;
    bool final = false;

    NFANode() = default;
};


class NFA {
private:
    std::vector<NFANode> states;
    int first_state = 0;
    int final_state = 0;


    // moving states function

    void move_states_from_other(std::vector<NFANode>& other_states, size_t first_size) {
        for (size_t i = 0; i < other_states.size(); ++i) {
            NFANode& current_state = other_states[i];

            for (auto& item: current_state.letter_edges) {
                for (auto& el: item.second) {
                    el += first_size;
                }
            }

            for (auto& el: current_state.epsilon_edges) {
                el += first_size;
            }

            states.push_back(std::move(current_state));
        }
    }


    // transition function
    
    std::unordered_set<int> epsilon_closure(const std::unordered_set<int>& start_states) const {
        std::unordered_set<int> result;
        std::vector<bool> visited(states.size(), false);
        std::queue<int> epsilon_neighbours;

        for (int el: start_states) {
            epsilon_neighbours.push(el);
            visited[el] = true;
            result.insert(el);
        }

        while (!epsilon_neighbours.empty()) {
            const NFANode& current_state = states[epsilon_neighbours.front()];
            epsilon_neighbours.pop();

            for (int neighbour: current_state.epsilon_edges) {
                if (!visited[neighbour]){
                    visited[neighbour] = true;
                    epsilon_neighbours.push(neighbour);
                    result.insert(neighbour);
                }
            }
        }

        return result;
    }


public:

    // constructors

    NFA() = default;

    NFA (char symb) {
        states.emplace_back();
        states.emplace_back();

        add_letter_edge(0, 1, symb);

        first_state = 0;
        final_state = 1;
        states[1].final = true;
    }


    // operations with NFA nodes

    std::vector<NFANode>& get_states() {
        return states;
    }

    int get_final_state() const {
        return final_state;
    }

    int get_first_state() const {
        return first_state;
    }

    bool add_letter_edge (int begin, int end, char symb) {
        if (begin > static_cast<int>(states.size()) || end > static_cast<int>(states.size()) || begin < 0 || end < 0) {
            return false;
        }

        states[begin].letter_edges[symb].push_back(end);

        return true;
    }

    bool add_epsilon_edge (int begin, int end) {
        if (begin > static_cast<int>(states.size()) || end > static_cast<int>(states.size()) || begin < 0 || end < 0) {
            return false;
        }

        states[begin].epsilon_edges.push_back(end);

        return true;
    }


    // operations with NFA

    void build_and(NFA&& other) {
        size_t first_size = states.size();

        std::vector<NFANode>& other_states = other.get_states();
        int other_first = other.get_first_state();
        int other_final = other.get_final_state();

        states.reserve(first_size + other_states.size());

        move_states_from_other(other_states, first_size);

        int end = other_first + first_size;
        int begin = final_state;

        add_epsilon_edge(begin, end);
        states[begin].final = false;
        final_state = first_size + other_final;
    }

    void build_or(NFA&& other) {
        size_t first_size = states.size();
        
        std::vector<NFANode>& other_states = other.get_states();
        states.reserve(first_size + other_states.size() + 2);

        int other_first = other.get_first_state();
        int other_final = other.get_final_state();

        int this_begin = get_first_state();
        int this_end = get_final_state();
        states[this_end].final = false;

        move_states_from_other(other_states, first_size);

        int other_begin = other_first + first_size;
        int other_end = other_final + first_size;
        states[other_end].final = false;

        states.emplace_back();
        states.emplace_back();

        //NFANode& begin = states[states.size() - 2];
        NFANode& end = states[states.size() - 1];

        end.final = true;

        first_state = states.size() - 2;
        final_state = states.size() - 1;

        add_epsilon_edge(first_state, this_begin);
        add_epsilon_edge(first_state, other_begin);

        add_epsilon_edge(this_end, final_state);
        add_epsilon_edge(other_end, final_state);

    }

    void build_star() {
        states.emplace_back();
        states.emplace_back();

        //NFANode& begin = states[states.size() - 2];
        NFANode& end = states[states.size() - 1];

        end.final = true;

        int this_begin = get_first_state();
        int this_end = get_final_state();
        states[this_end].final = false;

        first_state = states.size() - 2;
        final_state = states.size() - 1;

        add_epsilon_edge(first_state, this_begin);
        add_epsilon_edge(this_end, final_state);
        add_epsilon_edge(this_end, this_begin);
        add_epsilon_edge(first_state, final_state);
    }


    // next function

    std::unordered_set<int> next(const std::unordered_set<int>& start_states, char symb) const {
        std::unordered_set<int> result;

        std::unordered_set<int> closure = epsilon_closure(start_states);

        for (int state_ind: closure) {
            const NFANode& state = states[state_ind];
            auto it = state.letter_edges.find(symb);

            if (it != state.letter_edges.end()) {
                
                for (int neighbour: it->second) {
                    result.insert(neighbour);
                }
            }
        }

        result = epsilon_closure(result);

        return result;
    }


    // check string fits nfa

    int64_t count_matches_string(const std::string& text) const {
        int64_t result = 0;
        
        size_t pos = 0;

        if (text.empty()) {
            std::unordered_set<int> current = epsilon_closure({first_state});
            if (current.find(final_state) != current.end()) {
                return 1;
            }
        }

        while (pos < text.size()) {
            std::unordered_set<int> current = epsilon_closure({first_state});

            if (pos == 0 && current.find(final_state) != current.end()) {
                ++result;
            }

            while (pos < text.size()) {
                current = next(current, text[pos]);

                ++pos;

                if (current.empty()) {
                    break;
                }

                if (current.find(final_state) != current.end()) {
                    ++result;
                    break;
                }
            }
        }

        return result;
    }


    /*std::vector<size_t> count_matches(const std::string& filename) const {
        std::ifstream file(filename, std::ios::binary);

        std::string line;

        std::vector<size_t> result;
        
        while (getline(file, line)) {
            result.push_back(count_matches_string(line));
        }
    
        return result;
    }*/
};
