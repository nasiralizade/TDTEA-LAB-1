//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_OPS_H
#define LABB1_V4_OPS_H

#include <iostream>
#include <vector>


struct op {
    virtual bool eval(char *&first, char *last) = 0;
    virtual std::string id() = 0;

    std::vector<op *> children;
    void add(op *child) {
        children.push_back(child);
    }

    virtual ~op() {
        for (auto &child: children) {
            delete child;
        }
    }
};

struct char_op : op {
    char c;

    char_op(char c) : c(c) {}
    bool eval(char *&first, char *last) override {
        if (first != last && *first == c) {
            first++;
            return true;
        }
        return false;
    }

    std::string id() override {
        return "char_op";
    }
};

struct any_char_op : op {
    bool eval(char *&first, char *last) override {
        if (first != last) {
            first++;
            return true;
        }
        return false;
    }

    std::string id() override {
        return "any_char_op";
    }
};

struct or_op : op {
    bool eval(char *&first, char *last) override {
        char *temp = first;
        bool lhs = children[0]->eval(temp, last);
        if (lhs) {
            first = temp;
                return true;
            }
        return children[1]->eval(first, last);
    }

    std::string id() override {
        return "or_op";
    }
};

struct Sequence : op {
    bool eval(char *&first, char *last) override {
        auto start = first;
        for (auto &child: children) {
            if (!child->eval(first, last)) {
                first = start;
                return false;
            }
        }
        return true;
    }

    std::string id() override {
        return "Sequence";
    }
};

struct repeat : op {
    bool eval(char *&first, char *last) override {
        while (first != last && children[0]->eval(first, last)) {
        }
        return true;
    }
    std::string id() override {
        return "repeat";
    }
};

struct exact_op : op {
    int n;

    exact_op(int n) : n(n) {}
    bool eval(char *&first, char *last) override {
        auto start = first;
        for (int i = 0; i < n; i++) {
            if (!children[0]->eval(first, last)) {
                first = start;
                return false;
            }
        }
        return true;
    }
    std::string id() override {
        return "exact_op";
    }
};

struct ignore_case_op : op {
    op *child;

    ignore_case_op(op *child) : child(child) {}
    bool eval(char *&first, char *last) override {
        char *temp = first;
        if (child->eval(first, last)) {
            return true;
        }
        first = temp;
        return false;
    }
    std::string id() override {
        return "ignore_case_op";
    }
};

struct output_group_op : op {
    op *child;
    int group_index;
    std::string matchedText;

    output_group_op(op *child, int group_index) : child(child), group_index(group_index) {}
    bool eval(char *&first, char *last) override {
        char *temp = first;
        if (child->eval(first, last)) {
            matchedText.assign(temp, first);
            return true;
        }
        return false;
    }
    std::string id() override {
        return "output_group_op";
    }

    std::string getMatchedText() {
        return matchedText;
    }
};


#endif //LABB1_V4_OPS_H
