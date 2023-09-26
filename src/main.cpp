#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <consumer.h>
#include <cstdint>
#include <debug.h>
#include <functional>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <util.h>
#include <utility>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using namespace std;

// 空命令行选项
static cl::OptionCategory ammonCategory("Ammon options");

// 用于将源程序本身的警告屏蔽掉
class SilentDiagConsumer : public clang::DiagnosticConsumer {
public:
  void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                        const clang::Diagnostic &Info) override {}
};

// Frontend action
class AmmonFrontendAction : public ASTFrontendAction {
private:
  Rewriter rewriter;
  vector<function<void(Rewriter &)>> all;

public:
  AmmonFrontendAction() : rewriter(), all() {}

  void EndSourceFileAction() override {
    // 处理完文件后, 进行的操作
    auto mainFileId = rewriter.getSourceMgr().getMainFileID();
    int num = all.size();

    if (num == 0) {
      llvm::outs() << "==== End ====\n";
      return;
    }

    // 随机选择一个位置进行改写
    int rand = randomNumber(0, num);
    all[rand](rewriter);
    rewriter.getEditBuffer(mainFileId).write(llvm::outs());

    llvm::outs() << "==== End ====\n";
  }

  unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                            StringRef file) override {
    llvm::outs() << "==== Start " << file.str() << " ====\n";

    // 静默源程序处理输出
    ci.getDiagnostics().setClient(new SilentDiagConsumer(), true);

    // 为每个文件随机创建一个 ASTConsumer
    rewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());
    return ASTConsumerFactory::randomASTConsumer(all);
  }
};

int main(int argc, const char **argv) {
  // 命令行选项
  llvm::Expected<CommonOptionsParser> expectedParser =
      CommonOptionsParser::create(argc, argv, ammonCategory);
  if (!expectedParser) {
    return 1;
  }

  CommonOptionsParser &op = expectedParser.get();
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<AmmonFrontendAction>().get());
}
