#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Refactoring.h>
#include <clang/Tooling/Core/Replacement.h>
#include <cstdlib>
#include <ctime>
#include <debug.h>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace ast_matchers;

// Matcher callback for binary operator expressions
class OperatorBugInserter : public MatchFinder::MatchCallback {
private:
  RefactoringTool &tool;

  // Check if the operator is one of the math operators (+, -, *, /)
  bool isNeededOperator(BinaryOperatorKind opcode) {
    return opcode == BO_Add || opcode == BO_Sub || opcode == BO_Mul ||
           opcode == BO_Div;
  }

  // Get a random operator to replace the current operator
  std::string getRandomOperator(BinaryOperatorKind opcode) {
    // Add your own logic here to select a random operator
    const std::string operators[] = { "+", "-", "*", "/" };
    const int numOperators = sizeof(operators) / sizeof(operators[0]);

    // Remove the current operator from the available options
    std::vector<std::string> availableOperators;
    for (int i = 0; i < numOperators; ++i) {
      if (operators[i] != operators[opcode - BO_Add])
        availableOperators.push_back(operators[i]);
    }

    // Select a random operator from the available options
    int randomIndex = std::rand() % (numOperators - 1);
    return availableOperators[randomIndex];
  }

public:
  OperatorBugInserter(RefactoringTool &tool) : tool(tool) {}

  // This function will be called when a matching binary operator expression is
  // found
  virtual void run(const MatchFinder::MatchResult &result) {
    // 通过 id 获取对应的操作符
    const BinaryOperator *op =
        result.Nodes.getNodeAs<BinaryOperator>("binaryOperator");
    // 源代码管理
    SourceManager *sourceMgr = result.SourceManager;

    if (op &&
        isNeededOperator(op->getOpcode()) &&
        sourceMgr->isInMainFile(op->getOperatorLoc())) {
      // 源代码管理
      SourceManager *sourceMgr = result.SourceManager;
      // 操作符开始位置
      SourceLocation startLoc = op->getOperatorLoc();
      INFO(sourceMgr->getSpellingLineNumber(startLoc));
      // 应该被替换的操作符
      std::string replaceOp = getRandomOperator(op->getOpcode());
      // replacement 对象
      Replacement replacement(*sourceMgr, startLoc, 1, replaceOp);
      INFO(replacement.getFilePath().str());
      // 当前文件路径
      std::string filePath = replacement.getFilePath().str();

      // 将当前 replace 操作放入 map 中
      std::map<std::string, Replacements> &m = tool.getReplacements();
      if (m.count(filePath) == 0) {
        m[filePath] = Replacements(replacement);
      } else if (m[filePath].add(replacement)) {
        ERROR("Replacement add error");
      }
    }
  }
};

static cl::OptionCategory ammonCategory("Ammon options");

int main(int argc, const char **argv) {
  // 随机数种子
  std::srand(std::time(0));

  // 命令行选项
  llvm::Expected<CommonOptionsParser> expectedParser =
      CommonOptionsParser::create(argc, argv, ammonCategory);
  if (!expectedParser) {
    ERROR("CommandLine error");
    return 1;
  }
  CommonOptionsParser &optionsParser = expectedParser.get();

  // 初始化 RefactoringTool
  RefactoringTool tool(optionsParser.getCompilations(),
                       optionsParser.getSourcePathList());

  // 初始化 AST Matcher, 并绑定 id
  StatementMatcher binaryOpMatcher = binaryOperator().bind("binaryOperator");

  // AST Matcher 的回调, 用于插入特定 bug
  OperatorBugInserter inserter(tool);

  // 初始化 Match Finder
  MatchFinder finder;
  finder.addMatcher(binaryOpMatcher, &inserter);

  // 运行
  if (tool.run(newFrontendActionFactory(&finder).get())) {
    ERROR("Error running tool");
    return 1;
  }

  for (auto &entry : tool.getReplacements()) {
    Replacements &fileReplacements = entry.second;
    for (auto cur : fileReplacements) {
      std::cout << cur.toString() << " ";
    }
    std::cout << "\n";
  }

  return 0;
}
