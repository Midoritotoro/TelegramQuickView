#pragma once 

[[nodiscard]] void* tfind(
    const void* vkey,
    void* const* vrootp,
    int (*compar)(const void*,
        const void*));

[[nodiscard]] void* tsearch(
    const void* vkey,
    void** vrootp,
    int (*compar)(const void*,
        const void*));
