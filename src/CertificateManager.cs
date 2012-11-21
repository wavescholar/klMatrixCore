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
    public class SecurityHelper
    {
       public static void InstallCertificates(SecureString secureString)
        {
            InstallWcfSecurityCertificates(secureString);
        }

        public static string GetMacAddress()
        {
            IPGlobalProperties.GetIPGlobalProperties();
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();

            var myMac = "";
            foreach (var adapter in nics)
            {
                if (adapter != null)
                {
                    adapter.GetIPProperties();

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

        public static void InstallWcfSecurityCertificates(SecureString mySs)
        {
            var klBaseCert = new X509Certificate2(@"klBase.pfx", mySs, X509KeyStorageFlags.PersistKeySet);

            var klClientCert = new X509Certificate2(@"klClient.pfx", mySs, X509KeyStorageFlags.PersistKeySet);

            var klServerCert = new X509Certificate2(@"klServer.pfx", mySs, X509KeyStorageFlags.PersistKeySet);

            var store = new X509Store("TRUST", StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);

                RemoveCertificates(store, "CN=klBase");

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

                RemoveCertificates(store, "CN=klClient");

                store.Add(klClientCert);

                RemoveCertificates(store, "CN=klServer");

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

                RemoveCertificates(store, "CN=klClient");

                store.Add(klClientCert);

                RemoveCertificates(store, "CN=klServer");

                store.Add(klServerCert);
            }
            finally
            {
                store.Close();
            }


            GrantUserAccessToPrivateKeyFile(klBaseCert, "Everyone");
            GrantUserAccessToPrivateKeyFile(klClientCert, "Everyone");
            GrantUserAccessToPrivateKeyFile(klServerCert, "Everyone");

            ValidateCert(klBaseCert);
            ValidateCert(klServerCert);
            ValidateCert(klClientCert);
        }

        //Helper for Remove Certificate
        public static void RemoveCertificate(X509Store store, X509Certificate2 cert)
        {
            try
            {
                store.Remove(cert);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        //Removes all certificates from the store with the provided distinguished name 
        public static void RemoveCertificates(X509Store store, String distinguishedName)
        {
            var col = store.Certificates.Find(X509FindType.FindBySubjectDistinguishedName, distinguishedName, false);
            if (col.Count != 0)
            {
                for (int i = 0; i < col.Count; i++)
                {
                    RemoveCertificate(store, col[i]);
                }
            }
        }

        public static void InstallPerServerLicensePkCertificates()
        {
            SecureString mySs = GetSecureStringFromConsole();

            var klBaseCert = new X509Certificate2("klLicenseKeyGenBase.pfx", mySs);

            var klClientCert = new X509Certificate2("klLicenseKeyGen.pfx", mySs);

            var store = new X509Store("TRUST", StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);

                RemoveCertificates(store, "CN=klLicenseKeyGenBase");

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

                RemoveCertificates(store, "CN=klLicenseKeyGen");

                store.Add(klClientCert);
            }
            finally
            {
                store.Close();
            }

            store = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);

                RemoveCertificates(store, "CN=klLicenseKeyGen");

                store.Add(klClientCert);
            }
            finally
            {
                store.Close();
            }

            ValidateCert(klBaseCert);
            ValidateCert(klClientCert);
        }

        public static void InstallPerServerLicensePublicCertificates()
        {
            //SecureString mySs = GetSecureStringFromConsole();

            var klBaseCert = new X509Certificate2("klLicenseKeyGenBase.cer");

            var klClientCert = new X509Certificate2("klLicenseKeyGen.cer");

            var store = new X509Store("TRUST", StoreLocation.LocalMachine);

            try
            {
                store.Open(OpenFlags.ReadWrite);

                RemoveCertificates(store, "CN=klLicenseKeyGenBase");

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

                RemoveCertificates(store, "CN=klLicenseKeyGen");

                store.Add(klClientCert);
            }
            finally
            {
                store.Close();
            }

            store = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
            try
            {
                store.Open(OpenFlags.ReadWrite);

                RemoveCertificates(store, "CN=klLicenseKeyGen");

                store.Add(klClientCert);
            }
            finally
            {
                store.Close();
            }

            ValidateCert(klBaseCert);

            ValidateCert(klClientCert);
        }

        public static void ValidateCert(X509Certificate2 cert)
        {
            var chain = new X509Chain
            {
                ChainPolicy =
                {
                    RevocationFlag = X509RevocationFlag.EntireChain,
                    RevocationMode = X509RevocationMode.Online,
                    UrlRetrievalTimeout = new TimeSpan(0, 0, 30),
                    VerificationFlags = X509VerificationFlags.NoFlag
                }
            };

            chain.Build(cert);

            if (chain.ChainStatus.Length != 0)
                Console.WriteLine(chain.ChainStatus[0].Status);
        }

        public static void GrantUserAccessToPrivateKeyFile(X509Certificate2 cert, string user)
        {
            GrantKeyAccess.AddAccessToCertificate(cert, user);
        }

        public static class LicenseKeyHelper
        {
            public static void RunLocalLicenseKeyHelper()
            {
                InstallPerServerLicensePkCertificates();

                var mac = GetMacAddress();

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

                //Remove Certificates with private keys
                var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
                try
                {
                    store.Open(OpenFlags.ReadWrite);
                    RemoveCertificates(store, "CN=klLicenseKeyGen");
                }
                finally
                {
                    store.Close();
                }

                store = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
                try
                {
                    store.Open(OpenFlags.ReadWrite);
                    RemoveCertificates(store, "CN=klLicenseKeyGen");
                }
                finally
                {
                    store.Close();
                }

                store = new X509Store("TRUST", StoreLocation.LocalMachine);
                try
                {
                    store.Open(OpenFlags.ReadWrite);

                    RemoveCertificates(store, "CN=klLicenseKeyGenBase");


                }
                finally
                {
                    store.Close();
                }

                InstallPerServerLicensePublicCertificates();

                myFile = File.OpenRead("signedMac");

                long length = myFile.Length;

                var myRead = new byte[length];

                myFile.Read(myRead, 0, (int)length);

                myFile.Close();

                //Since we removed the private keys and instaleld the public ones, this step simulates the 
                //validation step that allows the WCF security to be installed on the machine with the specified MAC address. 

                var signedMessage = new SignedCms();

                signedMessage.Decode(myRead);

                signedMessage.CheckSignature(true);

                byte[] myDecodedMac = signedMessage.ContentInfo.Content;

                for (int i = 0; i < myDecodedMacPrivate.Length; i++)
                {
                    if (myDecodedMac[i] != myMac[i])
                    {
                        throw new Exception("kl License Key Invalid");
                    }
                }

                //Now we have the prerequisites to install the certificates on this machine.
                SecureString mySs = GetSecureStringFromConsole();

                InstallWcfSecurityCertificates(mySs);

            }

            public static byte[] RunKeyGen(string mac)
            {
                InstallPerServerLicensePkCertificates();

                var myMac = new byte[mac.Length];
                for (int i = 0; i < mac.Length; i++)
                    myMac[i] = (byte)mac[i];

                byte[] mySignedMacPrivateKey = SignMacWithPrivateKey(myMac);

                return mySignedMacPrivateKey;
            }

            public static void ValidateLicenseKeyAndInstallWcfSecurity(byte[] encodedMac)
            {
                InstallPerServerLicensePublicCertificates();

                var mac = GetMacAddress();

                var myMac = new byte[mac.Length];
                for (int i = 0; i < mac.Length; i++)
                    myMac[i] = (byte)mac[i];

                var signedMessage = new SignedCms();

                signedMessage.Decode(encodedMac);

                signedMessage.CheckSignature(true);

                byte[] myDecodedMac = signedMessage.ContentInfo.Content;

                for (int i = 0; i < myDecodedMac.Length; i++)
                {
                    if (myDecodedMac[i] != myMac[i])
                    {
                        throw new Exception("kl License Key Invalid");
                    }
                }

                //Now we have the prerequisites to install the certificates on this machine.

                char[] chars = { 'o', 'm', 'n', 'y', 'x', '1', '2', '3' };

                SecureString secureString;
                unsafe
                {
                    // Instantiate a new secure string.
                    fixed (char* pChars = chars)
                    {
                        secureString = new SecureString(pChars, chars.Length);
                    }
                }

                InstallWcfSecurityCertificates(secureString);

            }

            public static byte[] SignMacWithPrivateKey(byte[] mac)
            {
                X509Certificate2 myCert = CryptoHelper.FindCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectDistinguishedName, "CN=klLicenseKeyGen");

                byte[] mySignedMac = CryptoHelper.Sign(mac, myCert);

                return mySignedMac;
            }
        }

        class GrantKeyAccess
        {
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

            public static void AddAccessToCertificate(X509Certificate2 cert, string user)
            {
                var rsa = cert.PrivateKey as RSACryptoServiceProvider;
                if (rsa != null)
                {
                    string keyfilepath = FindKeyLocation(rsa.CspKeyContainerInfo.UniqueKeyContainerName);
                    var file = new FileInfo(keyfilepath + "\\" + rsa.CspKeyContainerInfo.UniqueKeyContainerName);
                    Console.WriteLine(file.Name);
                    var fs = file.GetAccessControl();
                    var account = new NTAccount(user);
                    fs.AddAccessRule(new FileSystemAccessRule(account, FileSystemRights.FullControl, AccessControlType.Allow));
                    try
                    {
                        file.SetAccessControl(fs);

                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Error granting key access" + e.Message);
                    }
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