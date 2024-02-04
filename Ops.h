//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_OPS_H
#define LABB1_V4_OPS_H

#include <string>
#include <vector>


struct op {
    virtual bool eval(char *&first, char *last) = 0;
    virtual std::string id() = 0;
    std::vector<op*> children;
    std::vector<std::string> capture_all;
    void add(op *child) {
        children.push_back(child);
    }
    void add(std::string capture) {
        capture_all.push_back(capture);
    }
};

struct basic_RE : op {
    bool eval(char *&first, char *last) override {
        for(auto &child : children) {
            if(!child->eval(first, last)) {
                return false;
            }
        }
        return true;
    }
    std::string id() override {
        return "basic_RE";
    }
};
struct operation_op : basic_RE {
   std::string id() override {
       return "operation_op";
   }
};
struct repeat_op : operation_op {
    /*char c;
    repeat_op(char c) : c(c) {}*/
    bool eval(char *&first, char *last) override { // TODO: fix this

        for (auto &child: children) {
            if(!child->eval(first, last)) {
                return false;
            }

        }
        return true;
    }
    std::string id() override {
        return "repeat_op";
    }
};
struct case_sensitive_op : operation_op {
        bool eval(char *&first, char *last) override {
            if(first != last && children[0]->eval(first, last)) {
                return true;
            }
            return false;
        }
        std::string id() override {
            return "case_sensitive_op";
        }
    };
struct capture_group_op : operation_op {
        std::vector<std::string> capture_all;
        int number;
         capture_group_op(int number) : number(number) {}
        bool eval(char *&first, char *last) override {
            char *start = first;
            if(children[0]->eval(first, last)) {
                capture_all.push_back(std::string(start, first));
                return true;
            }
            return false;
        }
        std::string id() override {
            return "capture_group_op";
        }
    };
struct or_op : operation_op {
    bool eval(char *&first, char *last) override {
        auto start = first;
        if(children[0]->eval(first, last)) {
            return true;
        }
        first = start;
        return children[1]->eval(first, last);
    }
    std::string id() override {
        return "or_op";
    }
};
struct count_op : operation_op {
    int count;
    count_op(int count) : count(count) {}
    bool eval(char *&first, char *last) override {
        for (int i = 0; i < count; ++i) {
            if (!children[0]->eval(first, last)) {
                return false;
            }
        }
        return true;
    }
    std::string id() override {
        return "count_op";
    }
};
struct basic_element_op : operation_op {
    std::string id() override {
        return "basic_element_op";
    }
};
struct group_op : basic_element_op {
    bool eval(char *&first, char *last) override {
        return children[0]->eval(first, last); // children[0] is basic_RE
    }
    std::string id() override {
        return "group_op";
    }
};

struct any_char_op : basic_element_op {
    bool eval(char *&first, char *last) override {
        if(first != last) {
            std::cout << "any_char_op: " << *first << std::endl;
            first++;
            return true;
        }
        return false;
    }
    std::string id() override {
        return "any_char_op";
    }
};
struct char_op : basic_element_op {
    char c;
    char_op(char c) : c(c) {}
    bool eval(char *&first, char *last) override {
        if(first != last && *first == c) {
            std::cout << "char_op: " << *first << std::endl;
            first++;
            return true;
        }
        return false;
    }
    std::string id() override {
        return "char_op"+std::string(1, c);
    }
};
struct word_op : basic_element_op {
    bool eval(char *&first, char *last) override {
        auto start = first;
        auto child_it = children.begin();
        while (child_it!= children.end() && first <= last) {
            if(!(*child_it)->eval(first, last)) {
                child_it = children.begin();
                first++;
            }else child_it++;
        }
        if (child_it == children.end()) {
            return true;
        }
        return false;
    }
    std::string id() override {
        return "word_op";
    }
};
struct program: op {
    bool eval(char *&first, char *last) override {
        for(auto &child : children) {
            if(!child->eval(first, last)) {
                return false;
            }
        }
        return true;
    }
    std::string id() override {
        return "program";
    }
};
struct  match_op : op {
    bool eval(char *&first, char *last) override {
        if(first == last) {
            return false;
        }
        return true;
    }
    std::string id() override {
        return "match_op";
    }
};

#endif //LABB1_V4_OPS_H
