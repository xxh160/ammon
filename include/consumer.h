#ifndef CONSUMER_H_
#define CONSUMER_H_

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <cstdint>
#include <string>
#include <utility>

class CustomASTConsumer : public clang::ASTConsumer {
protected:
  clang::ast_matchers::MatchFinder finder;
  std::unique_ptr<clang::ast_matchers::MatchFinder::MatchCallback> callback;

public:
  virtual std::string name() const = 0;
};

class OperatorSemantic : public CustomASTConsumer {
public:
  explicit OperatorSemantic(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class VariableSemantic : public CustomASTConsumer {
public:
  explicit VariableSemantic(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class OperandSemantic : public CustomASTConsumer {
public:
  explicit OperandSemantic(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class UndefVarSyntax : public CustomASTConsumer {
public:
  explicit UndefVarSyntax(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class ASTConsumerFactory {
public:
  static std::unique_ptr<clang::ASTConsumer>
  randomASTConsumer(std::vector<std::function<void(clang::Rewriter &)>> &all);
};

#endif
