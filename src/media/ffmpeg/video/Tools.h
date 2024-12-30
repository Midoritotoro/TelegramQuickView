#pragma once 

#include <cstdlib>
#include <cstdint>

#include <limits.h>
#include "Types.h"

#include <search.h>
#include <winsock.h>

#include "Threads.h"
#include "TFunctions.h"

#include <stdatomic.h>

#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))

namespace FFmpeg {
    static int CmpBool(value_t v, value_t w)
    {
        return v.b_bool ? w.b_bool ? 0 : 1 : w.b_bool ? -1 : 0;
    }

    static int CmpInt(value_t v, value_t w)
    {
        return v.i_int == w.i_int ? 0 : v.i_int > w.i_int ? 1 : -1;
    }

    static int CmpString(value_t v, value_t w)
    {
        if (!v.psz_string)
            return !w.psz_string ? 0 : -1;
        else
            return !w.psz_string ? 1 : strcmp(v.psz_string, w.psz_string);
    }

    static int CmpFloat(value_t v, value_t w) { return v.f_float == w.f_float ? 0 : v.f_float > w.f_float ? 1 : -1; }
    static int CmpAddress(value_t v, value_t w) { return v.p_address == w.p_address ? 0 : v.p_address > w.p_address ? 1 : -1; }

    static void DupDummy(value_t* p_val) { (void)p_val; /* unused */ }
    static void DupString(value_t* p_val)
    {
        p_val->psz_string = strdup(p_val->psz_string ? p_val->psz_string : "");
    }

    static void FreeDummy(value_t* p_val) { (void)p_val; /* unused */ }
    static void FreeString(value_t* p_val) { free(p_val->psz_string); }

    static const struct variable_ops_t
        void_ops = { NULL,       DupDummy,  FreeDummy, },
        addr_ops = { CmpAddress, DupDummy,  FreeDummy, },
        bool_ops = { CmpBool,    DupDummy,  FreeDummy, },
        float_ops = { CmpFloat,   DupDummy,  FreeDummy, },
        int_ops = { CmpInt,     DupDummy,  FreeDummy, },
        string_ops = { CmpString,  DupString, FreeString, },
        coords_ops = { NULL,       DupDummy,  FreeDummy, };

    struct node_t {
        char* key;
        struct node_t* llink, * rlink;
    };


    int varcmp(const void* a, const void* b)
    {
        const variable_t* va = (variable_t*)a, * vb = (variable_t*)b;

        /* psz_name must be first */
        assert(va == (const void*)&va->psz_name);
        return strcmp(va->psz_name, vb->psz_name);
    }

    variable_t* LookupVar(object_t* obj, const char* psz_name)
    {
        object_internals_t* priv = objectPrivate(obj);
        void** pp_var;

        mutex_lock(&priv->var_lock);
        pp_var = (void**)tfind(&psz_name, &priv->var_root, varcmp);
        return (pp_var != NULL) ? (variable_t *)*pp_var : NULL;
    }

    static struct
    {
        struct param** list;
        size_t count;
    } config = { NULL, 0 };


    struct param* param_Find(const char* name)
    {
        param* const* p;

        assert(name != NULL);
        p = (param* const*)bsearch(name, config.list, config.count, sizeof(*p), confnamecmp);
        return (p != NULL) ? *p : NULL;
    }

    variable_t* LookupVariable(object_t* obj, const char* psz_name)
    {
        object_internals_t* priv = objectPrivate(obj);
        void** pp_var;

        mutex_lock(&priv->var_lock);
        pp_var = (void**)tfind(&psz_name, &priv->var_root, varcmp);
        return (pp_var != NULL) ? (variable_t*)*pp_var : NULL;
    }

    int var_GetChecked(object_t* p_this, const char* psz_name,
        int expected_type, value_t* p_val)
    {
        assert(p_this);

        object_internals_t* p_priv = objectPrivate(p_this);
        variable_t* p_var;
        int err = SUCCESS;

        p_var = LookupVariable(p_this, psz_name);
        if (p_var != NULL)
        {
            assert(expected_type == 0 ||
                (p_var->i_type & VAR_CLASS) == expected_type);
            assert((p_var->i_type & VAR_CLASS) != VAR_VOID);

            /* Really get the variable */
            *p_val = p_var->val;

            /* Duplicate value if needed */
            p_var->ops->pf_dup(p_val);
        }
        else
            err = ENOENT;

        mutex_unlock(&p_priv->var_lock);
        return err;
    }

    static int confcmp(const void* a, const void* b)
    {
        const param* const* ca = (const param* const*)a, * const* cb = (const param* const*)b;

        return strcmp((*ca)->item.psz_name, (*cb)->item.psz_name);
    }

    static int confnamecmp(const void* key, const void* elem)
    {
        const struct param* const* conf = (const struct param* const*)elem;

        return strcmp((const char*)key, (*conf)->item.psz_name);
    }

    int64_t config_GetInt(const char* name)
    {
        const struct param* param = param_Find(name);

        /* sanity checks */
        assert(param != NULL);
        assert(IsConfigIntegerType(param->item.i_type));

        return atomic_load_explicit(&param->value.i, std::memory_order_relaxed);
    }

    float config_GetFloat(const char* name)
    {
        const param* _param = (const param*)param_Find(name);

        /* sanity checks */
        assert(_param != NULL);
        assert(IsConfigFloatType(_param->item.i_type));

        return atomic_load_explicit(&_param->value.f, memory_order_relaxed);
    }

    static thread_local struct rcu_thread current;

    bool rcu_read_held(void)
    {
        const struct rcu_thread* const self = &current;

        return self->recursion > 0;
    }

    static std::atomic<rcu_generation*> generation;


    void rcu_read_lock(void)
    {
        rcu_thread* const self = &current;
        rcu_generation* gen;

        if (self->recursion++ > 0)
            return; /* recursion: nothing to do */

        assert(self->generation == NULL);
        gen = atomic_load_explicit(&generation, std::memory_order_acquire);
        self->generation = gen;
        atomic_fetch_add_explicit(&gen->readers, 1, std::memory_order_relaxed);
    }

    void rcu_read_unlock(void)
    {
        rcu_thread* const self = &current;
        rcu_generation* gen;

        assert(rcu_read_held());

        if (--self->recursion > 0)
            return; /* recursion: nothing to do */

        gen = self->generation;
        self->generation = NULL;

        uintptr_t readers = atomic_fetch_sub_explicit(&gen->readers, 1,
            std::memory_order_relaxed);
        if (readers == 0)
            assert("unreachable!", unreachable());
        if (readers > 1)
            return; /* Other reader threads remain: nothing to do */

        if (unlikely(atomic_exchange_explicit(&gen->writer, 0,
            std::memory_order_release)))
            atomic_notify_one(&gen->writer); /* Last reader wakes writer up */
    }

    char* config_GetPsz(const char* name)
    {
        const param* _param = param_Find(name);
        char* str;

        /* sanity checks */
        assert(_param != NULL);
        assert(IsConfigStringType(_param->item.i_type));

        /* return a copy of the string */
        rcu_read_lock();
        str = atomic_load_explicit(&_param->value.str, memory_order_acquire);
        if (str != NULL)
            str = strdup(str);
        rcu_read_unlock();
        return str;
    }


    int var_Inherit(object_t* p_this, const char* psz_name, int i_type,
        value_t* p_val)
    {
        i_type &= VAR_CLASS;
        for (object_t* obj = p_this; obj != NULL; obj = object_parent(obj))
        {
            if (var_GetChecked(obj, psz_name, i_type, p_val) == SUCCESS)
                return SUCCESS;
        }

        /* else take value from config */
        switch (i_type & VAR_CLASS)
        {
        case VAR_STRING:
            p_val->psz_string = config_GetPsz(psz_name);
            break;
        case VAR_FLOAT:
            p_val->f_float = config_GetFloat(psz_name);
            break;
        case VAR_INTEGER:
            p_val->i_int = config_GetInt(psz_name);
            break;
        case VAR_BOOL:
            p_val->b_bool = config_GetInt(psz_name) > 0;
            break;
        default:
            assert("unreachable!", unreachable());
        case VAR_ADDRESS:
            return ENOENT;
        }
        return SUCCESS;
    }

    void Destroy(variable_t* p_var)
    {
        p_var->ops->pf_free(&p_var->val);

        for (size_t i = 0, count = p_var->choices_count; i < count; i++)
        {
            p_var->ops->pf_free(&p_var->choices[i]);
            free(p_var->choices_text[i]);
        }
        free(p_var->choices);
        free(p_var->choices_text);

        free(p_var->psz_name);
        free(p_var->psz_text);
        while (unlikely(p_var->value_callbacks != NULL))
        {
            callback_entry_t* next = p_var->value_callbacks->next;

            free(p_var->value_callbacks);
            p_var->value_callbacks = next;
        }
        assert(p_var->list_callbacks == NULL);
        free(p_var);
    }

    int (var_Create)(object_t* p_this, const char* psz_name, int i_type)
    {
        assert(p_this);

        variable_t* p_var = (variable_t*)calloc(1, sizeof(*p_var));
        if (p_var == NULL)
            return ENOMEM;

        p_var->psz_name = strdup(psz_name);
        p_var->psz_text = NULL;

        p_var->i_type = i_type & ~VAR_DOINHERIT;

        p_var->i_usage = 1;

        p_var->choices_count = 0;
        p_var->choices = NULL;
        p_var->choices_text = NULL;

        p_var->b_incallback = false;
        p_var->value_callbacks = NULL;

        /* Always initialize the variable, even if it is a list variable; this
         * will lead to errors if the variable is not initialized, but it will
         * not cause crashes in the variable handling. */
        switch (i_type & VAR_CLASS)
        {
        case VAR_BOOL:
            p_var->ops = &bool_ops;
            p_var->val.b_bool = false;
            break;
        case VAR_INTEGER:
            p_var->ops = &int_ops;
            p_var->val.i_int = 0;
            p_var->min.i_int = INT64_MIN;
            p_var->max.i_int = INT64_MAX;
            break;
        case VAR_STRING:
            p_var->ops = &string_ops;
            p_var->val.psz_string = NULL;
            break;
        case VAR_FLOAT:
            p_var->ops = &float_ops;
            p_var->val.f_float = 0.f;
            p_var->min.f_float = -FLT_MAX;
            p_var->max.f_float = FLT_MAX;
            break;
        case VAR_COORDS:
            p_var->ops = &coords_ops;
            p_var->val.coords.x = p_var->val.coords.y = 0;
            break;
        case VAR_ADDRESS:
            p_var->ops = &addr_ops;
            p_var->val.p_address = NULL;
            break;
        case VAR_VOID:
            p_var->ops = &void_ops;
            break;
        default:
            assert("unreachable!", unreachable());
        }

        cond_init(&p_var->wait);

        if (i_type & VAR_DOINHERIT)
            var_Inherit(p_this, psz_name, i_type, &p_var->val);

        object_internals_t* p_priv = objectPrivate(p_this);
        void** pp_var;
        variable_t* p_oldvar;
        int ret = SUCCESS;

        mutex_lock(&p_priv->var_lock);

        pp_var = (void**)tsearch(p_var, &p_priv->var_root, varcmp);
        if (unlikely(pp_var == NULL))
            ret = ENOMEM;
        else if ((p_oldvar = ((variable_t*)(*pp_var))) == p_var) /* Variable create */
            p_var = NULL; /* Variable created */
        else /* Variable already exists */
        {
            assert(((i_type ^ p_oldvar->i_type) & VAR_CLASS) == 0);
            p_oldvar->i_usage++;
            p_oldvar->i_type |= i_type & VAR_ISCOMMAND;
        }
        mutex_unlock(&p_priv->var_lock);

        /* If we did not need to create a new variable, free everything... */
        if (p_var != NULL)
            Destroy(p_var);
        return ret;
    }

    int64_t var_GetInteger(object_t* p_obj, const char* psz_name)
    {
        value_t val;
        if (!var_GetChecked(p_obj, psz_name, VAR_INTEGER, &val))
            return val.i_int;
        else
            return 0;
    }

    int64_t var_CreateGetInteger(object_t* p_obj, const char* psz_name)
    {
        var_Create(p_obj, psz_name, VAR_INTEGER | VAR_DOINHERIT);
        return var_GetInteger(p_obj, psz_name);
    }
} // namespace FFmpeg
