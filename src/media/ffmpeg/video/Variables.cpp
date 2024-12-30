#include "Variables.h"


namespace FFmpeg {
    int CmpBool(value_t v, value_t w)
    {
        return v.b_bool ? w.b_bool ? 0 : 1 : w.b_bool ? -1 : 0;
    }

    int CmpInt(value_t v, value_t w)
    {
        return v.i_int == w.i_int ? 0 : v.i_int > w.i_int ? 1 : -1;
    }

    int CmpString(value_t v, value_t w)
    {
        if (!v.psz_string)
            return !w.psz_string ? 0 : -1;
        else
            return !w.psz_string ? 1 : strcmp(v.psz_string, w.psz_string);
    }

    int CmpFloat(value_t v, value_t w) {
        return v.f_float == w.f_float ? 0 : v.f_float > w.f_float ? 1 : -1; 
    }

    int CmpAddress(value_t v, value_t w) { 
        return v.p_address == w.p_address ? 0 : v.p_address > w.p_address ? 1 : -1;
    }

    void DupDummy(value_t* p_val) { 
        (void)p_val; /* unused */ 
    }

    void DupString(value_t* p_val) {
        p_val->psz_string = strdup(p_val->psz_string ? p_val->psz_string : "");
    }

    void FreeDummy(value_t* p_val) {
        (void)p_val; /* unused */
    }

    void FreeString(value_t* p_val) {
        free(p_val->psz_string);
    }

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
        return (pp_var != NULL) ? (variable_t*)*pp_var : NULL;
    }

    param* param_Find(const char* name)
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
} // namespace FFmpeg