#ifndef __PROGTEST__

#include "sample.h"

namespace regexp {

    Alternation::Alternation(RegExp left, RegExp right)
            : m_left(std::move(left)), m_right(std::move(right)) {
    }

    Concatenation::Concatenation(RegExp left, RegExp right)
            : m_left(std::move(left)), m_right(std::move(right)) {
    }

    Iteration::Iteration(RegExp node)
            : m_node(std::move(node)) {
    }

    Symbol::Symbol(alphabet::Symbol symb)
            : m_symbol(std::move(symb)) {
    }


#if 1

/** @brief Example usage of std::variant visitor using overloaded struct and lambda functions */
    void to_string(const regexp::RegExp &r, std::ostream &os) {
        std::visit(overloaded{
                           [&os](const std::shared_ptr<regexp::Alternation> &arg) {
                               os << '(';
                               to_string(arg->m_left, os);
                               os << '+';
                               to_string(arg->m_right, os);
                               os << ')';
                           },
                           [&os](const std::shared_ptr<regexp::Concatenation> &arg) {
                               os << '(';
                               to_string(arg->m_left, os);
                               os << ' ';
                               to_string(arg->m_right, os);
                               os << ')';
                           },
                           [&os](const std::shared_ptr<regexp::Iteration> &arg) {
                               os << '(';
                               to_string(arg->m_node, os);
                               os << ")*";
                           },
                           [&os](const std::shared_ptr<regexp::Symbol> &arg) { os << arg->m_symbol; },
                           [&os](const std::shared_ptr<regexp::Epsilon> &arg) { os << "#E"; },
                           [&os](const std::shared_ptr<regexp::Empty> &arg) { os << "#0"; },
                   },
                   r);
    }

#else
    /** @brief Example usage of std::visitor without overloaded struct with lambdas, using std::holds_alternative and std::get instead */
void to_string(const regexp::RegExp& r, std::ostream& os)
{
    if (std::holds_alternative<std::shared_ptr<regexp::Alternation>>(r)) {
        const auto& node = std::get<std::shared_ptr<regexp::Alternation>>(r);
        os << '(';
        to_string(node->m_left, os);
        os << ' ';
        to_string(node->m_right, os);
        os << ')';
    } else if (std::holds_alternative<std::shared_ptr<regexp::Concatenation>>(r)) {
        const auto& node = std::get<std::shared_ptr<regexp::Concatenation>>(r);
        os << '(';
        to_string(node->m_left, os);
        os << ' ';
        to_string(node->m_right, os);
        os << ')';
    } else if (std::holds_alternative<std::shared_ptr<regexp::Iteration>>(r)) {
        const auto& node = std::get<std::shared_ptr<regexp::Iteration>>(r);
        os << '(';
        to_string(node->m_node, os);
        os << ")*";
    } else if (std::holds_alternative<std::shared_ptr<regexp::Symbol>>(r)) {
        os << std::get<std::shared_ptr<regexp::Symbol>>(r)->m_symbol;
    } else if (std::holds_alternative<std::shared_ptr<regexp::Epsilon>>(r)) {
        os << "#E";
    } else if (std::holds_alternative<std::shared_ptr<regexp::Empty>>(r)) {
        os << "#0";
    } else {
        __builtin_unreachable();
    }
}
#endif

/** @brief Convenient function for converting RegExps into the ALT string format (usable in the ALT tool) */
    std::ostream &operator<<(std::ostream &os, const regexp::RegExp &regexp) {
        to_string(regexp, os);
        return os;
    }
}

namespace automaton {
/** @brief Convenient function for converting NFAs into the ALT string format (usable in the ALT tool) */
    std::ostream &operator<<(std::ostream &os, const automaton::NFA &nfa) {
        os << "NFA ";
        for (const auto &symb : nfa.m_Alphabet)
            os << symb << " ";
        os << '\n';

        for (const auto &state : nfa.m_States) {
            os << (nfa.m_InitialState == state ? ">" : " ") << (nfa.m_FinalStates.count(state) ? "<" : " ") << state;

            for (const auto &symbol : nfa.m_Alphabet) {
                if (auto iter = nfa.m_Transitions.find({state, symbol}); iter != nfa.m_Transitions.end()) {
                    os << " ";
                    for (auto i = iter->second.begin(); i != iter->second.end(); ++i) {
                        os << (i != iter->second.begin() ? "|" : "") << *i;
                    }
                } else {
                    os << " -";
                }
            }
            os << std::endl;
        }
        return os;
    }

    bool operator==(const automaton::NFA &a, const automaton::NFA &b) {
        return std::tie(a.m_States, a.m_Alphabet, a.m_Transitions, a.m_InitialState, a.m_FinalStates) ==
               std::tie(b.m_States, b.m_Alphabet, b.m_Transitions, b.m_InitialState, b.m_FinalStates);
    }
}
#endif


automaton::NFA recconvert(const regexp::RegExp &regexp, int &counter) {
    // TODO: implement
    automaton::NFA a = {};


    std::visit(overloaded{
                       [&a, &counter](const std::shared_ptr<regexp::Alternation> &arg) {
                           automaton::NFA left = recconvert(arg->m_left,counter);
                           automaton::NFA right = recconvert(arg->m_right,counter);

                           a.m_FinalStates.insert(left.m_FinalStates.begin(), left.m_FinalStates.end()),
                           a.m_FinalStates.insert(right.m_FinalStates.begin(), right.m_FinalStates.end()),
                           a.m_States.insert(left.m_States.begin(), left.m_States.end()),
                           a.m_States.insert(right.m_States.begin(), right.m_States.end()),
                           a.m_Alphabet.insert(left.m_Alphabet.begin(), left.m_Alphabet.end()),
                           a.m_Alphabet.insert(right.m_Alphabet.begin(), right.m_Alphabet.end()),
//                           a.m_Alphabet.insert(alphabet::Symbol('|'));
                           a.m_Transitions.insert(left.m_Transitions.begin(), left.m_Transitions.end()),
                           a.m_Transitions.insert(right.m_Transitions.begin(), right.m_Transitions.end());



//                           counter++;
//                           a.m_InitialState = counter,
//                           a.m_States.insert(a.m_InitialState),
//                           a.m_Transitions[{a.m_InitialState, alphabet::Symbol('|')}].insert({left.m_InitialState});
//                           a.m_Transitions[{a.m_InitialState, alphabet::Symbol('|')}].insert({right.m_InitialState});





                           a.m_InitialState = left.m_InitialState;


//                           counter++;
//                           a.m_FinalStates.insert(counter),
//                           a.m_States.insert(counter);


//                           for (auto &x : right.m_Transitions) {
//                               for (auto &y : x.second) {
//                                   if (auto iter = right.m_FinalStates.find(y); iter != right.m_FinalStates.end())
//                                       a.m_Transitions[{x.first.first, x.first.second}].insert(counter);
//                               }
//                               if (auto iter = right.m_FinalStates.find(x.first.first); iter != right.m_FinalStates.end())
//                                   for (auto &y : x.second) {
//                                       a.m_Transitions[{counter, x.first.second}].insert(y);
//                                   }
//
//                           }


//                           for (auto &x : left.m_Transitions) {
//                               for (auto &y : x.second) {
//                                   if (auto iter = left.m_FinalStates.find(y); iter != left.m_FinalStates.end())
//                                       a.m_Transitions[{x.first.first, x.first.second}].insert(counter);
//                               }
//                               if (auto iter = left.m_FinalStates.find(x.first.first); iter != left.m_FinalStates.end())
//                                   for (auto &y : x.second) {
//                                       a.m_Transitions[{counter, x.first.second}].insert(y);
//                                   }
//                           }


                           if (auto iter = left.m_FinalStates.find(left.m_InitialState); iter != left.m_FinalStates.end()){
                               a.m_FinalStates.insert(a.m_InitialState);
                           }

                           if (auto iter = right.m_FinalStates.find(right.m_InitialState); iter != right.m_FinalStates.end()){
                               a.m_FinalStates.insert(a.m_InitialState);
                           }


//                           for (auto &x : right.m_Transitions) {
//                               if (auto iter = right.m_FinalStates.find(x.first.first); iter !=right.m_FinalStates.end())
//                                   for (auto &y : left.m_FinalStates) {
//                                       a.m_Transitions[{y, x.first.second}].insert(x.second.begin(),x.second.end());
//                                   }
//                           }


                           for (auto &x : right.m_Transitions) {
                               for (auto &y : x.second) {
                                   if (right.m_InitialState == y)
                                       a.m_Transitions[{x.first.first, x.first.second}].insert(left.m_InitialState);
                               }
                           }

                           for (auto &x : right.m_Transitions) {
                               if (right.m_InitialState == x.first.first)
                                   a.m_Transitions[{left.m_InitialState, x.first.second}].insert(x.second.begin(),x.second.end());
                           }

                           counter++;
                       },
                       [&a, &counter](const std::shared_ptr<regexp::Concatenation> &arg) {

                           automaton::NFA left = recconvert(arg->m_left,counter);
                           automaton::NFA right = recconvert(arg->m_right,counter);
                           a.m_InitialState = left.m_InitialState,
                           a.m_FinalStates = right.m_FinalStates,
                           a.m_States.insert(left.m_States.begin(), left.m_States.end()),
                           a.m_States.insert(right.m_States.begin(), right.m_States.end()),
                           a.m_Alphabet.insert(left.m_Alphabet.begin(), left.m_Alphabet.end()),
                           a.m_Alphabet.insert(right.m_Alphabet.begin(), right.m_Alphabet.end()),
                           a.m_Transitions.insert(left.m_Transitions.begin(), left.m_Transitions.end()),
                           a.m_Transitions.insert(right.m_Transitions.begin(), right.m_Transitions.end());


                           for (auto &x : right.m_Transitions) {
                                   if (x.first.first == right.m_InitialState){
                                       for (auto &y : left.m_FinalStates) {
                                           for(auto &z : x.second){
                                               a.m_Transitions[{y, x.first.second}].insert({z});
                                           }
                                       }
                                   }

                           };




                           if(auto iter = right.m_FinalStates.find(right.m_InitialState); iter != right.m_FinalStates.end()){
                                   for (auto &y : left.m_FinalStates) {
                                   a.m_FinalStates.insert(y);
                                   }
                               }




//                           for (auto &x : left.m_FinalStates) {
//                               a.m_Transitions[{x, alphabet::Symbol('|')}].insert({right.m_InitialState});
//                           }
//
//                           a.m_Alphabet.insert(alphabet::Symbol('|'));

                       },
                       [&a, &counter](const std::shared_ptr<regexp::Iteration> &arg) {
                           automaton::NFA mid = recconvert(arg->m_node,counter);
                           a.m_States.insert(mid.m_States.begin(), mid.m_States.end()),

//                           counter++;
//                           a.m_InitialState = counter,
//                           a.m_States.insert(counter);
                           a.m_InitialState = mid.m_InitialState,
                           //a.m_Transitions[{a.m_InitialState, alphabet::Symbol('|')}].insert({mid.m_InitialState});

                           a.m_FinalStates = mid.m_FinalStates,

                           a.m_Alphabet.insert(mid.m_Alphabet.begin(), mid.m_Alphabet.end()),
                           a.m_Transitions.insert(mid.m_Transitions.begin(), mid.m_Transitions.end());


                           counter++;
                           a.m_States.insert(counter);


                           for (auto &x : mid.m_Transitions) {
                               for (auto &y : x.second) {
                                   if (auto iter = mid.m_FinalStates.find(y); iter != mid.m_FinalStates.end())
                                       a.m_Transitions[{x.first.first, x.first.second}].insert({counter});
                               }
                           }


                           for (auto &x : a.m_Transitions) {
                               if(x.first.first == a.m_InitialState){
                                   for (auto &y : x.second) {
                                       a.m_Transitions[{counter, x.first.second}].insert({y});
                                   }
                               }
                           }




                           a.m_FinalStates.insert(a.m_InitialState);
//                           a.m_Alphabet.insert(alphabet::Symbol('|'));
//                           counter++;
//                           a.m_FinalStates.insert(counter);
//                           a.m_States.insert(counter);
//                           for (auto &x : mid.m_FinalStates) {
//                               a.m_Transitions[{x, alphabet::Symbol('|')}].insert({mid.m_InitialState});
//                               a.m_Transitions[{x, alphabet::Symbol('|')}].insert({counter});
//                           }
//                           a.m_Transitions[{a.m_InitialState, alphabet::Symbol('|')}].insert({counter});
                           counter++;
                       },
                       [&a, &counter](const std::shared_ptr<regexp::Symbol> &arg) {
                           a.m_Alphabet.insert(arg->m_symbol), a.m_InitialState = counter, a.m_States.insert(counter), counter++, a.m_States.insert(counter),
                                   a.m_FinalStates.insert(counter), counter++, a.m_Transitions.insert({{counter-2, arg->m_symbol},
                                                                                                            {counter -1}});
                       },
                       [&a, &counter](const std::shared_ptr<regexp::Epsilon> &arg) {
                            a.m_InitialState = counter, a.m_States.insert(counter),
                                   a.m_FinalStates.insert(counter), counter++;
                           },
                       [&a, &counter](const std::shared_ptr<regexp::Empty> &arg) {
                           a.m_InitialState = counter, a.m_States.insert(counter), counter++;
                           },
               },
               regexp);




    std::cout << a << std::endl;
    return a;
}


automaton::NFA convert(const regexp::RegExp &regexp) {
    // TODO: implement
    int counter = 0;
    automaton::NFA a = {};

    a = recconvert(regexp,counter);

    //std::cout << a << std::endl;
    //std::map<std::pair<automaton::State, alphabet::Symbol>, std::set<automaton::State>> newTrans;

//    //epsilon closure
//    for (auto &x : a.m_States) {
//        auto copystates = a.m_States;
//        std::queue<automaton::State> copystates2;
//        std::set<automaton::State> epsilonClosure;
//        copystates.erase(x);
//        copystates2.push(x);
//        while (!copystates2.empty()) {
//            auto curr = copystates2.front();
//            copystates2.pop();
//            epsilonClosure.insert(curr);
//            for (auto &y : a.m_Transitions) {
//                if (y.first.second == '|' && y.first.first == curr) {
//                    for (auto &e : y.second) {
//                        if (auto iter = copystates.find(e); iter != copystates.end()) {
//                            copystates.erase(e);
//                            copystates2.push(e);
//                            //a.m_Transitions.erase(y.first);
//                        }
//                    }
//
//                }
//            }
//        }
//        for (auto &z : epsilonClosure) {
//                for (auto &t : a.m_Transitions) {
//                    if (t.first.second != '|' && t.first.first == z){
//                        for (auto &e : t.second) {
//                            newTrans[{x, t.first.second}].insert({e});
//                        }
//                    }
//                }
//            if (auto iter = a.m_FinalStates.find(z); iter != a.m_FinalStates.end()){
//                a.m_FinalStates.insert(x);
//            }
//        }
//    }
//    a.m_Transitions = newTrans;
//    a.m_Alphabet.erase('|');






    return a;
}

#ifndef __PROGTEST__
regexp::RegExp tests[] = {
                        std::make_shared<regexp::Iteration>(
                                std::make_shared<regexp::Alternation>(
                                        std::make_shared<regexp::Symbol>('a'),
                                        std::make_shared<regexp::Symbol>('b'))),



//        std::make_shared<regexp::Iteration>(
//                std::make_shared<regexp::Alternation>(
//                        std::make_shared<regexp::Symbol>('a'),
//                        std::make_shared<regexp::Symbol>('b'))),


//                        std::make_shared<regexp::Iteration>(
//                                std::make_shared<regexp::Concatenation>(
//                                        std::make_shared<regexp::Symbol>('j'),
//                                        std::make_shared<regexp::Iteration>(
//                                                std::make_shared<regexp::Empty>()))),


        std::make_shared<regexp::Iteration>(
                std::make_shared<regexp::Concatenation>(
                        std::make_shared<regexp::Iteration>(
                                std::make_shared<regexp::Alternation>(
                                        std::make_shared<regexp::Symbol>('a'),
                                        std::make_shared<regexp::Symbol>('b'))),
                        std::make_shared<regexp::Concatenation>(
                                std::make_shared<regexp::Symbol>('a'),
                                std::make_shared<regexp::Concatenation>(
                                        std::make_shared<regexp::Symbol>('b'),
                                        std::make_shared<regexp::Iteration>(
                                                std::make_shared<regexp::Alternation>(
                                                        std::make_shared<regexp::Symbol>('a'),
                                                        std::make_shared<regexp::Symbol>('b'))))))),

        std::make_shared<regexp::Iteration>(
                std::make_shared<regexp::Alternation>(
                        std::make_shared<regexp::Alternation>(
                                std::make_shared<regexp::Symbol>('a'),
                                std::make_shared<regexp::Epsilon>()),
                        std::make_shared<regexp::Alternation>(
                                std::make_shared<regexp::Symbol>('b'),
                                std::make_shared<regexp::Empty>()))),

        std::make_shared<regexp::Concatenation>(
                std::make_shared<regexp::Iteration>(
                        std::make_shared<regexp::Concatenation>(
                                std::make_shared<regexp::Symbol>('a'),
                                std::make_shared<regexp::Concatenation>(
                                        std::make_shared<regexp::Alternation>(
                                                std::make_shared<regexp::Empty>(),
                                                std::make_shared<regexp::Epsilon>()),
                                        std::make_shared<regexp::Concatenation>(
                                                std::make_shared<regexp::Iteration>(
                                                        std::make_shared<regexp::Symbol>('b')),
                                                std::make_shared<regexp::Symbol>('a'))))),
                std::make_shared<regexp::Alternation>(
                        std::make_shared<regexp::Concatenation>(
                                std::make_shared<regexp::Symbol>('b'),
                                std::make_shared<regexp::Iteration>(
                                        std::make_shared<regexp::Symbol>('c'))),
                        std::make_shared<regexp::Concatenation>(
                                std::make_shared<regexp::Iteration>(
                                        std::make_shared<regexp::Symbol>('a')),
                                std::make_shared<regexp::Alternation>(
                                        std::make_shared<regexp::Epsilon>(),
                                        std::make_shared<regexp::Concatenation>(
                                                std::make_shared<regexp::Symbol>('b'),
                                                std::make_shared<regexp::Empty>()))))),
};

// Your solution might give another (equivalent) answers. See task details.
automaton::NFA results[] = {
        {{0, 1, 2, 3, 4, 5, 6},
                {'a', 'b'},
                {
                        {{0, 'a'}, {1, 3}},
                        {{0, 'b'}, {2}},
                        {{1, 'a'}, {1, 3}},
                        {{1, 'b'}, {2}},
                        {{2, 'a'}, {1, 3}},
                        {{2, 'b'}, {2}},
                        {{3, 'b'}, {4}},
                        {{4, 'a'}, {1, 3, 5}},
                        {{4, 'b'}, {2, 6}},
                        {{5, 'a'}, {1, 3, 5}},
                        {{5, 'b'}, {2, 6}},
                        {{6, 'a'}, {1, 3, 5}},
                        {{6, 'b'}, {2, 6}},
                },
                0,
                {0, 4, 5, 6}},

        {{0, 1, 2},
                {'a', 'b'},
                {
                        {{0, 'a'}, {1}},
                        {{0, 'b'}, {2}},
                        {{1, 'a'}, {1}},
                        {{1, 'b'}, {2}},
                        {{2, 'a'}, {1}},
                        {{2, 'b'}, {2}},
                },
                0,
                {0, 1, 2}},

        {{0, 1, 2, 3, 4, 5, 6, 7},
                {'a', 'b', 'c'},
                {
                        {{0, 'a'}, {1, 6}},
                        {{0, 'b'}, {4, 7}},
                        {{1, 'a'}, {3}},
                        {{1, 'b'}, {2}},
                        {{2, 'a'}, {3}},
                        {{2, 'b'}, {2}},
                        {{3, 'a'}, {1, 6}},
                        {{3, 'b'}, {4, 7}},
                        {{4, 'c'}, {5}},
                        {{5, 'c'}, {5}},
                        {{6, 'a'}, {6}},
                        {{6, 'b'}, {7}},
                },
                0,
                {0, 3, 4, 5, 6}},
};

int main() {
   std::ostringstream ss;
   to_string(tests[0],ss);
   std::cout << ss.str() << std::endl;
   automaton::NFA a = convert(tests[3]);
   std::cout << a<< std::endl;
   std::cout << results[0] << std::endl;
   assert(convert(tests[0]) == results[0]);
   assert(convert(tests[1]) == results[1]);
   assert(convert(tests[2]) == results[2]);
}

#endif

