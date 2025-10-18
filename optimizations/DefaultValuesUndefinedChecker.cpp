/*
******                                         ******
************                             ************
Undefined value and Array Index Out Of Bounds checker
************                             ************
******                                         ******
*/

// includes
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Checkers/Taint.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicExtent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"
#include <queue>

// namespaces
using namespace clang;
using namespace ento;
using namespace taint;
using namespace std;

namespace {

class DefaultValuesUndefinedChecker
    : public Checker<check::ASTDecl<CXXRecordDecl>> {

private:
  mutable std::unique_ptr<BugType> BT;

public:
  void checkASTDecl(const CXXRecordDecl *D, AnalysisManager &AM,
                    BugReporter &BR) const {
    if (!D->hasDefinition() || !D->hasPrivateFields())
      return;

    llvm::errs() << "Constructor checker\n";
    llvm::errs().flush();

    auto className = D->getNameAsString();
    for (const FieldDecl *field : D->fields()) {
      if (field->getAccess() == AS_private ) {
        std::string fieldName = field->getNameAsString();

      if (field->hasInClassInitializer()) {
        continue;
      }

      for(const auto *ctor:D->ctors()){
        if(!ctor->isUserProvided() || ctor->inits().empty())
          continue;

        bool thisContructorDoesentInitField = true;
        for(const auto*init:ctor->inits()){
          if(init->getMember() == field){
            thisContructorDoesentInitField = false;
            break;
          }
        }
        if(thisContructorDoesentInitField  ){
          llvm::errs() << className << " has a constructor that does not initialize private field " << fieldName << "consider giving this field a default value" << "\n";
          llvm::errs().flush();
        }

      }

      }
    }

}
};
}

// Register Undefined Value Checker

void ento::registerDefaultValuesUndefinedChecker(CheckerManager &mgr) {
  mgr.registerChecker<DefaultValuesUndefinedChecker>();
}

// This checker should be enabled regardless of how language options are set.
bool ento::shouldRegisterDefaultValuesUndefinedChecker(
    const CheckerManager &mgr) {
  return true;
}

/*
Lalalalla
3
1.cpp:6:1: warning: Base*lalalla*
 [alpha.unix.UndefinedValue]
    6 | class Base{
      | ^
1.cpp:10:1: warning: Something*lalalla*Base
 [alpha.unix.UndefinedValue]
   10 | class Something: public Base
      | ^
1.cpp:15:1: warning: Something1*lalalla*Base
 [alpha.unix.UndefinedValue]
   15 | class Something1: public Base
      | ^
1.cpp:19:1: warning: DoubleInheritance*lalalla*SomethingSomething1
 [alpha.unix.UndefinedValue]
   19 | class DoubleInheritance: public Something, public Something1
      | ^
4 warnings generated.
scan-build: Analysis run complete.
scan-build: 4 bugs found.
scan-build: Run 'scan-view /tmp/scan-build-2025-08-30-124622-69329-1' to examine
bug reports.
*/