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
#include <exception>
#include <exp.h>
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

namespace {

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
  AmmonFrontendAction() = default;

  void EndSourceFileAction() override {
    // 处理完文件后, 进行的操作
    auto mainFileId = rewriter.getSourceMgr().getMainFileID();
    int num = all.size();

    if (num == 0) {
      // 无效的 ASTConsumer
      throw InvalidConsumerExp();
    }

    // 随机选择一个位置进行改写
    int rand = randomNumber(0, num);
    all[rand](rewriter);

    // 将整个修改后的文件写入 llvm 输出
    rewriter.getEditBuffer(mainFileId).write(outs());
  }

  unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                            StringRef file) override {
    // 静默源程序处理输出
    ci.getDiagnostics().setClient(new SilentDiagConsumer(), true);

    rewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());

    // 为每个文件随机创建一个 ASTConsumer
    return ASTConsumerFactory::randomASTConsumer(all);
  }
};

} // namespace

int main(int argc, const char **argv) {
  // 空命令行选项
  cl::OptionCategory ammonCategory("Ammon options");
  Expected<CommonOptionsParser> expectedParser =
      CommonOptionsParser::create(argc, argv, ammonCategory);
  if (!expectedParser) {
    return 1;
  }

  CommonOptionsParser &op = expectedParser.get();
  // 目前只考虑处理一个文件
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  outs() << "==== Start " << op.getSourcePathList()[0] << " ====\n";

  while (true) {
    // 如果这次选择的 consumer 无效, 则多次选择
    try {
      Tool.run(newFrontendActionFactory<AmmonFrontendAction>().get());
    } catch (const InvalidConsumerExp &e) {
      continue; // 继续循环就行了
    } catch (const exception &e) {
      errs() << e.what() << "\n";
    } catch (...) {
      errs() << "Something Unknown Happened\n";
    }

    break;
  }

  outs() << "==== End ====\n";

  return 0;
}
