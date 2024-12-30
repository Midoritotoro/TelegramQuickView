#pragma once 

#include "Types.h"
#include "FourccHelper.h"

#ifdef WORDS_BIGENDIAN
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
           | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#   define TWOCC( a, b ) \
        ( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#else
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
           | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#   define TWOCC( a, b ) \
        ( (uint16_t)(a) | ( (uint16_t)(b) << 8 ) )

#endif

namespace FFmpeg {
	[[nodiscard]] int FourccCmp(
		const void* key,
		const void* ent);

	[[nodiscard]] fourcc_t Lookup(
		fourcc_t fourcc,
		const char** dsc,
		const struct fourcc_mapping* mapv, size_t mapc,
		const struct fourcc_desc* dscv, size_t dscc);

	[[nodiscard]] fourcc_t LookupVideo(
		fourcc_t fourcc,
		const char** dsc);

	[[nodiscard]] fourcc_t LookupAudio(
		fourcc_t fourcc,
		const char** dsc);
	[[nodiscard]] fourcc_t LookupSpu(
		fourcc_t fourcc,
		const char** dsc);

	[[nodiscard]] fourcc_t LookupCat(
		fourcc_t fourcc,
		const char** dsc,
		int cat);
	[[nodiscard]] fourcc_t FourccGetCodec(int cat, fourcc_t fourcc);

	[[nodiscard]] int64_t GCD(
		int64_t a,
		int64_t b);

	[[nodiscard]] int LCM(int a, int b);

	bool UReduce(
		unsigned* pi_dst_nom, unsigned* pi_dst_den,
		uint64_t i_nom, uint64_t i_den, uint64_t i_max);

    [[nodiscard]] const chroma_description_t* 
        FourccGetChromaDescription(fourcc_t i_fourcc);
} // namespace FFmpeg