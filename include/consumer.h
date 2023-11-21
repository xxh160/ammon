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
  std::vector<std::unique_ptr<clang::ast_matchers::MatchFinder::MatchCallback>>
      callbacks;

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

class BoundSemantic : public CustomASTConsumer {
public:
  explicit BoundSemantic(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class VarUseSemantic : public CustomASTConsumer {
public:
  explicit VarUseSemantic(
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

class ForSemiSyntax : public CustomASTConsumer {
public:
  explicit ForSemiSyntax(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

class ParenSyntax : public CustomASTConsumer {
public:
  explicit ParenSyntax(
      std::vector<std::function<void(clang::Rewriter &)>> &all);

  void HandleTranslationUnit(clang::ASTContext &Context) override;

  std::string name() const override;
};

enum {
  // 语义错误
  OPERATOR_SEMANTIC = 0,
  BOUND_SEMANTIC,
  VARUSE_SEMANTIC,
  // 语法错误
  UNDEFVAR_SYNTAX,
  FORSEMI_SYNTAX,
  PAREN_SYNTAX,
  ALL_CONSUMER
};

class ASTConsumerFactory {
public:
  static std::unique_ptr<clang::ASTConsumer>
  getASTConsumer(std::vector<std::function<void(clang::Rewriter &)>> &all,
                 int type = ALL_CONSUMER);

  static bool isValidType(int type) {
    return type < ALL_CONSUMER && type >= OPERATOR_SEMANTIC;
  }
};

#endif
