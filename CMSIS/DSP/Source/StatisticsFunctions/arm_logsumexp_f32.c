/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_logsumexp_f32.c
 * Description:  LogSumExp
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2019 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arm_math.h"
#include <limits.h>
#include <math.h>


/**
 * @addtogroup groupStats
 * @{
 */


/**
 * @brief Computation of the LogSumExp
 *
 * In probabilistic computations, the dynamic of the probability values can be very
 * wide because they come from gaussian functions.
 * To avoid underflow and overflow issues, the values are represented by their log.
 * In this representation, multiplying the original exp values is easy : their log are added.
 * But adding the original exp values is requiring some special handling and it is the
 * goal of the LogSumExp function.
 *
 * If the values are x1...xn, the function is computing:
 *
 * ln(exp(x1) + ... + exp(xn)) and the computation is done in such a way that
 * rounding issues are minimised.
 *
 * The max xm of the values if extracted and the function is computing:
 * xm + ln(exp(x1 - xm) + ... + exp(xn - xm))
 *
 * @param[in]    *in         points to an array of input values.
 * @param[in]  blockSize     number of samples in the input array.
 * @return LogSumExp
 *
 */

#if defined(ARM_MATH_NEON)

#include "NEMath.h"
float32_t arm_logsumexp_f32(const float32_t *in, uint32_t blockSize)
{
    float32_t maxVal;
    float32_t tmp;
    float32x4_t tmpV, tmpVb;
    float32x2_t tmpV2;
    float32x4_t maxValV;
    uint32x4_t idxV;
    float32x4_t accumV;
    float32x2_t accumV2;

    const float32_t *pIn;
    uint32_t blkCnt;
    float32_t accum;
 
    pIn = in;

    blkCnt = blockSize;

    if (blockSize <= 3)
    {
      maxVal = *pIn++;
      blkCnt--;

      while(blkCnt > 0)
      {
         tmp = *pIn++;
  
         if (tmp > maxVal)
         {
            maxVal = tmp;
         }
         blkCnt--;
      }
    }
    else
    {
      maxValV = vld1q_f32(pIn);
      pIn += 4;
      blkCnt = (blockSize - 4) >> 2;

      while(blkCnt > 0)
      {
         tmpVb = vld1q_f32(pIn);
         pIn += 4;
  
         idxV = vcgtq_f32(tmpVb, maxValV);
         maxValV = vbslq_f32(idxV, tmpVb, maxValV );

         blkCnt--;
      }

      accumV2 = vpmax_f32(vget_low_f32(maxValV),vget_high_f32(maxValV));
      accumV2 = vpmax_f32(accumV2,accumV2);
      maxVal = accumV2[0];

      blkCnt = (blockSize - 4) & 3;

      while(blkCnt > 0)
      {
         tmp = *pIn++;
  
         if (tmp > maxVal)
         {
            maxVal = tmp;
         }
         blkCnt--;
      }

    }

    

    maxValV = vdupq_n_f32(maxVal);
    pIn = in;
    accum = 0;
    accumV = vdupq_n_f32(0.0);

    blkCnt = blockSize >> 2;

    while(blkCnt > 0)
    {
       tmpV = vld1q_f32(pIn);
       pIn += 4;
       tmpV = vsubq_f32(tmpV, maxValV);
       tmpV = vexpq_f32(tmpV);
       accumV = vaddq_f32(accumV, tmpV);

       blkCnt--;
    
    }
    accumV2 = vpadd_f32(vget_low_f32(accumV),vget_high_f32(accumV));
    accum = accumV2[0] + accumV2[1];

    blkCnt = blockSize & 0x3;
    while(blkCnt > 0)
    {
       tmp = *pIn++;
       accum += exp(tmp - maxVal);
       blkCnt--;
    
    }

    accum = maxVal + log(accum);

    return(accum);
}
#else
float32_t arm_logsumexp_f32(const float32_t *in, uint32_t blockSize)
{
    float32_t maxVal;
    float32_t tmp;
    const float32_t *pIn;
    uint32_t blkCnt;
    float32_t accum;
 
    pIn = in;
    blkCnt = blockSize;

    maxVal = *pIn++;
    blkCnt--;

    while(blkCnt > 0)
    {
       tmp = *pIn++;

       if (tmp > maxVal)
       {
          maxVal = tmp;
       }
       blkCnt--;
    
    }

    blkCnt = blockSize;
    pIn = in;
    accum = 0;
    while(blkCnt > 0)
    {
       tmp = *pIn++;
       accum += exp(tmp - maxVal);
       blkCnt--;
    
    }
    accum = maxVal + log(accum);

    return(accum);
}
#endif
/**
 * @} end of groupStats group
 */
