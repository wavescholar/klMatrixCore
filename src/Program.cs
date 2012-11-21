using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServiceCoreTest
{
	class Program
	{
        String imageFilePAth = "D:\\BruceBCampbell\\ImageProcessing\\Pathology\\Liver";
        [System.Runtime.InteropServices.DllImport("klDll.dll", EntryPoint = "fnklDll")]
        unsafe public static extern int fnklDll();
		static void Main(string[] args)
		{
			fnklDll();

		}
	}
}
