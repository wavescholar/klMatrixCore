using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.Text;

//using securityDLL;

namespace ConsoleApplication1
{
    class Program
    {
        public static void StartHost()
        {
             //System.ServiceModel.Activation.Configuration .

            //Type myType = typeof(Uri);
            //Array myURIArray = System.Array.CreateInstance(myType, 1);
            //Uri myUri = new Uri("http://localhost:444/Dogs/DogPoundSecure/");
            //myURIArray.SetValue(myUri, 0);
            //Type myServiceType = typeof(WcfServiceLibrary.Service1);
            //System.Uri[] mUris = new Uri[1];
            //mUris[0] = myUri;
            using (System.ServiceModel.ServiceHost mServiceHost = new ServiceHost(typeof(WcfServiceLibrary.Service1)))
            {
                mServiceHost.Open();
                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.ReadLine();
                mServiceHost.Close();
            }
        }
        static void Main(string[] args)
        {
            //securityDLL.klSecurityWrapper mySecWrapper = new klSecurityWrapper();
            //mySecWrapper.enumerateSecurityProviders();

            //mySecWrapper.enumerateCertificateStoreLocations();

            //mySecWrapper.enumerateCertificates();

            StartHost();
            Console.WriteLine("Your WFC SErvice @ net.tcp://localhost:444/Dogs/DogPoundSecure Has Been Started.  Press Enter to end program.");
            Console.ReadLine();
        }
    }
}
