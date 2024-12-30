#include "Object.h"


namespace FFmpeg {
    res** obj_res(object_t* obj)
    {
        return &internals(obj)->resources;
    }

    void* objres_new(size_t size, void (*release)(void*))
    {
        if (unlikely(ckd_add(&size, sizeof(struct res), size)))
        {
            errno = ENOMEM;
            return NULL;
        }

        struct res* res = malloc(size);
        if (unlikely(res == NULL))
            return NULL;

        res->release = release;
        return res->payload;
    }

    void objres_push(object_t* obj, void* data)
    {
        struct res** restrict pp = obj_res(obj);
        struct res* res = container_of(data, struct res, payload);

        res->prev = *pp;
        *pp = res;
    }

    static void* objres_pop(object_t* obj)
    {
        struct res** restrict pp = obj_res(obj);
        struct res* res = *pp;

        if (res == NULL)
            return NULL;
        *pp = res->prev;
        return res->payload;
    }

    void objres_clear(object_t* obj)
    {
        void* data;

        while ((data = objres_pop(obj)) != NULL)
        {
            struct res* res = container_of(data, struct res, payload);

            res->release(res->payload);
            free(res);
        }
    }

    void objres_remove(object_t* obj, void* data,
        bool (*match)(void*, void*))
    {
        struct res** restrict pp = obj_res(obj);

        /* With a doubly-linked list, this function could have constant complexity.
         * But that would require one more pointer per resource.
         *
         * Any given list should contain a fairly small number of resources,
         * and in most cases, the resources are destroyed implicitly by
         * objres_clear().
         */
        for (;;)
        {
            struct res* res = *pp;

            assert(res != NULL); /* invalid free? */

            if (match(res->payload, data))
            {
                *pp = res->prev;
                res->release(res->payload);
                free(res);
                return;
            }

            pp = &res->prev;
        }
    }

    void dummy_release(void* data)
    {
        (void)data;
    }

    bool ptrcmp(void* a, void* b)
    {
        return a == b;
    }

    void* (obj_malloc)(object_t* obj, size_t size)
    {
        void* ptr = objres_new(size, dummy_release);
        if (likely(ptr != NULL))
            objres_push(obj, ptr);
        return ptr;
    }

    void* (obj_calloc)(object_t* obj, size_t nmemb, size_t size)
    {
        size_t tabsize;
        if (unlikely(ckd_mul(&tabsize, nmemb, size)))
        {
            errno = ENOMEM;
            return NULL;
        }

        void* ptr = obj_malloc(obj, tabsize);
        if (likely(ptr != NULL))
            memset(ptr, 0, tabsize);
        return ptr;
    }

    void* obj_memdup(object_t* obj, const void* base, size_t len)
    {
        void* ptr = obj_malloc(obj, len);
        if (likely(ptr != NULL))
            memcpy(ptr, base, len);
        return ptr;
    }

    char* (obj_strdup)(object_t* obj, const char* str)
    {
        return obj_memdup(obj, str, strlen(str) + 1);
    }

    void (obj_free)(object_t* obj, void* ptr)
    {
        objres_remove(obj, ptr, ptrcmp);
    }
} // namespace FFmpeg