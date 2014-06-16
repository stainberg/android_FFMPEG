/*
 * ARM NEON optimised DSP functions
 * Copyright (c) 2008 Mans Rullgard <mans@mansr.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>

#include "libavcodec/avcodec.h"
#include "libavcodec/dsputil.h"
#include "dsputil_arm.h"

void ff_simple_idct_neon(DCTELEM *data);
void ff_simple_idct_put_neon(uint8_t *dest, int line_size, DCTELEM *data);
void ff_simple_idct_add_neon(uint8_t *dest, int line_size, DCTELEM *data);

void ff_vp3_idct_neon(DCTELEM *data);
void ff_vp3_idct_put_neon(uint8_t *dest, int line_size, DCTELEM *data);
void ff_vp3_idct_add_neon(uint8_t *dest, int line_size, DCTELEM *data);
void ff_vp3_idct_dc_add_neon(uint8_t *dest, int line_size, const DCTELEM *data);

void ff_clear_block_neon(DCTELEM *block);
void ff_clear_blocks_neon(DCTELEM *blocks);

void ff_put_pixels16_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_x2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_y2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_xy2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_x2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_y2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_xy2_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_x2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_y2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels16_xy2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_x2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_y2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_put_pixels8_xy2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);

void ff_avg_pixels16_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_x2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_y2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_xy2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_x2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_y2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_xy2_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_x2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_y2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels16_xy2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_x2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_y2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);
void ff_avg_pixels8_xy2_no_rnd_neon(uint8_t *, const uint8_t *, int, int);

void ff_add_pixels_clamped_neon(const DCTELEM *, uint8_t *, int);
void ff_put_pixels_clamped_neon(const DCTELEM *, uint8_t *, int);
void ff_put_signed_pixels_clamped_neon(const DCTELEM *, uint8_t *, int);

void ff_put_h264_qpel16_mc00_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc10_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc20_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc30_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc01_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc11_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc21_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc31_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc02_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc12_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc22_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc32_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc03_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc13_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc23_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel16_mc33_neon(uint8_t *, uint8_t *, int);

void ff_put_h264_qpel8_mc00_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc10_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc20_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc30_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc01_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc11_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc21_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc31_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc02_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc12_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc22_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc32_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc03_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc13_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc23_neon(uint8_t *, uint8_t *, int);
void ff_put_h264_qpel8_mc33_neon(uint8_t *, uint8_t *, int);

void ff_avg_h264_qpel16_mc00_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc10_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc20_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc30_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc01_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc11_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc21_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc31_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc02_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc12_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc22_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc32_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc03_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc13_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc23_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel16_mc33_neon(uint8_t *, uint8_t *, int);

void ff_avg_h264_qpel8_mc00_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc10_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc20_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc30_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc01_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc11_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc21_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc31_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc02_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc12_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc22_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc32_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc03_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc13_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc23_neon(uint8_t *, uint8_t *, int);
void ff_avg_h264_qpel8_mc33_neon(uint8_t *, uint8_t *, int);

void ff_put_h264_chroma_mc8_neon(uint8_t *, uint8_t *, int, int, int, int);
void ff_put_h264_chroma_mc4_neon(uint8_t *, uint8_t *, int, int, int, int);
void ff_put_h264_chroma_mc2_neon(uint8_t *, uint8_t *, int, int, int, int);

void ff_avg_h264_chroma_mc8_neon(uint8_t *, uint8_t *, int, int, int, int);
void ff_avg_h264_chroma_mc4_neon(uint8_t *, uint8_t *, int, int, int, int);
void ff_avg_h264_chroma_mc2_neon(uint8_t *, uint8_t *, int, int, int, int);

void ff_vp3_v_loop_filter_neon(uint8_t *, int, int *);
void ff_vp3_h_loop_filter_neon(uint8_t *, int, int *);

void ff_vector_fmul_neon(float *dst, const float *src0, const float *src1, int len);
void ff_vector_fmul_window_neon(float *dst, const float *src0,
                                const float *src1, const float *win, int len);
void ff_vector_fmul_scalar_neon(float *dst, const float *src, float mul,
                                int len);
void ff_vector_fmac_scalar_neon(float *dst, const float *src, float mul,
                                int len);
void ff_butterflies_float_neon(float *v1, float *v2, int len);
float ff_scalarproduct_float_neon(const float *v1, const float *v2, int len);
void ff_vector_fmul_reverse_neon(float *dst, const float *src0,
                                 const float *src1, int len);
void ff_vector_fmul_add_neon(float *dst, const float *src0, const float *src1,
                             const float *src2, int len);

void ff_vector_clipf_neon(float *dst, const float *src, float min, float max,
                          int len);
void ff_vector_clip_int32_neon(int32_t *dst, const int32_t *src, int32_t min,
                               int32_t max, unsigned int len);

void ff_vorbis_inverse_coupling_neon(float *mag, float *ang, int blocksize);

int32_t ff_scalarproduct_int16_neon(const int16_t *v1, const int16_t *v2, int len);
int32_t ff_scalarproduct_and_madd_int16_neon(int16_t *v1, const int16_t *v2,
                                             const int16_t *v3, int len, int mul);

void ff_apply_window_int16_neon(int16_t *dst, const int16_t *src,
                                const int16_t *window, unsigned n);

#if HAVE_NEON // modified by xjm for this will crash as HAVE_NEON condition for AAC_DECODER ( it only conclude by othe place example )
static void vector_fmul_c(float *dst, const float *src0, const float *src1, int len){
    int i;
    for(i=0; i<len; i++)
        dst[i] = src0[i] * src1[i];
}

static void vector_fmul_reverse_c(float *dst, const float *src0, const float *src1, int len){
    int i;
    src1 += len-1;
    for(i=0; i<len; i++)
        dst[i] = src0[i] * src1[-i];
}

static void vector_fmul_add_c(float *dst, const float *src0, const float *src1, const float *src2, int len){
    int i;
    for(i=0; i<len; i++)
        dst[i] = src0[i] * src1[i] + src2[i];
}

static void vector_fmul_window_c(float *dst, const float *src0,
                                 const float *src1, const float *win, int len)
{
    int i,j;
    dst += len;
    win += len;
    src0+= len;
    for(i=-len, j=len-1; i<0; i++, j--) {
        float s0 = src0[i];
        float s1 = src1[j];
        float wi = win[i];
        float wj = win[j];
        dst[i] = s0*wj - s1*wi;
        dst[j] = s0*wi + s1*wj;
    }
}

static void vector_fmul_scalar_c(float *dst, const float *src, float mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src[i] * mul;
}

static void vector_fmac_scalar_c(float *dst, const float *src, float mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] += src[i] * mul;
}

static void butterflies_float_c(float *restrict v1, float *restrict v2,
                                int len)
{
    int i;
    for (i = 0; i < len; i++) {
        float t = v1[i] - v2[i];
        v1[i] += v2[i];
        v2[i] = t;
    }
}

static void butterflies_float_interleave_c(float *dst, const float *src0,
                                           const float *src1, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        float f1 = src0[i];
        float f2 = src1[i];
        dst[2*i    ] = f1 + f2;
        dst[2*i + 1] = f1 - f2;
    }
}

static float scalarproduct_float_c(const float *v1, const float *v2, int len)
{
    float p = 0.0;
    int i;

    for (i = 0; i < len; i++)
        p += v1[i] * v2[i];

    return p;
}

static inline uint32_t clipf_c_one(uint32_t a, uint32_t mini,
                   uint32_t maxi, uint32_t maxisign)
{

    if(a > mini) return mini;
    else if((a^(1U<<31)) > maxisign) return maxi;
    else return a;
}

static void vector_clipf_c_opposite_sign(float *dst, const float *src, float *min, float *max, int len){
    int i;
    uint32_t mini = *(uint32_t*)min;
    uint32_t maxi = *(uint32_t*)max;
    uint32_t maxisign = maxi ^ (1U<<31);
    uint32_t *dsti = (uint32_t*)dst;
    const uint32_t *srci = (const uint32_t*)src;
    for(i=0; i<len; i+=8) {
        dsti[i + 0] = clipf_c_one(srci[i + 0], mini, maxi, maxisign);
        dsti[i + 1] = clipf_c_one(srci[i + 1], mini, maxi, maxisign);
        dsti[i + 2] = clipf_c_one(srci[i + 2], mini, maxi, maxisign);
        dsti[i + 3] = clipf_c_one(srci[i + 3], mini, maxi, maxisign);
        dsti[i + 4] = clipf_c_one(srci[i + 4], mini, maxi, maxisign);
        dsti[i + 5] = clipf_c_one(srci[i + 5], mini, maxi, maxisign);
        dsti[i + 6] = clipf_c_one(srci[i + 6], mini, maxi, maxisign);
        dsti[i + 7] = clipf_c_one(srci[i + 7], mini, maxi, maxisign);
    }
}
static void vector_clipf_c(float *dst, const float *src, float min, float max, int len){
    int i;
    if(min < 0 && max > 0) {
        vector_clipf_c_opposite_sign(dst, src, &min, &max, len);
    } else {
        for(i=0; i < len; i+=8) {
            dst[i    ] = av_clipf(src[i    ], min, max);
            dst[i + 1] = av_clipf(src[i + 1], min, max);
            dst[i + 2] = av_clipf(src[i + 2], min, max);
            dst[i + 3] = av_clipf(src[i + 3], min, max);
            dst[i + 4] = av_clipf(src[i + 4], min, max);
            dst[i + 5] = av_clipf(src[i + 5], min, max);
            dst[i + 6] = av_clipf(src[i + 6], min, max);
            dst[i + 7] = av_clipf(src[i + 7], min, max);
        }
    }
}

static int32_t scalarproduct_int16_c(const int16_t * v1, const int16_t * v2, int order)
{
    int res = 0;

    while (order--)
        res += *v1++ * *v2++;

    return res;
}

static int32_t scalarproduct_and_madd_int16_c(int16_t *v1, const int16_t *v2, const int16_t *v3, int order, int mul)
{
    int res = 0;
    while (order--) {
        res   += *v1 * *v2++;
        *v1++ += mul * *v3++;
    }
    return res;
}

#ifdef WINDOW_INT16
static void apply_window_int16_c(int16_t *output, const int16_t *input,
                                 const int16_t *window, unsigned int len)
{
    int i;
    int len2 = len >> 1;

    for (i = 0; i < len2; i++) {
        int16_t w       = window[i];
        output[i]       = (MUL16(input[i],       w) + (1 << 14)) >> 15;
        output[len-i-1] = (MUL16(input[len-i-1], w) + (1 << 14)) >> 15;
    }
}
#endif
static void vector_clip_int32_c(int32_t *dst, const int32_t *src, int32_t min,
                                int32_t max, unsigned int len)
{
    do {
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        *dst++ = av_clip(*src++, min, max);
        len -= 8;
    } while (len > 0);
}
#endif 
void ff_dsputil_init_neon(DSPContext *c, AVCodecContext *avctx)
{
    const int high_bit_depth = avctx->bits_per_raw_sample > 8;

    if (!avctx->lowres && avctx->bits_per_raw_sample <= 8) {
        if (avctx->idct_algo == FF_IDCT_AUTO ||
            avctx->idct_algo == FF_IDCT_SIMPLENEON) {
            c->idct_put              = ff_simple_idct_put_neon;
            c->idct_add              = ff_simple_idct_add_neon;
            c->idct                  = ff_simple_idct_neon;
            c->idct_permutation_type = FF_PARTTRANS_IDCT_PERM;
        } else if ((CONFIG_VP3_DECODER || CONFIG_VP5_DECODER ||
                    CONFIG_VP6_DECODER) &&
                   avctx->idct_algo == FF_IDCT_VP3) {
            c->idct_put              = ff_vp3_idct_put_neon;
            c->idct_add              = ff_vp3_idct_add_neon;
            c->idct                  = ff_vp3_idct_neon;
            c->idct_permutation_type = FF_TRANSPOSE_IDCT_PERM;
        }
    }

    if (!high_bit_depth) {
        c->clear_block  = ff_clear_block_neon;
        c->clear_blocks = ff_clear_blocks_neon;

        c->put_pixels_tab[0][0] = ff_put_pixels16_neon;
        c->put_pixels_tab[0][1] = ff_put_pixels16_x2_neon;
        c->put_pixels_tab[0][2] = ff_put_pixels16_y2_neon;
        c->put_pixels_tab[0][3] = ff_put_pixels16_xy2_neon;
        c->put_pixels_tab[1][0] = ff_put_pixels8_neon;
        c->put_pixels_tab[1][1] = ff_put_pixels8_x2_neon;
        c->put_pixels_tab[1][2] = ff_put_pixels8_y2_neon;
        c->put_pixels_tab[1][3] = ff_put_pixels8_xy2_neon;

        c->put_no_rnd_pixels_tab[0][0] = ff_put_pixels16_neon;
        c->put_no_rnd_pixels_tab[0][1] = ff_put_pixels16_x2_no_rnd_neon;
        c->put_no_rnd_pixels_tab[0][2] = ff_put_pixels16_y2_no_rnd_neon;
        c->put_no_rnd_pixels_tab[0][3] = ff_put_pixels16_xy2_no_rnd_neon;
        c->put_no_rnd_pixels_tab[1][0] = ff_put_pixels8_neon;
        c->put_no_rnd_pixels_tab[1][1] = ff_put_pixels8_x2_no_rnd_neon;
        c->put_no_rnd_pixels_tab[1][2] = ff_put_pixels8_y2_no_rnd_neon;
        c->put_no_rnd_pixels_tab[1][3] = ff_put_pixels8_xy2_no_rnd_neon;

        c->avg_pixels_tab[0][0] = ff_avg_pixels16_neon;
        c->avg_pixels_tab[0][1] = ff_avg_pixels16_x2_neon;
        c->avg_pixels_tab[0][2] = ff_avg_pixels16_y2_neon;
        c->avg_pixels_tab[0][3] = ff_avg_pixels16_xy2_neon;
        c->avg_pixels_tab[1][0] = ff_avg_pixels8_neon;
        c->avg_pixels_tab[1][1] = ff_avg_pixels8_x2_neon;
        c->avg_pixels_tab[1][2] = ff_avg_pixels8_y2_neon;
        c->avg_pixels_tab[1][3] = ff_avg_pixels8_xy2_neon;

        c->avg_no_rnd_pixels_tab[0][0] = ff_avg_pixels16_neon;
        c->avg_no_rnd_pixels_tab[0][1] = ff_avg_pixels16_x2_no_rnd_neon;
        c->avg_no_rnd_pixels_tab[0][2] = ff_avg_pixels16_y2_no_rnd_neon;
        c->avg_no_rnd_pixels_tab[0][3] = ff_avg_pixels16_xy2_no_rnd_neon;
        c->avg_no_rnd_pixels_tab[1][0] = ff_avg_pixels8_neon;
        c->avg_no_rnd_pixels_tab[1][1] = ff_avg_pixels8_x2_no_rnd_neon;
        c->avg_no_rnd_pixels_tab[1][2] = ff_avg_pixels8_y2_no_rnd_neon;
        c->avg_no_rnd_pixels_tab[1][3] = ff_avg_pixels8_xy2_no_rnd_neon;
    }

    c->add_pixels_clamped = ff_add_pixels_clamped_neon;
    c->put_pixels_clamped = ff_put_pixels_clamped_neon;
    c->put_signed_pixels_clamped = ff_put_signed_pixels_clamped_neon;

    if (CONFIG_H264_DECODER && !high_bit_depth) {
        c->put_h264_chroma_pixels_tab[0] = ff_put_h264_chroma_mc8_neon;
        c->put_h264_chroma_pixels_tab[1] = ff_put_h264_chroma_mc4_neon;
        c->put_h264_chroma_pixels_tab[2] = ff_put_h264_chroma_mc2_neon;

        c->avg_h264_chroma_pixels_tab[0] = ff_avg_h264_chroma_mc8_neon;
        c->avg_h264_chroma_pixels_tab[1] = ff_avg_h264_chroma_mc4_neon;
        c->avg_h264_chroma_pixels_tab[2] = ff_avg_h264_chroma_mc2_neon;

        c->put_h264_qpel_pixels_tab[0][ 0] = ff_put_h264_qpel16_mc00_neon;
        c->put_h264_qpel_pixels_tab[0][ 1] = ff_put_h264_qpel16_mc10_neon;
        c->put_h264_qpel_pixels_tab[0][ 2] = ff_put_h264_qpel16_mc20_neon;
        c->put_h264_qpel_pixels_tab[0][ 3] = ff_put_h264_qpel16_mc30_neon;
        c->put_h264_qpel_pixels_tab[0][ 4] = ff_put_h264_qpel16_mc01_neon;
        c->put_h264_qpel_pixels_tab[0][ 5] = ff_put_h264_qpel16_mc11_neon;
        c->put_h264_qpel_pixels_tab[0][ 6] = ff_put_h264_qpel16_mc21_neon;
        c->put_h264_qpel_pixels_tab[0][ 7] = ff_put_h264_qpel16_mc31_neon;
        c->put_h264_qpel_pixels_tab[0][ 8] = ff_put_h264_qpel16_mc02_neon;
        c->put_h264_qpel_pixels_tab[0][ 9] = ff_put_h264_qpel16_mc12_neon;
        c->put_h264_qpel_pixels_tab[0][10] = ff_put_h264_qpel16_mc22_neon;
        c->put_h264_qpel_pixels_tab[0][11] = ff_put_h264_qpel16_mc32_neon;
        c->put_h264_qpel_pixels_tab[0][12] = ff_put_h264_qpel16_mc03_neon;
        c->put_h264_qpel_pixels_tab[0][13] = ff_put_h264_qpel16_mc13_neon;
        c->put_h264_qpel_pixels_tab[0][14] = ff_put_h264_qpel16_mc23_neon;
        c->put_h264_qpel_pixels_tab[0][15] = ff_put_h264_qpel16_mc33_neon;

        c->put_h264_qpel_pixels_tab[1][ 0] = ff_put_h264_qpel8_mc00_neon;
        c->put_h264_qpel_pixels_tab[1][ 1] = ff_put_h264_qpel8_mc10_neon;
        c->put_h264_qpel_pixels_tab[1][ 2] = ff_put_h264_qpel8_mc20_neon;
        c->put_h264_qpel_pixels_tab[1][ 3] = ff_put_h264_qpel8_mc30_neon;
        c->put_h264_qpel_pixels_tab[1][ 4] = ff_put_h264_qpel8_mc01_neon;
        c->put_h264_qpel_pixels_tab[1][ 5] = ff_put_h264_qpel8_mc11_neon;
        c->put_h264_qpel_pixels_tab[1][ 6] = ff_put_h264_qpel8_mc21_neon;
        c->put_h264_qpel_pixels_tab[1][ 7] = ff_put_h264_qpel8_mc31_neon;
        c->put_h264_qpel_pixels_tab[1][ 8] = ff_put_h264_qpel8_mc02_neon;
        c->put_h264_qpel_pixels_tab[1][ 9] = ff_put_h264_qpel8_mc12_neon;
        c->put_h264_qpel_pixels_tab[1][10] = ff_put_h264_qpel8_mc22_neon;
        c->put_h264_qpel_pixels_tab[1][11] = ff_put_h264_qpel8_mc32_neon;
        c->put_h264_qpel_pixels_tab[1][12] = ff_put_h264_qpel8_mc03_neon;
        c->put_h264_qpel_pixels_tab[1][13] = ff_put_h264_qpel8_mc13_neon;
        c->put_h264_qpel_pixels_tab[1][14] = ff_put_h264_qpel8_mc23_neon;
        c->put_h264_qpel_pixels_tab[1][15] = ff_put_h264_qpel8_mc33_neon;

        c->avg_h264_qpel_pixels_tab[0][ 0] = ff_avg_h264_qpel16_mc00_neon;
        c->avg_h264_qpel_pixels_tab[0][ 1] = ff_avg_h264_qpel16_mc10_neon;
        c->avg_h264_qpel_pixels_tab[0][ 2] = ff_avg_h264_qpel16_mc20_neon;
        c->avg_h264_qpel_pixels_tab[0][ 3] = ff_avg_h264_qpel16_mc30_neon;
        c->avg_h264_qpel_pixels_tab[0][ 4] = ff_avg_h264_qpel16_mc01_neon;
        c->avg_h264_qpel_pixels_tab[0][ 5] = ff_avg_h264_qpel16_mc11_neon;
        c->avg_h264_qpel_pixels_tab[0][ 6] = ff_avg_h264_qpel16_mc21_neon;
        c->avg_h264_qpel_pixels_tab[0][ 7] = ff_avg_h264_qpel16_mc31_neon;
        c->avg_h264_qpel_pixels_tab[0][ 8] = ff_avg_h264_qpel16_mc02_neon;
        c->avg_h264_qpel_pixels_tab[0][ 9] = ff_avg_h264_qpel16_mc12_neon;
        c->avg_h264_qpel_pixels_tab[0][10] = ff_avg_h264_qpel16_mc22_neon;
        c->avg_h264_qpel_pixels_tab[0][11] = ff_avg_h264_qpel16_mc32_neon;
        c->avg_h264_qpel_pixels_tab[0][12] = ff_avg_h264_qpel16_mc03_neon;
        c->avg_h264_qpel_pixels_tab[0][13] = ff_avg_h264_qpel16_mc13_neon;
        c->avg_h264_qpel_pixels_tab[0][14] = ff_avg_h264_qpel16_mc23_neon;
        c->avg_h264_qpel_pixels_tab[0][15] = ff_avg_h264_qpel16_mc33_neon;

        c->avg_h264_qpel_pixels_tab[1][ 0] = ff_avg_h264_qpel8_mc00_neon;
        c->avg_h264_qpel_pixels_tab[1][ 1] = ff_avg_h264_qpel8_mc10_neon;
        c->avg_h264_qpel_pixels_tab[1][ 2] = ff_avg_h264_qpel8_mc20_neon;
        c->avg_h264_qpel_pixels_tab[1][ 3] = ff_avg_h264_qpel8_mc30_neon;
        c->avg_h264_qpel_pixels_tab[1][ 4] = ff_avg_h264_qpel8_mc01_neon;
        c->avg_h264_qpel_pixels_tab[1][ 5] = ff_avg_h264_qpel8_mc11_neon;
        c->avg_h264_qpel_pixels_tab[1][ 6] = ff_avg_h264_qpel8_mc21_neon;
        c->avg_h264_qpel_pixels_tab[1][ 7] = ff_avg_h264_qpel8_mc31_neon;
        c->avg_h264_qpel_pixels_tab[1][ 8] = ff_avg_h264_qpel8_mc02_neon;
        c->avg_h264_qpel_pixels_tab[1][ 9] = ff_avg_h264_qpel8_mc12_neon;
        c->avg_h264_qpel_pixels_tab[1][10] = ff_avg_h264_qpel8_mc22_neon;
        c->avg_h264_qpel_pixels_tab[1][11] = ff_avg_h264_qpel8_mc32_neon;
        c->avg_h264_qpel_pixels_tab[1][12] = ff_avg_h264_qpel8_mc03_neon;
        c->avg_h264_qpel_pixels_tab[1][13] = ff_avg_h264_qpel8_mc13_neon;
        c->avg_h264_qpel_pixels_tab[1][14] = ff_avg_h264_qpel8_mc23_neon;
        c->avg_h264_qpel_pixels_tab[1][15] = ff_avg_h264_qpel8_mc33_neon;
    }

    if (CONFIG_VP3_DECODER) {
        c->vp3_v_loop_filter = ff_vp3_v_loop_filter_neon;
        c->vp3_h_loop_filter = ff_vp3_h_loop_filter_neon;
        c->vp3_idct_dc_add   = ff_vp3_idct_dc_add_neon;
    }
#if 0 // modified by xjm for this will crash as HAVE_NEON condition for AAC_DECODER ( it only conclude by othe place example )
    c->vector_fmul                = ff_vector_fmul_neon;
    c->vector_fmul_window         = ff_vector_fmul_window_neon;
    c->vector_fmul_scalar         = ff_vector_fmul_scalar_neon;
    c->vector_fmac_scalar         = ff_vector_fmac_scalar_neon;
    c->butterflies_float          = ff_butterflies_float_neon;
    c->scalarproduct_float        = ff_scalarproduct_float_neon;
    c->vector_fmul_reverse        = ff_vector_fmul_reverse_neon;
    c->vector_fmul_add            = ff_vector_fmul_add_neon;
    c->vector_clipf               = ff_vector_clipf_neon;
    c->vector_clip_int32          = ff_vector_clip_int32_neon;
#else
	c->vector_fmul                = vector_fmul_c;
    c->vector_fmul_window         = vector_fmul_window_c;
    c->vector_fmul_scalar         = vector_fmul_scalar_c;
    c->vector_fmac_scalar         = vector_fmac_scalar_c;
    c->butterflies_float          = butterflies_float_c;
    c->scalarproduct_float        = scalarproduct_float_c;
    c->vector_fmul_reverse        = vector_fmul_reverse_c;
    c->vector_fmul_add            = vector_fmul_add_c;
    c->vector_clipf               = vector_clipf_c;
    c->vector_clip_int32          = vector_clip_int32_c;
#endif
    if (CONFIG_VORBIS_DECODER)
        c->vorbis_inverse_coupling = ff_vorbis_inverse_coupling_neon;

    c->scalarproduct_int16 = ff_scalarproduct_int16_neon;
    c->scalarproduct_and_madd_int16 = ff_scalarproduct_and_madd_int16_neon;

    c->apply_window_int16 = ff_apply_window_int16_neon;
}
