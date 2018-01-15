#include <R.h>
#include <Rinternals.h>

class LongjumpException
{
public:

  LongjumpException(SEXP token)
    : token_(token)
  {
  }

  void resume()
  {
    ::R_ReleaseObject(token_);
    ::R_ContinueUnwind(token_);
  }

private:
  SEXP token_;
};

SEXP fun(void* data)
{
  Rf_error("ouch!\n");
}

void clean(void* data, Rboolean jump)
{
  if (jump)
  {
    SEXP token = static_cast<SEXP>(data);
    ::R_PreserveObject(token);
    throw LongjumpException(token);
  }
}

extern "C" SEXP longjmp() {
  try
  {
    SEXP token = ::R_MakeUnwindCont();
    return ::R_UnwindProtect(fun, NULL, clean, token, token);
  }
  catch (LongjumpException& e)
  {
    e.resume();
    Rprintf("Caught exception!");
  }

  return R_NilValue;
}

