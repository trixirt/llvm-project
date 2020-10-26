// RUN: %check_clang_tidy %s linuxkernel-switch-semi %t

int f(int a) {
  switch (a) {
  case 1:
    return 0;
  default:
    break;
  };
  // CHECK-MESSAGES: warning: unneeded semicolon
  // CHECK-MESSAGES: note: FIX-IT applied suggested code changes
  return 0;
}

// A normal switch, should not produce a warning
int p1(int a) {
  switch (a) {
  case 1:
    return 0;
  default:
    break;
  }
  return 0;
}

#define EMPTY_MACRO()
// A corner case, do not fix an empty macro
int p2(int a) {
  switch (a) {
  case 1:
    return 0;
  default:
    break;
  }
  EMPTY_MACRO();
  return 0;
}
