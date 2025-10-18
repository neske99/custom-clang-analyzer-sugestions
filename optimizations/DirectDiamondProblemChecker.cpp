/*
******                                         ******
************                             ************
Undefined value and Array Index Out Of Bounds checker
************                             ************
******                                         ******
*/

// includes
#include "clang/StaticAnalyzer/Checkers/Taint.h"
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicExtent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"
#include<queue>

// namespaces
using namespace clang;
using namespace ento;
using namespace taint;
using namespace std;


namespace {

class DirectDiamondProblemChecker : public Checker<check::ASTDecl<CXXRecordDecl>, check::EndOfTranslationUnit> {

private:
  mutable std::unique_ptr<BugType> BT;
  mutable llvm::DenseMap<const CXXRecordDecl*, int> inLevel;
  mutable llvm::DenseMap<const CXXRecordDecl*, llvm::SmallVector<pair<const CXXRecordDecl*,bool>>> childrenAndTheirVirtuality;

public:
  void checkASTDecl(const CXXRecordDecl *D, AnalysisManager &AM,
                    BugReporter &BR) const {

    inLevel[D]=0;
    //forming of top sort
    for (auto it = D->bases().begin(); it != D->bases().end(); it++) {

      inLevel[D]++;
      childrenAndTheirVirtuality[it->getType()->getAsCXXRecordDecl()].push_back({D,it->isVirtual()});

    }

  }




  void checkEndOfTranslationUnit(const TranslationUnitDecl *D,
                                 AnalysisManager &AM,
                                 BugReporter &BR) const {

                                  SourceManager &SM = AM.getSourceManager();


                                llvm:: errs() << "Lalalalla\n"<<childrenAndTheirVirtuality.size()<<"\n";



                                  queue<const CXXRecordDecl*> ZeroParentsQueue;
                                  for(auto it=inLevel.begin();it!=inLevel.end(); it++) {
                                    //llvm:: errs() << it->first->getNameAsString() <<" : " << it->second<<"\n";
                                    if(it->second==0){
                                      ZeroParentsQueue.push(it->first);

                                    llvm:: errs() << it->first->getNameAsString() << " lvl zero "<<"\n";
                                    }

                                  }

                                  while(!ZeroParentsQueue.empty())
                                  {
                                    int numElements=ZeroParentsQueue.size();
                                    for(int i =0;i<numElements;i++){
                                      auto current=ZeroParentsQueue.front();
                                      ZeroParentsQueue.pop();

                                      llvm::DenseMap<const CXXRecordDecl*, pair<const CXXRecordDecl*,bool>> parentMapping;

                                      for(auto it=childrenAndTheirVirtuality[current].begin();it!=childrenAndTheirVirtuality[current].end();it++){
                                        dfsHelper(0,current,it->first,it->second,parentMapping,BR,SM);
                                        ZeroParentsQueue.push(it->first);
                                      }
                                    }

                                  }


 }

 void dfsHelper(int lvl,const CXXRecordDecl*parent,const CXXRecordDecl* child,bool isVirtual,llvm::DenseMap<const CXXRecordDecl*,pair<const CXXRecordDecl*,bool>>&parentMapping,BugReporter &BR,SourceManager&SM) const{
    //llvm:: errs() << "lvl: "<<lvl<<"\n";
    //llvm:: errs().flush();
  if(lvl==0){
    for(auto ch = childrenAndTheirVirtuality[child].begin(); ch!=childrenAndTheirVirtuality[child].end(); ch++){
      dfsHelper(lvl+1,child,ch->first,isVirtual,parentMapping,BR,SM);
    }

  } else if(lvl==1){

    //llvm:: errs()<< "child"<<child->getNameAsString()<<" parent: "<<parent->getNameAsString()<<"\n";
    //llvm:: errs().flush();
    if(parentMapping.find(child)!=parentMapping.end()){

      //TODO
      //Diamond occurs
      //print someting
      if(!isVirtual ||  !parentMapping[child].second){
        llvm::errs()<< "Diamond occurs\n";
        llvm::errs()<< "Parents: "<<parentMapping[child].first->getNameAsString()<<" :: "<<parent->getNameAsString()<<"\n";
        llvm::errs()<< "Parents virtuality: "<<parentMapping[child].second<<" :: "<<isVirtual<<"\n";
      }

    }else{
      parentMapping[child]={parent,isVirtual};
    }

  }

 }

};
}



// Register Undefined Value Checker

void ento::registerDirectDiamondProblemChecker(CheckerManager &mgr) {
  mgr.registerChecker<DirectDiamondProblemChecker>();
}

// This checker should be enabled regardless of how language options are set.
bool ento::shouldRegisterDirectDiamondProblemChecker(const CheckerManager &mgr) {
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
scan-build: Run 'scan-view /tmp/scan-build-2025-08-30-124622-69329-1' to examine bug reports.
*/