#include "Ancillary.h"
#include "Atomic.h"


namespace FFmpeg {
    void AncillaryRelease(struct ancillary* ancillary)
    {
        if (AtomicRcDec(&ancillary->rc))
        {
            if (ancillary->free_cb != NULL)
                ancillary->free_cb(ancillary->data);
            free(ancillary);
        }
    };

    void AncillaryArrayClear(struct ancillary*** array)
    {
        if (*array != NULL)
        {
            for (struct ancillary** ancillary = *array;
                *ancillary != NULL; ancillary++)
            {
                AncillaryRelease(*ancillary);
            }

            free(*array);
            *array = NULL;
        }
    }
} // namespace FFmpeg