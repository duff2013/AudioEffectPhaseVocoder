#include "Arduino.h"
//--------------------------------------------------------------
void split_rfft_f32 (
                                   float * pSrc,
                                   uint32_t fftLen,
                                   const float * pATable,
                                   const float * pBTable,
                                   float * pDst)
{
    uint32_t i;                                                   /* Loop Counter */
    float outR, outI;                                             /* Temporary variables for output */
    const float *pCoefA, *pCoefB;                                 /* Temporary pointers for twiddle factors */
    float CoefA1, CoefA2, CoefB1;                                 /* Temporary variables for twiddle coefficients */
    float *pDst1 = &pDst[2], *pDst2 = &pDst[(4u * fftLen) - 1u];  /* temp pointers for output buffer */
    float *pSrc1 = &pSrc[2], *pSrc2 = &pSrc[(2u * fftLen) - 1u];  /* temp pointers for input buffer */
    
    /* Init coefficient pointers */
    pCoefA = &pATable[2u];
    pCoefB = &pBTable[2u];
    
    i = fftLen - 1u;
    
    while (i > 0u) {
        /*
         outR = (pSrc[2 * i] * pATable[2 * i] - pSrc[2 * i + 1] * pATable[2 * i + 1]
         + pSrc[2 * n - 2 * i] * pBTable[2 * i] +
         pSrc[2 * n - 2 * i + 1] * pBTable[2 * i + 1]);
         */
        
        /* outI = (pIn[2 * i + 1] * pATable[2 * i] + pIn[2 * i] * pATable[2 * i + 1] +
         pIn[2 * n - 2 * i] * pBTable[2 * i + 1] -
         pIn[2 * n - 2 * i + 1] * pBTable[2 * i]); */
        
        /* read pATable[2 * i] */
        CoefA1 = *pCoefA++;
        /* pATable[2 * i + 1] */
        CoefA2 = *pCoefA;
        
        /* pSrc[2 * i] * pATable[2 * i] */
        outR = *pSrc1 * CoefA1;
        /* pSrc[2 * i] * CoefA2 */
        outI = *pSrc1++ * CoefA2;
        
        /* (pSrc[2 * i + 1] + pSrc[2 * fftLen - 2 * i + 1]) * CoefA2 */
        outR -= (*pSrc1 + *pSrc2) * CoefA2;
        /* pSrc[2 * i + 1] * CoefA1 */
        outI += *pSrc1++ * CoefA1;
        
        CoefB1 = *pCoefB;
        
        /* pSrc[2 * fftLen - 2 * i + 1] * CoefB1 */
        outI -= *pSrc2-- * CoefB1;
        /* pSrc[2 * fftLen - 2 * i] * CoefA2 */
        outI -= *pSrc2 * CoefA2;
        
        /* pSrc[2 * fftLen - 2 * i] * CoefB1 */
        outR += *pSrc2-- * CoefB1;
        
        /* write output */
        *pDst1++ = outR / 2;
        *pDst1++ = outI / 2;
        
        /* write complex conjugate output */
        *pDst2-- = -outI / 2;
        *pDst2-- = outR / 2;
        
        /* update coefficient pointer */
        pCoefB = pCoefB + (2u);
        pCoefA = pCoefA + (2u - 1u);
        
        i--;
        
    }
    
    pDst[2u * fftLen] = pSrc[0] - pSrc[1];
    pDst[(2u * fftLen) + 1u] = 0.0f;
    
    pDst[0] = pSrc[0] + pSrc[1];
    pDst[1] = 0.0f;
    
}

//--------------------------------------------------------------
void split_rifft_f32 (
                                    float * pSrc,
                                    uint32_t fftLen,
                                    const float * pATable,
                                    const float * pBTable,
                                    float * pDst)
{
    float outR, outI;                                           /* Temporary variables for output */
    const float *pCoefA, *pCoefB;                               /* Temporary pointers for twiddle factors */
    float CoefA1, CoefA2, CoefB1;                               /* Temporary variables for twiddle coefficients */
    float *pSrc1 = &pSrc[0], *pSrc2 = &pSrc[(2u * fftLen) + 1u];
    
    pCoefA = &pATable[0];
    pCoefB = &pBTable[0];
    
    while (fftLen > 0u)
    {
        /*
         outR = (pIn[2 * i] * pATable[2 * i] + pIn[2 * i + 1] * pATable[2 * i + 1] +
         pIn[2 * n - 2 * i] * pBTable[2 * i] -
         pIn[2 * n - 2 * i + 1] * pBTable[2 * i + 1]);
         outI = (pIn[2 * i + 1] * pATable[2 * i] - pIn[2 * i] * pATable[2 * i + 1] -
         pIn[2 * n - 2 * i] * pBTable[2 * i + 1] -
         pIn[2 * n - 2 * i + 1] * pBTable[2 * i]);
         */
        
        CoefA1 = *pCoefA++;
        CoefA2 = *pCoefA;
        
        /* outR = (pSrc[2 * i] * CoefA1 */
        outR = *pSrc1 * CoefA1;
        
        /* - pSrc[2 * i] * CoefA2 */
        outI = -(*pSrc1++) * CoefA2;
        
        /* (pSrc[2 * i + 1] + pSrc[2 * fftLen - 2 * i + 1]) * CoefA2 */
        outR += (*pSrc1 + *pSrc2) * CoefA2;
        
        /* pSrc[2 * i + 1] * CoefA1 */
        outI += (*pSrc1++) * CoefA1;
        
        CoefB1 = *pCoefB;
        
        /* - pSrc[2 * fftLen - 2 * i + 1] * CoefB1 */
        outI -= *pSrc2-- * CoefB1;
        
        /* pSrc[2 * fftLen - 2 * i] * CoefB1 */
        outR += *pSrc2 * CoefB1;
        //outR = ((CoefB1 + CoefA2) * (*pSrc2) + CoefA2 * (*pSrc1) + CoefA1 * (*pSrc1)) / 2;
        
        /* pSrc[2 * fftLen - 2 * i] * CoefA2 */
        outI += *pSrc2-- * CoefA2;
        
        //outR = ((CoefB1 + CoefA2) * (*pSrc2) + (CoefA2 + CoefA1) * (*pSrc1)) / 2;
        
        /* write output */
        *pDst++ = outR / 2;
        *pDst++ = outI / 2;
        
        /* update coefficient pointer */
        pCoefB = pCoefB + (2u);
        pCoefA = pCoefA + (2u - 1u);
        
        /* Decrement loop count */
        fftLen--;
    }
    
}
