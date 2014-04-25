/*
 * getopt for mruby
 *
 * Copyright (c) 2014 M&T Technology, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "mruby.h"
#include "mruby/array.h"
#include "mruby/class.h"
#include "mruby/hash.h"
#include "mruby/string.h"
#include "mruby/value.h"

/*
 * Array#getopt_long(optstring, longopts) -> Hash
 * optstring: "abc:d:012"
 * longopts: [["add",     1, 0, 0],
 *            ["append",  0, 0, 0 ],
 *            ["delete",  1, 0, 0 ],
 *            ["verbose", 0, 0, 0 ],
 *            ["create",  1, 0, 'c'],
 *            ["file",    1, 0, 0 ]
 *           ]
 */
static mrb_value
mrb_getopt_long(mrb_state *mrb, mrb_value ary)
{
  int argc;
  char **argv = NULL;
  char *optstring = NULL;
  struct option *longopts = NULL;
  int longindex;

  int i;
  mrb_value longopts_m;
  mrb_value opt_hash;

  /* optind global variable should be set to 1 each time */
  optind = 1;

  mrb_get_args(mrb, "zA", &optstring, &longopts_m);

  /* check if longopts_m items consist of Array objects only */
  for (i = 0; i < RARRAY_LEN(longopts_m); i++) {
    mrb_value longopt_m = mrb_ary_ref(mrb, longopts_m, i);
    if (mrb_type(longopt_m) != MRB_TT_ARRAY) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "The second argument must be an array of arrays!");
    }
    if (RARRAY_LEN(longopt_m) != 4) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "longopts must be an array of arrays, each of which has four items!");
    }
  }

  /* argv[0] in C contains a program name. */
  argc = RARRAY_LEN(ary) + 1;
  argv = (char**)malloc(sizeof(char*) * argc);
  if (!argv) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Memory allocation failed!");
  }
  argv[0] = "";
  for (i = 1; i < argc; i++) {
    argv[i] = mrb_str_to_cstr(mrb, mrb_ary_ref(mrb, ary, i-1));
  }
  
  /* The last item of longopts must be a struct option,
   * in which all members are null.
   */
  longopts = (struct option*)malloc(sizeof(struct option) * (RARRAY_LEN(longopts_m)+1));
  if (!longopts) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Memory allocation failed!");
  }
  for (i = 0; i < RARRAY_LEN(longopts_m); i++) {
    mrb_value value_m;
    mrb_value longopt_m = mrb_ary_ref(mrb, longopts_m, i);

    value_m = mrb_ary_ref(mrb, longopt_m, 0);
    longopts[i].name = mrb_str_to_cstr(mrb, value_m);

    value_m = mrb_ary_ref(mrb, longopt_m, 1);
    longopts[i].has_arg = mrb_fixnum(value_m);

    //value_m = mrb_ary_ref(mrb, longopt_m, 2);
    longopts[i].flag = NULL;

    //value_m = mrb_ary_ref(mrb, longopt_m, 3);
    longopts[i].val = 0;
  }
  longopts[i].name    = NULL;
  longopts[i].has_arg = 0;
  longopts[i].flag    = NULL;
  longopts[i].val     = 0;

  opt_hash = mrb_hash_new(mrb);

#ifdef DEBUG
  printf("argc: %d\n", argc);
  for (i = 0; i < RARRAY_LEN(ary) + 1; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("optstring: %s\n", optstring);
  for (i = 0; i < RARRAY_LEN(longopts_m) + 1; i++) {
    printf("longopts[%d].name: %s\n", i, (longopts[i].name)?(longopts[i].name):"(null)");
    printf("longopts[%d].has_arg: %d\n", i, longopts[i].has_arg);
    printf("longopts[%d].flag: %p\n", i, longopts[i].flag);
    printf("longopts[%d].val: %d\n", i, longopts[i].val);
  }
#endif

  while (1) {
    mrb_value key, val;
    char buf[] = ".";
    int c = getopt_long(argc, argv, optstring, longopts, &longindex);

    if (c == EOF) {
      break;
    }

#ifdef DEBUG
    printf("c: %c, longindex: %d\n", c, longindex);
#endif

    if (!c) { /* long option */
      key = mrb_str_new_cstr(mrb, longopts[longindex].name);
    } else {  /* short option */
      buf[0] = (char)c;
      key = mrb_str_new_cstr(mrb, buf);
    }
    val = mrb_str_new_cstr(mrb, optarg);
    mrb_hash_set(mrb, opt_hash, key, val);
  }

  free(longopts);
  free(argv);

  return opt_hash;
}

void
mrb_mruby_getopts_gem_init(mrb_state *mrb)
{
  struct RClass *a = mrb_define_module(mrb, "Getopts");

  mrb_define_method(mrb, a, "getopt_long", mrb_getopt_long, MRB_ARGS_REQ(2));
}

void
mrb_mruby_getopts_gem_final(mrb_state *mrb)
{
}
