#pragma once
#define TEST_CATEGORY(name) puts(name)
#define TEST_EQ2(a, b, c) {                        \
  auto t1 = a;\
  auto t2 = b;\
  if(t1 == t2){\
    printf("OK\n");\
  }else{\
    printf("FAIL\t%x != %x at %d " #b "\n", t1, t2, c);  \
  }\
  }
#define TEST_EQ(a, b) TEST_EQ2(a,b, __LINE__)
