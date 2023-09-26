#include <bi.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Core/Replacement.h>
#include <debug.h>
#include <clang/Rewrite/Core/Rewriter.h>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace ast_matchers;
using namespace std;

static string id = "binaryOperator";

OperatorBugInserter::OperatorBugInserter(map<string, vector<BugInfo>> &bugInfos,
  map<string, Replacements> &m) : BugInserter(bugInfos, m) {}

// Check if the operator is one of the math operators (+, -, *, /)
static bool isNeededOperator(BinaryOperatorKind opcode) {
  return opcode == BO_Add || opcode == BO_Sub || opcode == BO_Mul ||
         opcode == BO_Div;
}

// Get a random operator to replace the current operator
static std::string getRandomOperator(BinaryOperatorKind opcode) {
  const std::string operators[] = {"+", "-", "*", "/"};
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

void OperatorBugInserter::run(const MatchFinder::MatchResult &result) {
  // 通过 id 获取对应的操作符
  const BinaryOperator *op =
      result.Nodes.getNodeAs<BinaryOperator>(id);
  // 源代码管理
  SourceManager *sourceMgr = result.SourceManager;

  if (op &&
      isNeededOperator(op->getOpcode()) &&
      sourceMgr->isInMainFile(op->getOperatorLoc())) {
    // 操作符开始位置
    SourceLocation startLoc = op->getOperatorLoc();

    // 应该被替换的操作符
    std::string replaceOp = getRandomOperator(op->getOpcode());

    // replacement 对象
    Replacement replacement(*sourceMgr, startLoc, 1, replaceOp);

    // 当前文件路径
    std::string filePath = replacement.getFilePath().str();

    // 将当前 replace 操作放入 map 中
    if (m.count(filePath) == 0) {
      m[filePath] = Replacements(replacement);
      bugInfos[filePath].push_back(BugInfo(replacement, sourceMgr,
                                           op->getOperatorLoc(),
                                           op->getOpcodeStr().size()));
    } else if (m[filePath].add(replacement)) {
      bugInfos[filePath].push_back(BugInfo(replacement, sourceMgr,
                                           op->getOperatorLoc(),
                                           op->getOpcodeStr().size()));
    } else {
      ERROR("Replacement add error");
    }
  }
}

StatementMatcher OperatorBugInserter::getMatcher() {
  StatementMatcher binaryOpMatcher = binaryOperator().bind(id);
  return binaryOpMatcher;
}
