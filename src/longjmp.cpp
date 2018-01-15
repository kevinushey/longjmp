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

  try
  {
    return ::R_UnwindProtect(fun, NULL, clean, NULL, s_token);
  }
  catch (LongjumpException& e)
  {
  }

  return Rf_ScalarLogical(0);
}

