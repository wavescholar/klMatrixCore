using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.DataParallelArrays;
using PA = Microsoft.Research.DataParallelArrays.ParallelArrays;
using FPA = Microsoft.Research.DataParallelArrays.FloatParallelArray;
using DFPA = Microsoft.Research.DataParallelArrays.DisposableFloatParallelArray;

namespace klGPGPU
{
    public class klAccelerator
    {
        ~klAccelerator()
        {
            //img.Dispose();
            PA.UnInit();
        }

        //Do a Gaussian convolution using the Parallel Accelerator Lib.
        //this is a GP_GPU Calculations.
        public DFPA GaussianConv(DFPA img, int kernelSize, float sigma)
        {
            int size;


            PA.InitGPU();

            float[] coeff = ComputeCoefficients(kernelSize, sigma);
            size = 1000;
            img = convolution(coeff, img);
            return img;

        }

       public static float[] ComputeCoefficients(int filterSize, float sigma)
        {
            float[] result = new float[filterSize];
            float sum = 0;
            for (int i = 0; i < result.Length; i++)
            {
                result[i] = (float)Math.Exp(-(i - 2) * (i - 2) / (2 * sigma * sigma));
                sum += result[i];
            }
            for (int i = 0; i < result.Length; i++)
            {
                result[i] /= sum;
            }
            return result;
        }

        public DFPA convolution(float[] kernel, DFPA pa)
        {
           // DFPA pa = new DFPA(img);

            // Convolve in X direction.
            FPA resultX = new FPA(0, pa.Shape);
            for (int i = 0; i < kernel.Length; i++)
            {
                resultX += PA.Shift(pa, 0, i) * kernel[i];
            }

            // Convolve in Y direction.
            FPA resultY = new FPA(0, pa.Shape);
            for (int i = 0; i < kernel.Length; i++)
            {
                resultY += PA.Shift(resultX, i, 0) * kernel[i];
            }

            DFPA result = PA.Evaluate(resultY);
            
            return result;

            //pa.Dispose();
        }
    }

}
