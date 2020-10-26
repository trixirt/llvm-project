//===--- SwitchSemiCheck.cpp - clang-tidy----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SwitchSemiCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace linuxkernel {

void SwitchSemiCheck::registerMatchers(MatchFinder *Finder) {
  // From the reproducer
  // void foo (int a) {
  //   switch (a) {};
  // }
  // The AST
  // `-FunctionDecl
  //   |-ParmVarDecl
  //   `-CompoundStmt <--- "comp", 'C'
  //    |-SwitchStmt <-- "switch", 'S'
  //    | |-ImplicitCastExpr
  //    | | `-DeclRefExpr
  //    | `-CompoundStmt
  //    `-NullStmt <-------------- 'N'
  Finder->addMatcher(
      compoundStmt(has(switchStmt().bind("switch"))).bind("comp"), this);
}

void SwitchSemiCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *C = Result.Nodes.getNodeAs<CompoundStmt>("comp");
  const auto *S = Result.Nodes.getNodeAs<SwitchStmt>("switch");

  auto Current = C->body_begin();
  auto Next = Current;
  Next++;
  while (Next != C->body_end()) {
    if (*Current == S) {
      if (const auto *N = dyn_cast<NullStmt>(*Next)) {
        // This code has the same AST as the reproducer
        //
        // #define EMPTY()
        // void foo (int a) {
        // switch (a) {} EMPTY();
        // }
        //
        // But we do not want to remove the ; because the
        // macro may only be conditionally empty.
        // ex/ the release version of a debug macro
        //
        // So check that the NullStmt does not have a
        // leading empty macro.
        if (!N->hasLeadingEmptyMacro() && S->getBody()->getEndLoc().isValid() &&
            N->getSemiLoc().isValid()) {
          auto H = FixItHint::CreateReplacement(
              SourceRange(S->getBody()->getEndLoc(), N->getSemiLoc()), "}");
          diag(N->getSemiLoc(), "unneeded semicolon") << H;
          break;
        }
      }
    }
    Current = Next;
    Next++;
  }
}

} // namespace linuxkernel
} // namespace tidy
} // namespace clang
