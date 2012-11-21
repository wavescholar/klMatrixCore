using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
    using System;
    // it's required for reading/writing into the registry:
    using Microsoft.Win32;
    // and for the MessageBox function:
namespace Check_DTC_MSMQ_Settings
{
    /// <summary>
    /// An useful class to read/write/delete/count registry keys
    /// </summary>
    public class ModifyRegistry
    {
        private bool showError = false;

        public bool ShowError
        {
            get { return showError; }
            set { showError = value; }
        }

        private string subKey = "";
 

        public string SubKey
        {
            get { return subKey; }
            set { subKey = value; }
        }

        private RegistryKey baseRegistryKey = Registry.LocalMachine;


        public RegistryKey BaseRegistryKey
        {
            get { return baseRegistryKey; }
            set { baseRegistryKey = value; }
        }

  
        public string Read(string KeyName)
        {
            RegistryKey rk = baseRegistryKey;
            RegistryKey sk1 = rk.OpenSubKey(subKey);
  
            if (sk1 == null)
            {
                return null;
            }
            else
            {
                try
                {
                    return (string)sk1.GetValue(KeyName.ToUpper());
                }
                catch (Exception e)
                {
                    System.Console.WriteLine(e.Message + "Reading registry " + KeyName.ToUpper());
                    return null;
                }
            }
        }
        
        public bool Write(string KeyName, object Value)
        {
            try
            {
                // Setting
                RegistryKey rk = baseRegistryKey;
                // I have to use CreateSubKey 
                // (create or open it if already exits), 
                // 'cause OpenSubKey open a subKey as read-only
                RegistryKey sk1 = rk.CreateSubKey(subKey);
                // Save the value
                sk1.SetValue(KeyName.ToUpper(), Value);

                return true;
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message + "Writing registry " + KeyName.ToUpper());
                return false;
            }
        }

        public bool DeleteKey(string KeyName)
        {
            try
            {
                // Setting
                RegistryKey rk = baseRegistryKey;
                RegistryKey sk1 = rk.CreateSubKey(subKey);
                // If the RegistrySubKey doesn't exists -> (true)
                if (sk1 == null)
                    return true;
                else
                    sk1.DeleteValue(KeyName);

                return true;
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message + "Deleting SubKey " + this.subKey);
                return false;
            }
        }

        public bool DeleteSubKeyTree()
        {
            try
            {
                // Setting
                RegistryKey rk = baseRegistryKey;
                RegistryKey sk1 = rk.OpenSubKey(subKey);
                // If the RegistryKey exists, I delete it
                if (sk1 != null)
                    rk.DeleteSubKeyTree(subKey);

                return true;
            }
            catch (Exception e)
            {

                System.Console.WriteLine(e.Message + "Deleting SubKey " + this.subKey);
                return false;
            }
        }

        public int SubKeyCount()
        {
            try
            {
                // Setting
                RegistryKey rk = baseRegistryKey;
                RegistryKey sk1 = rk.OpenSubKey(subKey);
                // If the RegistryKey exists...
                if (sk1 != null)
                    return sk1.SubKeyCount;
                else
                    return 0;
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message + "Retriving subkeys of " + this.subKey);
                return 0;
            }
        }

        public int ValueCount()
        {
            try
            {
                RegistryKey rk = baseRegistryKey;
                RegistryKey sk1 = rk.OpenSubKey(subKey);

                if (sk1 != null)
                {
                    return sk1.ValueCount;
                }
                else
                {
                    return 0;
                }
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message + "Retriving keys of " + this.subKey);
                return 0;
            }
        }
    }

    public class CheckDTCSettings
    {
        public static void Check()
        {
            ModifyRegistry myRegistry = new ModifyRegistry();
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\DomainControllerState";
            System.Console.WriteLine(myRegistry.Read("1"));
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccess";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessAdmin";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessClients";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessInbound";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessOutbound";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessTip";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\NetworkDtcAccessTransactions";
            myRegistry.SubKey = "Software\\Microsoft\\MSDTC\\Security\\XaTransactions";

            myRegistry.ShowError = true;
            System.Console.WriteLine(myRegistry.Read("0"));
            int numberValues = myRegistry.ValueCount();
            for (int i = 0; i < numberValues; i++)
            {
                System.Console.WriteLine(myRegistry.Read(i.ToString()));
            }
        }
    }
}
