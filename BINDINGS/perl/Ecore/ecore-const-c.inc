#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_17 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ECORE_EVENT_COUNT ECORE_EXE_RESPAWN ECORE_POLLER_CORE */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'O':
    if (memEQ(name, "ECORE_POLLER_CORE", 17)) {
    /*                      ^                */
      *iv_return = ECORE_POLLER_CORE;
      return PERL_constant_ISIV;
    }
    break;
  case 'V':
    if (memEQ(name, "ECORE_EVENT_COUNT", 17)) {
    /*                      ^                */
#ifdef ECORE_EVENT_COUNT
      *iv_return = ECORE_EVENT_COUNT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'X':
    if (memEQ(name, "ECORE_EXE_RESPAWN", 17)) {
    /*                      ^                */
      *iv_return = ECORE_EXE_RESPAWN;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ECORE_CALLBACK_RENEW ECORE_EXE_NOT_LEADER ECORE_EXE_PIPE_ERROR
     ECORE_EXE_PIPE_WRITE */
  /* Offset 17 gives the best switch position.  */
  switch (name[17]) {
  case 'D':
    if (memEQ(name, "ECORE_EXE_NOT_LEADER", 20)) {
    /*                                ^         */
      *iv_return = ECORE_EXE_NOT_LEADER;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "ECORE_EXE_PIPE_WRITE", 20)) {
    /*                                ^         */
      *iv_return = ECORE_EXE_PIPE_WRITE;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "ECORE_CALLBACK_RENEW", 20)) {
    /*                                ^         */
#ifdef ECORE_CALLBACK_RENEW
      *iv_return = ECORE_CALLBACK_RENEW;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'R':
    if (memEQ(name, "ECORE_EXE_PIPE_ERROR", 20)) {
    /*                                ^         */
      *iv_return = ECORE_EXE_PIPE_ERROR;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/usr/bin/perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV)};
my @names = (qw(ECORE_CALLBACK_CANCEL ECORE_CALLBACK_RENEW ECORE_EVENT_COUNT
	       ECORE_EVENT_NONE ECORE_EVENT_SIGNAL_EXIT ECORE_EVENT_SIGNAL_HUP
	       ECORE_EVENT_SIGNAL_POWER ECORE_EVENT_SIGNAL_REALTIME
	       ECORE_EVENT_SIGNAL_USER ECORE_EXE_PRIORITY_INHERIT FALSE TRUE),
            {name=>"ECORE_EXE_NOT_LEADER", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_AUTO", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_ERROR", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_ERROR_LINE_BUFFERED", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_READ", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_READ_LINE_BUFFERED", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_PIPE_WRITE", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_RESPAWN", type=>"IV", macro=>"1"},
            {name=>"ECORE_EXE_USE_SH", type=>"IV", macro=>"1"},
            {name=>"ECORE_FD_ERROR", type=>"IV", macro=>"1"},
            {name=>"ECORE_FD_READ", type=>"IV", macro=>"1"},
            {name=>"ECORE_FD_WRITE", type=>"IV", macro=>"1"},
            {name=>"ECORE_POLLER_CORE", type=>"IV", macro=>"1"});

print constant_types(), "\n"; # macro defs
foreach (C_constant ("EFL::Ecore", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "\n#### XS Section:\n";
print XS_constant ("EFL::Ecore", $types);
__END__
   */

  switch (len) {
  case 4:
    if (memEQ(name, "TRUE", 4)) {
#ifdef TRUE
      *iv_return = TRUE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 5:
    if (memEQ(name, "FALSE", 5)) {
#ifdef FALSE
      *iv_return = FALSE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 13:
    if (memEQ(name, "ECORE_FD_READ", 13)) {
      *iv_return = ECORE_FD_READ;
      return PERL_constant_ISIV;
    }
    break;
  case 14:
    /* Names all of length 14.  */
    /* ECORE_FD_ERROR ECORE_FD_WRITE */
    /* Offset 12 gives the best switch position.  */
    switch (name[12]) {
    case 'O':
      if (memEQ(name, "ECORE_FD_ERROR", 14)) {
      /*                           ^        */
        *iv_return = ECORE_FD_ERROR;
        return PERL_constant_ISIV;
      }
      break;
    case 'T':
      if (memEQ(name, "ECORE_FD_WRITE", 14)) {
      /*                           ^        */
        *iv_return = ECORE_FD_WRITE;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 16:
    /* Names all of length 16.  */
    /* ECORE_EVENT_NONE ECORE_EXE_USE_SH */
    /* Offset 10 gives the best switch position.  */
    switch (name[10]) {
    case 'T':
      if (memEQ(name, "ECORE_EVENT_NONE", 16)) {
      /*                         ^            */
#ifdef ECORE_EVENT_NONE
        *iv_return = ECORE_EVENT_NONE;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'U':
      if (memEQ(name, "ECORE_EXE_USE_SH", 16)) {
      /*                         ^            */
        *iv_return = ECORE_EXE_USE_SH;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 17:
    return constant_17 (aTHX_ name, iv_return);
    break;
  case 19:
    /* Names all of length 19.  */
    /* ECORE_EXE_PIPE_AUTO ECORE_EXE_PIPE_READ */
    /* Offset 18 gives the best switch position.  */
    switch (name[18]) {
    case 'D':
      if (memEQ(name, "ECORE_EXE_PIPE_REA", 18)) {
      /*                                 D      */
        *iv_return = ECORE_EXE_PIPE_READ;
        return PERL_constant_ISIV;
      }
      break;
    case 'O':
      if (memEQ(name, "ECORE_EXE_PIPE_AUT", 18)) {
      /*                                 O      */
        *iv_return = ECORE_EXE_PIPE_AUTO;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 20:
    return constant_20 (aTHX_ name, iv_return);
    break;
  case 21:
    if (memEQ(name, "ECORE_CALLBACK_CANCEL", 21)) {
#ifdef ECORE_CALLBACK_CANCEL
      *iv_return = ECORE_CALLBACK_CANCEL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 22:
    if (memEQ(name, "ECORE_EVENT_SIGNAL_HUP", 22)) {
#ifdef ECORE_EVENT_SIGNAL_HUP
      *iv_return = ECORE_EVENT_SIGNAL_HUP;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 23:
    /* Names all of length 23.  */
    /* ECORE_EVENT_SIGNAL_EXIT ECORE_EVENT_SIGNAL_USER */
    /* Offset 22 gives the best switch position.  */
    switch (name[22]) {
    case 'R':
      if (memEQ(name, "ECORE_EVENT_SIGNAL_USE", 22)) {
      /*                                     R      */
#ifdef ECORE_EVENT_SIGNAL_USER
        *iv_return = ECORE_EVENT_SIGNAL_USER;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'T':
      if (memEQ(name, "ECORE_EVENT_SIGNAL_EXI", 22)) {
      /*                                     T      */
#ifdef ECORE_EVENT_SIGNAL_EXIT
        *iv_return = ECORE_EVENT_SIGNAL_EXIT;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 24:
    if (memEQ(name, "ECORE_EVENT_SIGNAL_POWER", 24)) {
#ifdef ECORE_EVENT_SIGNAL_POWER
      *iv_return = ECORE_EVENT_SIGNAL_POWER;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 26:
    if (memEQ(name, "ECORE_EXE_PRIORITY_INHERIT", 26)) {
#ifdef ECORE_EXE_PRIORITY_INHERIT
      *iv_return = ECORE_EXE_PRIORITY_INHERIT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 27:
    if (memEQ(name, "ECORE_EVENT_SIGNAL_REALTIME", 27)) {
#ifdef ECORE_EVENT_SIGNAL_REALTIME
      *iv_return = ECORE_EVENT_SIGNAL_REALTIME;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 33:
    if (memEQ(name, "ECORE_EXE_PIPE_READ_LINE_BUFFERED", 33)) {
      *iv_return = ECORE_EXE_PIPE_READ_LINE_BUFFERED;
      return PERL_constant_ISIV;
    }
    break;
  case 34:
    if (memEQ(name, "ECORE_EXE_PIPE_ERROR_LINE_BUFFERED", 34)) {
      *iv_return = ECORE_EXE_PIPE_ERROR_LINE_BUFFERED;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

