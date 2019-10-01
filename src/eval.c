#include <plisp/eval.h>
#include <plisp/hashtable.h>
#include <plisp/read.h>
#include <plisp/builtin.h>
#include <plisp/print.h>

#include <stdio.h>

plisp_t *toplevel = NULL;

void plisp_init_eval(void) {
    toplevel = plisp_make_hashtable(HT_EQ);
}

void plisp_toplevel_def(plisp_t *sym, plisp_t *obj) {
    plisp_hashtable_insert(toplevel, sym, obj);
}

void plisp_def_subr(const char *name, void *fp, short nargs, bool rest) {
    plisp_toplevel_def(make_interned_symbol(name), plisp_make_prim_fn(fp, nargs, rest));
}

typedef plisp_t *(*prim0)();
typedef plisp_t *(*prim1)(plisp_t *);
typedef plisp_t *(*prim2)(plisp_t *,plisp_t *);
typedef plisp_t *(*prim3)(plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim4)(plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim5)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim6)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim7)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim8)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim9)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);
typedef plisp_t *(*prim10)(plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *,plisp_t *);

plisp_t *plisp_call_prim(plisp_t *fn, plisp_t *rest_uneval) {
    struct pl_prim_fn pfn = fn->data.primfn;
    plisp_t *args[PRIM_FN_MAX_ARGS];

    for (int i = 0; i < pfn.nargs; ++i) {
        if (plisp_c_null(rest_uneval)) {
            fprintf(stderr, "error: not enough arguments to function ");
            plisp_write(stderr, fn);
            fprintf(stderr, "\n");
            return NULL;
        }
        args[i] = plisp_eval(plisp_car(rest_uneval));

        if (args[i] == NULL) {
            return NULL;
        }

        rest_uneval = plisp_cdr(rest_uneval);
    }

    //plisp_t *rest = plisp_make_nil();
    if (pfn.rest) {
        //TODO
    } else {
        if (!plisp_c_null(rest_uneval)) {
            fprintf(stderr, "error: too many arguments to function ");
            plisp_write(stderr, fn);
            fprintf(stderr, "\n");
            return NULL;
        }
    }

    // TODO pass rest arguments
    switch (pfn.nargs) {
        case 0:
            return ((prim0) pfn.fp)();
        case 1:
            return ((prim1) pfn.fp)(args[0]);
        case 2:
            return ((prim2) pfn.fp)(args[0],args[1]);
        case 3:
            return ((prim3) pfn.fp)(args[0],args[1],args[2]);
        case 4:
            return ((prim4) pfn.fp)(args[0],args[1],args[2],args[3]);
        case 5:
            return ((prim5) pfn.fp)(args[0],args[1],args[2],args[3],args[4]);
        case 6:
            return ((prim6) pfn.fp)(args[0],args[1],args[2],args[3],args[4],args[5]);
        case 7:
            return ((prim7) pfn.fp)(args[0],args[1],args[2],args[3],args[4],args[5],args[6]);
        case 8:
            return ((prim8) pfn.fp)(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
        case 9:
            return ((prim9) pfn.fp)(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8]);
        case 10:
            return ((prim10) pfn.fp)(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9]);
    }

    return NULL;
}

plisp_t *plisp_eval(plisp_t *form) {
    if (form->tid == TID_CONS) {
        plisp_t* fn = plisp_eval(plisp_car(form));
        if (fn == NULL) {
            return NULL;
        }

        if (fn->tid == TID_PRIM_FN) {
            return plisp_call_prim(fn, plisp_cdr(form));
        }

        fprintf(stderr, "cannot call non-function: ");
        plisp_write(stderr, fn);
        fprintf(stderr, "\n");
        return NULL;
    } else if (form->tid == TID_SYMBOL) {
        plisp_t *ret = plisp_hashtable_find(toplevel, form);
        if (ret == NULL) {
            fprintf(stderr, "variable '%s' not found\n", form->data.string.base);
        }

        return ret;
    }

    return form;
}
