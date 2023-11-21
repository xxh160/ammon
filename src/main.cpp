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
  // 这是不知道 rewriter 里边有 editBuffer 迭代器之前的设计...
  vector<function<void(Rewriter &)>> all;
  int typeOption;

public:
  AmmonFrontendAction(int t) : typeOption(t) {}

  // 处理完文件后, 进行的操作
  void EndSourceFileAction() override {
    auto mainFileId = rewriter.getSourceMgr().getMainFileID();
    int num = all.size();

    if (num == 0) {
      // 无效的 ASTConsumer
      throw InvalidConsumerExp();
    }

    // 随机选择一个位置进行改写
    int rand = randomNumber(0, num);
    // 在这里将修改存储在 rewriter 缓冲区中
    all[rand](rewriter);

    // 将整个修改后的文件写入 llvm 输出
    rewriter.getEditBuffer(mainFileId).write(outs());
  }

  unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                            StringRef file) override {
    // 静默源程序处理输出
    ci.getDiagnostics().setClient(new SilentDiagConsumer(), true);

    rewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());

    // 为每个文件创建一个 ASTConsumer
    return ASTConsumerFactory::getASTConsumer(all, typeOption);
  }
};

class AmmonFrontendActionFactory : public FrontendActionFactory {
private:
  int typeOption;

public:
  AmmonFrontendActionFactory(int t) : typeOption(t) {}

  unique_ptr<FrontendAction> create() override {
    return make_unique<AmmonFrontendAction>(typeOption);
  }
};

unique_ptr<FrontendActionFactory> newAmmon(int t) {
  return make_unique<AmmonFrontendActionFactory>(t);
}

}; // namespace

namespace {

// 命令行选项
cl::OptionCategory ammonCategory("Ammon options");

// -t <type>
// magic number?
int typeOptionInit = ALL_CONSUMER;
cl::opt<int> typeOption("t", cl::desc("Specify mutation type"),
                        cl::value_desc("type"), cl::Optional,
                        cl::init(typeOptionInit), cl::cat(ammonCategory));

void runTool(ClangTool &tool) {
  // 预留处理其它参数的空间
  if (ASTConsumerFactory::isValidType(typeOption)) {
    try {
      tool.run(newAmmon(typeOption).get());
    } catch (const InvalidConsumerExp &e) {
    } catch (...) {
      ERROR("Something Unknown Happened");
    }

    return;
  }

  // 默认是循环尝试所有 ASTConsumer, 直到有一个可以用
  while (true) {
    // 如果这次选择的 consumer 无效, 则多次选择
    try {
      tool.run(newAmmon(typeOption).get());
    } catch (const InvalidConsumerExp &e) {
      continue; // 继续循环就行了
    } catch (const exception &e) {
      ERROR(e.what());
    } catch (...) {
      ERROR("Something Unknown Happened");
    }

    break;
  }
}

} // namespace

int main(int argc, const char **argv) {
  cl::HideUnrelatedOptions(ammonCategory);
  Expected<CommonOptionsParser> expectedParser =
      CommonOptionsParser::create(argc, argv, ammonCategory);
  if (!expectedParser) {
    return 1;
  }

  CommonOptionsParser &op = expectedParser.get();
  // 只考虑处理一个文件
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  outs() << "==== Start " << op.getSourcePathList()[0] << " ====\n";

  runTool(tool);

  outs() << "==== End ====\n";

  return 0;
}
