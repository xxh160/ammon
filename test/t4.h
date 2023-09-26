#ifndef __MUTANT_H_
#define __MUTANT_H_

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Tooling/Core/Replacement.h>
#include <clang/Tooling/Refactoring.h>
#include <cstdint>
#include <vector>

class BugInfo {
public:
  clang::tooling::Replacement replacement;
  clang::SourceManager *sourceManager;
  clang::SourceLocation sourceLocation;
  int length;

  BugInfo();

  BugInfo(clang::tooling::Replacement r, clang::SourceManager *sm,
          clang::SourceLocation sl, int l);

  BugInfo &operator=(const BugInfo &bi);
};

class BugInserter : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  std::map<std::string, std::vector<BugInfo>> &bugInfos;
  std::map<std::string, clang::tooling::Replacements> &m;

  BugInserter(std::map<std::string, std::vector<BugInfo>> &bugInfos,
              std::map<std::string, clang::tooling::Replacements> &m);

  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) = 0;

  virtual clang::ast_matchers::StatementMatcher getMatcher() = 0;
};

class OperatorBugInserter : public BugInserter {
public:
  OperatorBugInserter(std::map<std::string, std::vector<BugInfo>> &bugInfos,
                      std::map<std::string, clang::tooling::Replacements> &m);

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

  clang::ast_matchers::StatementMatcher getMatcher() override;
};

BugInserter *
randomBugInserter(std::map<std::string, std::vector<BugInfo>> &bugInfos,
                  std::map<std::string, clang::tooling::Replacements> &m);

#endif
