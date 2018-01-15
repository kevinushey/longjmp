#include <R.h>
#include <Rinternals.h>

static SEXP s_token;
struct LongjumpException {};

SEXP fun(void* data)
{
  Rf_error("ouch!\n");
  return Rf_ScalarLogical(1);
}

void clean(void* data, Rboolean jump)
{
  if (jump)
    throw LongjumpException();
}

extern "C" SEXP longjmp() {

  s_token = ::R_MakeUnwindCont();
  ::R_PreserveObject(s_token);

  SEXP result;
  try
  {
    result = PROTECT(::R_UnwindProtect(fun, NULL, clean, NULL, s_token));
  }
  catch (LongjumpException& e)
  {
    result = PROTECT(Rf_ScalarLogical(0));
  }

  ::R_ReleaseObject(s_token);
  UNPROTECT(1);
  return result;
}

