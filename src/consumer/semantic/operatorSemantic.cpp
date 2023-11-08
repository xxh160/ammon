#include <algorithm>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <debug.h>
#include <functional>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <util.h>
#include <vector>

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace std;

namespace {

const string desc = "This bug is a semantic error caused by the use of an "
                    "incorrect operator in an expression";
const string id = "binaryOperator";

vector<vector<string>> binaryOperators = {
    {"+", "-", "*", "/", "%"},          // Arithmetic
    {"<", "<=", ">", ">=", "==", "!="}, // Comparison
    {"&&", "||"},                       // Logical
    {"&", "|", "^"},                    // Bitwise
    {"=", "+=", "-=", "*=", "/=", "%="} // Assignment
};

string nxtStr(string &cur) {
  string nxt = cur;
  for (auto &line : binaryOperators) {
    if (find(line.begin(), line.end(), cur) == line.end())
      continue;
    int s = line.size();
    // if (s == 1) break;

    int rand = randomNumber(0, s);
    nxt = line[rand];
    while (nxt == cur) {
      rand = randomNumber(0, s);
      nxt = line[rand];
    }
    break;
  }
  return nxt;
}

class OperatorCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit OperatorCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const auto *bo = result.Nodes.getNodeAs<BinaryOperator>(id);
    SourceManager *sm = result.SourceManager;
    bool isInMain = sm->isInMainFile(bo->getOperatorLoc());

    if (bo && isInMain) {
      string cur = bo->getOpcodeStr().str();
      string nxt = nxtStr(cur);

      function<void(Rewriter &)> f = [bo, sm, cur, nxt](Rewriter &r) {
        const SourceLocation &loc = bo->getOperatorLoc();
        // 设置 rewriter
        r.ReplaceText(loc, cur.size(), nxt);

        outs() << sm->getPresumedLineNumber(loc) << " "
               << sm->getPresumedColumnNumber(loc) << " " << cur << " " << nxt
               << "\n"
               << desc << "\n";
      };

      all.push_back(f);
    }
  }
};

} // namespace

OperatorSemantic::OperatorSemantic(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new OperatorCallback(all)));

  // 寻找所有的二元操作符
  finder.addMatcher(binaryOperator(isExpansionInMainFile()).bind(id),
                    this->callbacks[0].get());
}

void OperatorSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string OperatorSemantic::name() const { return "OperatorSemantic"; }
