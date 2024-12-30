#include "Ancillary.h"
#include "Atomic.h"


namespace FFmpeg {
    void AncillaryRelease(ancillary* ancillary) {
        if (AtomicRcDec(&ancillary->rc) == false)
            return;

        if (ancillary->free_cb != NULL)
            ancillary->free_cb(ancillary->data);

        free(ancillary);
    };

    void AncillaryArrayClear(ancillary*** array) {
        if (*array == NULL)
            return;

        for (struct ancillary** ancillary = *array;
            *ancillary != NULL; ancillary++)
            AncillaryRelease(*ancillary);

        free(*array);
        *array = NULL;
    }

    void AncillaryArrayInit(ancillary*** array) {
        *array = NULL;
    }
} // namespace FFmpeg