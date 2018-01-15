# longjmp

The development version of R provides a mechanism for safely evaluating C++
code that might need to throw an exception. This package attempts to exercise
that.

Unfortunately, I see this when running with R on macOS:

``` r
> longjmp()
Error: ouch!
memory.c:3747:5: runtime error: member access within null pointer of type 'struct SEXPREC'
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior memory.c:3747:5 in 
memory.c:3747:5: runtime error: member access within null pointer of type 'struct sxpinfo_struct'
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior memory.c:3747:5 in 
ASAN:DEADLYSIGNAL
=================================================================
==73405==ERROR: AddressSanitizer: SEGV on unknown address 0x606000042760 (pc 0x000101480b59 bp 0x7ffeefbfad70 sp 0x7ffeefbfad40 T0)
==73405==The signal is caused by a READ memory access.
    #0 0x101480b58 in SETCAR memory.c:3747
    #1 0x101278714 in R_UnwindProtect context.c:904
    #2 0x1160fbbf3 in longjmp longjmp.cpp:26
    #3 0x1012b3039 in R_doDotCall dotcode.c:567
    #4 0x10130013d in do_dotcall dotcode.c:1252
    #5 0x1013aeccd in Rf_eval eval.c:727
    #6 0x1013f6679 in do_begin eval.c:2362
    #7 0x1013ae97d in Rf_eval eval.c:699
    #8 0x1013f1f04 in R_execClosure eval.c
    #9 0x1013aee38 in Rf_eval eval.c:747
    #10 0x101461118 in Rf_ReplIteration main.c:258
    #11 0x101465160 in R_ReplConsole main.c:308
    #12 0x101464f37 in run_Rmainloop main.c:1082
```

The crash seems to be occurring here:

https://github.com/wch/r-source/blob/ee900c7d2c5551e2c1a0e6c21254bf10bb8b92d7/src/main/context.c#L904

which suggests that either `cont` or `R_ReturnedValue` is null -- I suspect
`R_ReturnedValue` is null here.

Also, the exception is not caught at all using the 32-bit version of R on
Windows, and the uncaught exception causes a termination of the R process.
