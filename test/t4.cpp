#include <clang/Basic/SourceLocation.h>
#include <clang/Tooling/Core/Replacement.h>
#include <ctime>
#include <bi.h>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace ast_matchers;
using namespace std;

BugInfo::BugInfo() {}

BugInfo::BugInfo(Replacement r, SourceManager *sm, SourceLocation sl, int l)
    : replacement(r), sourceManager(sm), sourceLocation(sl), length(l) {}

BugInfo &BugInfo::operator=(const BugInfo &bi) {
  this->replacement = bi.replacement;
  this->sourceManager = bi.sourceManager;
  this->sourceLocation = bi.sourceLocation;
  this->length = bi.length;
  return *this;
}

BugInserter::BugInserter(map<string, vector<BugInfo>> &bugInfos,
  map<string, Replacements> &m) : bugInfos(bugInfos), m(m) {}


enum {
  OPERATOR_BI = 0,
  OPERAND_BI,
  VARIABLE_BI,
  BI_COUNT
};

BugInserter *randomBugInserter(map<string, vector<BugInfo>> &bugInfos,
  map<string, Replacements> &m) {
  int index = rand() % BI_COUNT;
  switch (index) {
    case OPERATOR_BI:
      return new OperatorBugInserter(bugInfos, m);
    case OPERAND_BI:
      return nullptr;
    case VARIABLE_BI:
      return nullptr;
    default:
      return nullptr;
  }
}
