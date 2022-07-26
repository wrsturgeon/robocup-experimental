// No include guards; taken care of in code_checker.sh

#define MEMBER_INLINE inline __attribute__((always_inline))
#define INLINE static MEMBER_INLINE

// Let Clang aggressively optimize `for` loops
// https://clang.llvm.org/docs/LanguageExtensions.html#extensions-for-loop-hint-optimizations
#if !DEBUG
#define CLANG_OPTIMIZE_LOOP _Pragma("clang loop vectorize(enable) interleave(enable) unroll(enable) distribute(enable)")
#define for CLANG_OPTIMIZE_LOOP for
#define do CLANG_OPTIMIZE_LOOP do
#endif // !DEBUG
// #define while CLANG_OPTIMIZE_LOOP while // TODO: Issues with do...while
