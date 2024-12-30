#pragma once 

#include "FourccCodecs.h"
#include "Config.h"

#include "Chroma.h"
#include "Math.h"

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
	using fourcc_t = uint32_t;

	[[nodiscard]] int FourccCmp(
		const void* key,
		const void* ent);

	[[nodiscard]] fourcc_t FourccLookup(
		fourcc_t fourcc,
		const char** dsc,
		const struct fourcc_mapping* mapv, size_t mapc,
		const struct fourcc_desc* dscv, size_t dscc);

	[[nodiscard]] fourcc_t FourccLookupVideo(
		fourcc_t fourcc,
		const char** dsc);

	[[nodiscard]] fourcc_t FourccLookupAudio(
		fourcc_t fourcc,
		const char** dsc);
	[[nodiscard]] fourcc_t FourccLookupSpu(
		fourcc_t fourcc,
		const char** dsc);

	[[nodiscard]] fourcc_t FourccLookupCat(
		fourcc_t fourcc,
		const char** dsc,
		int cat);
	[[nodiscard]] fourcc_t FourccGetCodec(int cat, fourcc_t fourcc);
} // namespace FFmpeg