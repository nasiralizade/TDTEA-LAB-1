//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_OPS_H
#define LABB1_V4_OPS_H

#include <iostream>
#include <vector>
#include "Parse.h"


struct op {
    virtual bool eval(char *&first, char *last) = 0;
    virtual std::string id() = 0;

    std::vector<op *> children;
    void add(op *child) {
        children.push_back(child);
    }

    bool ignore_case_ = false;
    void set_ignore_case(bool ignore_case) { ignore_case_ = ignore_case; }
};

extern int group_index_;
extern std::vector<std::string> capturedGroups;

struct char_op : op {
    char c;
  explicit char_op(char c) : c(c) {}
  bool eval(char *&first, char *last) override {
      char inputChar = *first;
      bool match =
          ignore_case_ ? tolower(inputChar) == tolower(c) : inputChar == c;
      if (match) {
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

struct Subexpression : op {
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

    std::string id() override { return "Subexpression"; }
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

  explicit exact_op(int n) : n(n) {}
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
  explicit ignore_case_op(op *child) : child(child) {
    set_ignore_case_recursive(child, true);
  }
    bool eval(char *&first, char *last) override {
    for (auto &c : child->children) {
      c->set_ignore_case(true);
    }
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
    void set_ignore_case_recursive(op *operation, bool ignore_case) {
      operation->set_ignore_case(ignore_case);
      for (auto &child1 : operation->children) {
        set_ignore_case_recursive(child1, ignore_case);
      }
    }
};

struct capture_group_op : op {
    std::string matchedSubstring;

    bool eval(char *&first, char *last) override {
        char *temp = first;
        for (auto &child: children) {
            if (!child->eval(first, last)) {
                first = temp;
                return false;
            }
            matchedSubstring.assign(temp, first);
            capturedGroups.push_back(matchedSubstring);
        }
        return true;
    }

    std::string id() override {
        return "capture_group_op";
    }
};
struct output_group_op : op {
  explicit output_group_op(int group_index) : group_index(group_index) {}
  bool eval(char *&first, char *last) override {
        group_index_ = group_index;
        return true;
    }
    std::string id() override {
        return "output_group_op";
    }

    int group_index;
};



#endif //LABB1_V4_OPS_H
