using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Net.Security;
using System.Security;
using System.Security.AccessControl;
using System.Security.Cryptography;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography.X509Certificates;
using System.Security.Principal;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Security;
using System.Text;
using System.Xml;

namespace CertificateHelper
{
    public class CertificateManager
    {
        public static string GetMacAddress()
        {
            IPGlobalProperties.GetIPGlobalProperties();
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
            //Console.WriteLine("Interface information for {0}.{1}     ",computerProperties.HostName, computerProperties.DomainName);
            var myMac = "";
            foreach (var adapter in nics)
            {
                if (adapter != null)
                {
                    adapter.GetIPProperties();
                    //Console.WriteLine(adapter.Description);
                    //Console.WriteLine(String.Empty.PadLeft(adapter.Description.Length, '='));
                    //Console.WriteLine("  Interface type .......................... : {0}", adapter.NetworkInterfaceType);
                    //Console.WriteLine("  Physical Address ........................ : {0}", adapter.GetPhysicalAddress().ToString());
                    Console.WriteLine("  MacAddress .............................. : {0}", adapter.Id);
                    //Console.WriteLine("  Is receive only.......................... : {0}", adapter.IsReceiveOnly);
                    //Console.WriteLine("  Multicast................................ : {0}", adapter.SupportsMulticast);
                    //Console.WriteLine();

                    myMac = adapter.Id;
                }
            }
            return myMac;

        }

        public static SecureString GetSecureStringFromConsole()
        {
            var password = new SecureString();

            Console.Write("Enter Password: ");
            while (true)
            {
                ConsoleKeyInfo cki = Console.ReadKey(true);

                if (cki.Key == ConsoleKey.Enter) break;
                if (cki.Key == ConsoleKey.Escape)
                {
                    password.Dispose();
                    return null;
                }
                if (cki.Key == ConsoleKey.Backspace)
                {
                    if (password.Length != 0)
                        password.RemoveAt(password.Length - 1);
                }
                else password.AppendChar(cki.KeyChar);
            }

            return password;
        }

        public static void InstallCertificates()
        {

            LicenseKeyHelper.TestLicenseKeyHelper();

            SecureString mySs = GetSecureStringFromConsole();

            var klBaseCert = new X509Certificate2("klBase.pfx", mySs);
            var klClientCert = new X509Certificate2("klClient.pfx", mySs);
            var klServerCert = new X509Certificate2("klServer.pfx", mySs);

            var store = new X509Store("TRUST", StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);
                store.Add(klBaseCert);
            }
            catch (CryptographicException e)
            {
                Console.WriteLine(e.Message);


            }
            finally
            {
                store.Close();
            }

            store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);
                store.Add(klClientCert);
                store.Add(klServerCert);
            }
            finally
            {
                store.Close();
            }

            store = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);
                store.Add(klClientCert);
                store.Add(klServerCert);
            }
            finally
            {
                store.Close();
            }


            ValidateCert(klBaseCert);
            ValidateCert(klServerCert);
            ValidateCert(klClientCert);


            RevokeCertificate(klBaseCert);
            RevokeCertificate(klClientCert);
            RevokeCertificate(klServerCert);



        }

        public static void ValidateCert(X509Certificate2 cert)
        {
            var chain = new X509Chain();

            chain.ChainPolicy.RevocationFlag = X509RevocationFlag.EntireChain;

            chain.ChainPolicy.RevocationMode = X509RevocationMode.Online; //| X509RevocationMode.Offline;

            chain.ChainPolicy.UrlRetrievalTimeout = new TimeSpan(0, 0, 30);

            chain.ChainPolicy.VerificationFlags = X509VerificationFlags.NoFlag;

            chain.Build(cert);

            if (chain.ChainStatus.Length != 0)
                Console.WriteLine(chain.ChainStatus[0].Status);

            GrantKeyAccess.GetKey(cert);
            GrantKeyAccess.AddAccessToCertificate(cert, "kl\\bruce.campbell");

        }

        public static void RevokeCertificate(X509Certificate2 cert)
        {
            //CERTADMINLib.CCertAdminClass certAdmin = new
            //CERTADMINLib.CCertAdminClass();
            //string strConfig =
            //configurationBase.GetConfigParamValueByParamName("CAName");
            //string strSert = X509Cert.GetSerialNumberString();
            //issueDate = X509Cert.NotBefore;
            ////PrintParameters(strConfig,strSert);
            //certAdmin.RevokeCertificate(strConfig, strSert, 0,DateTime.UtcNow.Date);

            string[] CRL = X509Certificate2Extensions.GetCrlDistributionPoints(cert);

        }

        public static class LicenseKeyHelper
        {
            public static void TestLicenseKeyHelper()
            {
                var mac = GetMacAddress();

                var klLicenseCert = new X509Certificate2("klLicenseKeyGen.cer");

                var myMac = new byte[mac.Length];
                for (int i = 0; i < mac.Length; i++)
                    myMac[i] = (byte)mac[i];

                byte[] mySignedMacPrivateKey = SignMacWithPrivateKey(myMac);

                byte[] myDecodedMacPrivate = CryptoHelper.VerifyAndRemoveSignature(mySignedMacPrivateKey);

                for (int i = 0; i < myDecodedMacPrivate.Length; i++)
                {
                    if (myDecodedMacPrivate[i] != myMac[i])
                    {
                        throw new Exception("kl License Key Invalid");
                    }
                }

                var myFile = File.OpenWrite("signedMac");
                myFile.Write(mySignedMacPrivateKey, 0, mySignedMacPrivateKey.Length);
                myFile.Close();

                //delete certificates here
                var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
                try
                {
                    store.Open(OpenFlags.ReadWrite);
                    store.Add(klLicenseCert);

                }
                finally
                {
                    store.Close();
                }

                store = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
                try
                {
                    store.Open(OpenFlags.ReadWrite);
                    store.Add(klLicenseCert);

                }
                finally
                {
                    store.Close();
                }
                myFile = File.OpenRead("signedMac");
                long length = myFile.Length;
                var myRead = new byte[length];
                myFile.Read(myRead, 0, (int)length);
                myFile.Close();
                byte[] myDecodedMac = CryptoHelper.VerifyAndRemoveSignature(mySignedMacPrivateKey);
            }

            public static byte[] SignMacWithPrivateKey(byte[] mac)
            {
                X509Certificate2 myCert = CryptoHelper.FindCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectDistinguishedName, "CN=klLicenseKeyGen");

                byte[] mySignedMac = CryptoHelper.Sign(mac, myCert);

                return mySignedMac;
            }
        }

        public static class X509Certificate2Extensions
        {
            public static string[] GetCrlDistributionPoints(X509Certificate2 certificate)
            {
                if (certificate == null)
                {
                    throw new ArgumentNullException("certificate");
                }
                X509Extension ext = certificate.Extensions.Cast<X509Extension>().FirstOrDefault(
                    e => e.Oid.Value == "2.5.29.31");

                if (ext == null || ext.RawData == null || ext.RawData.Length < 11)
                    return EmptyStrings;

                int prev = -2;
                var items = new List<string>();
                while (prev != -1 && ext.RawData.Length > prev + 1)
                {
                    int next = IndexOf(ext.RawData, 0x86, prev == -2 ? 8 : prev + 1);
                    if (next == -1)
                    {
                        if (prev >= 0)
                        {
                            string item = Encoding.UTF8.GetString(ext.RawData, prev + 2, ext.RawData.Length - (prev + 2));
                            items.Add(item);
                        }

                        break;
                    }

                    if (prev >= 0 && next > prev)
                    {
                        string item = Encoding.UTF8.GetString(ext.RawData, prev + 2, next - (prev + 2));
                        items.Add(item);
                    }

                    prev = next;
                }

                return items.ToArray();
            }

            static int IndexOf(byte[] instance, byte item, int start)
            {
                for (int i = start, l = instance.Length; i < l; i++)
                    if (instance[i] == item)
                        return i;

                return -1;
            }

            private static string[] EmptyStrings = new string[0];
        }

        class GrantKeyAccess
        {
            public static void GetKey(X509Certificate2 cert)
            {
                string keyfileName = GetKeyFileName(cert);
                string keyfilePath = FindKeyLocation(keyfileName);

                Console.WriteLine(keyfilePath);
                Console.WriteLine(keyfileName);

                Console.WriteLine("Press enter to continue");
                Console.ReadLine();

            }

            private static string FindKeyLocation(string keyFileName)
            {
                // check machine path first
                string machinePath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
                string machinePathFull = machinePath + @"\Microsoft\Crypto\RSA\MachineKeys";
                string[] machineFiles = Directory.GetFiles(machinePathFull, keyFileName);
                if (machineFiles.Length > 0)
                {
                    return machinePathFull;
                }

                // then user path
                string userPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
                string userPathFull = userPath + @"\Microsoft\Crypto\RSA\";
                string[] userDirectories = Directory.GetDirectories(userPathFull);

                if (userDirectories.Length > 0)
                {
                    string[] userDirectoriesClone = userDirectories;
                    for (int i = 0; i < userDirectoriesClone.Length; i++)
                    {
                        string dir = userDirectoriesClone[i];
                        userDirectories = Directory.GetFiles(dir, keyFileName);
                        if (userDirectories.Length != 0)
                        {
                            return dir;
                        }
                    }
                }
                return null;
            }

            public static string GetKeyFileName(X509Certificate2 cert)
            {
                string filename = null;

                if (cert.PrivateKey != null)
                {
                    RSACryptoServiceProvider provider = cert.PrivateKey as RSACryptoServiceProvider;
                    filename = provider.CspKeyContainerInfo.UniqueKeyContainerName;
                }
                return filename;
            }

            public static void AddAccessToCertificate(X509Certificate2 cert, string user)
            {
                RSACryptoServiceProvider rsa = cert.PrivateKey as RSACryptoServiceProvider;
                if (rsa != null)
                {
                    string keyfilepath = FindKeyLocation(rsa.CspKeyContainerInfo.UniqueKeyContainerName);
                    FileInfo file = new FileInfo(keyfilepath + "\\" + rsa.CspKeyContainerInfo.UniqueKeyContainerName);
                    Console.WriteLine(file.Name);
                    FileSecurity fs = file.GetAccessControl();
                    NTAccount account = new NTAccount(user);
                    fs.AddAccessRule(new FileSystemAccessRule(account, FileSystemRights.FullControl, AccessControlType.Allow));
                    file.SetAccessControl(fs);
                }
            }
        }
    }

    public class CustomSecurityHeader : MessageHeader
    {
        private readonly string _key;

        public string Key
        {
            get
            {
                return _key;
            }
        }

        public CustomSecurityHeader(string key)
        {
            _key = key;
        }

        public override string Name
        {
            get { return CustomHeaderNames.CustomHeaderName; }
        }

        public override string Namespace
        {
            get { return CustomHeaderNames.CustomHeaderNamespace; }
        }

        protected override void OnWriteHeaderContents(XmlDictionaryWriter writer, MessageVersion messageVersion)
        {

            writer.WriteElementString(CustomHeaderNames.KeyName, Key);
        }

        public static CustomSecurityHeader ReadHeader(XmlDictionaryReader reader)
        {
            if (reader.ReadToDescendant(CustomHeaderNames.KeyName, CustomHeaderNames.CustomHeaderNamespace))
            {
                String key = reader.ReadElementString();
                return new CustomSecurityHeader(key);
            }
            return null;
        }
    }

    public static class CustomHeaderNames
    {
        public const String CustomHeaderName = "CustomSecurityHeader";

        public const String KeyName = "Key";

        public const String CustomHeaderNamespace = "http://schemas.kl.com/CustomSecurityHeader";

    }

    public class CryptoHelper
    {
        public static byte[] Sign(byte[] data, X509Certificate2 signingCert)
        {
            var content = new ContentInfo(data);

            var signedMessage = new SignedCms(content);

            var signer = new CmsSigner(signingCert);

            signedMessage.ComputeSignature(signer);

            byte[] signedBytes = signedMessage.Encode();

            return signedBytes;
        }

        public static byte[] SignDetached(byte[] data, X509Certificate2 signingCert)
        {
            var content = new ContentInfo(data);

            var signedMessage = new SignedCms(content, true);

            var signer = new CmsSigner(signingCert);

            signedMessage.ComputeSignature(signer);

            var signedBytes = signedMessage.Encode();
            return signedBytes;
        }

        public static void ValidateCert(X509Certificate2 cert)
        {
            if (cert == null)
            {
                throw new ArgumentNullException("cert");
            }
            // ReSharper disable BitwiseOperatorOnEnumWihtoutFlags
            var chain = new X509Chain { ChainPolicy = { RevocationFlag = X509RevocationFlag.EntireChain, RevocationMode = X509RevocationMode.Online | X509RevocationMode.Offline, UrlRetrievalTimeout = new TimeSpan(0, 0, 30), VerificationFlags = X509VerificationFlags.NoFlag } };
            // ReSharper restore BitwiseOperatorOnEnumWihtoutFlags

            chain.Build(cert);

            if (chain.ChainStatus.Length != 0)
                Console.WriteLine(chain.ChainStatus[0].Status);
        }

        public static X509Certificate2 FindCertificate(StoreLocation location, StoreName name, X509FindType findType, string findValue)
        {
            var store = new X509Store(name, location);

            try
            {
                store.Open(OpenFlags.ReadOnly);

                var col = store.Certificates.Find(findType, findValue, true);

                return col[0];
            }
            finally
            {
                store.Close();
            }
        }

        public static byte[] Encrypt(byte[] data, X509Certificate2 encryptingCert)
        {
            var plainContent = new ContentInfo(data);

            var encryptedData = new EnvelopedCms(plainContent);

            var recipient = new CmsRecipient(encryptingCert);

            encryptedData.Encrypt(recipient);

            byte[] encryptedBytes = encryptedData.Encode();

            return encryptedBytes;
        }

        public static bool VerifyDetached(byte[] data, byte[] signature)
        {
            var content = new ContentInfo(data);

            var signedMessage = new SignedCms(content, true);

            signedMessage.Decode(signature);

            try
            {
                signedMessage.CheckSignature(false);
                return true;
            }
            catch
            {
                return false;
            }
        }

        public static byte[] VerifyAndRemoveSignature(byte[] data)
        {
            var signedMessage = new SignedCms();

            signedMessage.Decode(data);

            signedMessage.CheckSignature(false);

            foreach (SignerInfo signer in signedMessage.SignerInfos)
            {
                Console.WriteLine("Subject: {0}", signer.Certificate.Subject);
            }

            return signedMessage.ContentInfo.Content;
        }
    }

    public class StreamingSecurityMessageInspector : IDispatchMessageInspector, IClientMessageInspector
    {
        //Server Side
        public object AfterReceiveRequest(IClientChannel channel, InstanceContext instanceContext)
        {
            if (channel == null)
            {
                throw new ArgumentNullException("channel");
            }
            if (instanceContext == null)
            {
                throw new ArgumentNullException("instanceContext");
            }
            Message request = null;
            return AfterReceiveRequest(ref request, channel, instanceContext);
        }

        public object AfterReceiveRequest(ref Message request, IClientChannel channel, InstanceContext instanceContext)
        {
            if (request == null)
            {
                throw new ArgumentNullException("request");
            }
            return null;
        }

        //Server Side
        public void BeforeSendReply(ref Message reply, object correlationState)
        {
            X509Certificate2 myCert = CryptoHelper.FindCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectDistinguishedName, "CN=klServer");
            var myHash = myCert.GetCertHash();

            var mySignedHash = CryptoHelper.Sign(myHash, myCert);

            var keyChars = new char[mySignedHash.Length];

            for (int i = 0; i < mySignedHash.Length; i++)
                keyChars[i] = (char)mySignedHash[i];

            reply.Headers.Add((new CustomSecurityHeader(new string(keyChars))));

            MessageBuffer buffer = reply.CreateBufferedCopy(Int32.MaxValue);
            reply = buffer.CreateMessage();

        }

        //Client Side
        public void AfterReceiveReply(ref Message reply, object correlationState)
        {
            MessageBuffer buffer = reply.CreateBufferedCopy(Int32.MaxValue);
            reply = buffer.CreateMessage();

            X509Certificate2 myCert = CryptoHelper.FindCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectDistinguishedName, "CN=klServer");
            byte[] myHash = myCert.GetCertHash();

            Int32 headerPosition = reply.Headers.FindHeader(CustomHeaderNames.CustomHeaderName, CustomHeaderNames.CustomHeaderNamespace);

            XmlDictionaryReader reader = reply.Headers.GetReaderAtHeader(headerPosition);

            CustomSecurityHeader header = CustomSecurityHeader.ReadHeader(reader);

            string mySignedHashString = header.Key;

            var mysignedHashCharArray = mySignedHashString.ToArray();

            var mySignedHash = new byte[mySignedHashString.Length];
            for (int i = 0; i < mySignedHashString.Length; i++)
                mySignedHash[i] = (byte)mysignedHashCharArray[i];

            byte[] myDecodedHash = CryptoHelper.VerifyAndRemoveSignature(mySignedHash);

            for (int i = 0; i < myHash.Length; i++)
            {
                if (myDecodedHash[i] != myHash[i])
                {
                    throw new Exception("kl Access Denied");
                }
            }
        }

        public object BeforeSendRequest(ref Message request, IClientChannel channel)
        {

            return null;
        }

    }

    public class StreamingSecurityBehavior : IEndpointBehavior
    {
        #region IEndpointBehavior Members

        public void AddBindingParameters(ServiceEndpoint endpoint, BindingParameterCollection bindingParameters)
        {
        }

        public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
        {
            var inspector = new StreamingSecurityMessageInspector();
            clientRuntime.MessageInspectors.Add(inspector);
        }

        public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
        {
            var inspector = new StreamingSecurityMessageInspector();
            endpointDispatcher.DispatchRuntime.MessageInspectors.Add(inspector);
        }

        public void Validate(ServiceEndpoint endpoint)
        {
        }

        #endregion
    }

    public class ProtectionEndpointBehavior : IEndpointBehavior
    {
        private static readonly String ProtectionSetting = Convert.ToString(ConfigurationManager.AppSettings["ProtectionLevel"]);

        public void AddBindingParameters(ServiceEndpoint endpoint, BindingParameterCollection bindingParameters)
        {
            //Find the old requirements and remove
            var requirements = bindingParameters.Find<ChannelProtectionRequirements>();
            bindingParameters.Remove(requirements);

            //Setting the ProtectionLevel at the Service Contract
            if (ProtectionSetting == "None")
                endpoint.Contract.ProtectionLevel = ProtectionLevel.None;
            if (ProtectionSetting == "Sign")
                endpoint.Contract.ProtectionLevel = ProtectionLevel.Sign;
            if (ProtectionSetting == "EncryptAndSign")
                endpoint.Contract.ProtectionLevel = ProtectionLevel.EncryptAndSign;

            //create a new set of requirements
            requirements = new ChannelProtectionRequirements();
            bindingParameters.Add(requirements);

            var unprotectedBody = new MessagePartSpecification();
            var protectedBody = new MessagePartSpecification(true);

            switch (endpoint.Contract.ProtectionLevel)
            {
                case ProtectionLevel.None:
                    requirements.OutgoingSignatureParts.AddParts(unprotectedBody, "*");
                    requirements.IncomingSignatureParts.AddParts(unprotectedBody, "*");
                    requirements.OutgoingEncryptionParts.AddParts(unprotectedBody, "*");
                    requirements.IncomingEncryptionParts.AddParts(unprotectedBody, "*");
                    break;
                case ProtectionLevel.Sign:
                    requirements.OutgoingSignatureParts.AddParts(protectedBody, "*");
                    requirements.IncomingSignatureParts.AddParts(protectedBody, "*");

                    requirements.OutgoingEncryptionParts.AddParts(unprotectedBody, "*");
                    requirements.IncomingEncryptionParts.AddParts(unprotectedBody, "*");
                    break;
                case ProtectionLevel.EncryptAndSign:
                    requirements.OutgoingSignatureParts.AddParts(protectedBody, "*");
                    requirements.IncomingSignatureParts.AddParts(protectedBody, "*");
                    requirements.OutgoingEncryptionParts.AddParts(protectedBody, "*");
                    requirements.IncomingEncryptionParts.AddParts(protectedBody, "*");
                    break;
            }
        }

        public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
        {
        }

        public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
        {
        }

        public void Validate(ServiceEndpoint endpoint)
        {

        }
    }
}