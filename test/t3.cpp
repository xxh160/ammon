#include <clang/Basic/LangOptions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendOptions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <cstdlib>
#include <ctime>
#include <bi.h>
#include <debug.h>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace ast_matchers;
using namespace std;

static cl::OptionCategory ammonCategory("Ammon options");

int main(int argc, const char **argv) {
  // 随机数种子
  std::srand(std::time(0));

  // 命令行选项
  llvm::Expected<CommonOptionsParser> expectedParser =
      CommonOptionsParser::create(argc, argv, ammonCategory);
  if (!expectedParser) {
    ERROR("CommandLine Error");
    return 1;
  }
  CommonOptionsParser &optionsParser = expectedParser.get();

  // 初始化 RefactoringTool
  RefactoringTool tool(optionsParser.getCompilations(),
                       optionsParser.getSourcePathList());

  map<string, vector<BugInfo>> bugInfos;
  // 获取缺陷插入者
  BugInserter *bugInserter = randomBugInserter(bugInfos, tool.getReplacements());

  // 获得 AST Matcher
  StatementMatcher binaryOpMatcher = bugInserter->getMatcher();

  // 初始化 Match Finder
  MatchFinder finder;
  finder.addMatcher(binaryOpMatcher, bugInserter);

  // 匹配节点
  if (tool.run(newFrontendActionFactory(&finder).get())) {
    ERROR("Error running tool");
    return 1;
  }

  CompilerInstance ci;
  for (const string &v : optionsParser.getSourcePathList()) {
    ci.getFrontendOpts().Inputs.push_back(FrontendInputFile(v, InputKind::CXX));
  }
  ci.getFrontendOpts().Inputs.push_back(FrontendInputFile(optionsParser.getSourcePathList()))

  LangOptions langOpts = LangOptions();
  langOpts.CPlusPlus = 1;
  langOpts.CPlusPlus17 = 1;

  for (auto &entry : bugInfos) {
    auto &v = entry.second;
    for (BugInfo cur : v) {
      Rewriter re(*(cur.sourceManager), langOpts);
      // re.getEditBuffer(cur.sourceManager->getMainFileID())
      //   .write(llvm::outs());
    }
    std::cout << "\n";
  }

  return 0;
}
