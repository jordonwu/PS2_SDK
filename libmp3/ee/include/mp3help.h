#ifdef __cplusplus
extern "C" {
#endif

#include "mad.h"

struct audio_dither;
#define MadErrorString(x) mad_stream_errorstr(x)
inline unsigned long prng(unsigned long state);
inline short audio_linear_dither(unsigned int bits, mad_fixed_t sample,	struct audio_dither *dither);
signed short MadFixedToSshort(mad_fixed_t Fixed);


#ifdef __cplusplus
}
#endif
