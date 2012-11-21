using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;


namespace klTownsendFileDataReader
{
    class Program
    {
        [Serializable]
        public struct Data
        {
            public int rows;
            public int cols;
            public double[,] data;
            public String ToString()
            {
                StringBuilder sb = new StringBuilder("");
                for (int row = 0; row < rows; row++)
                {
                    String line = "";
                    for (int col = 0; col < cols; col++)
                    {
                        if (col != cols - 2)
                            line += data[row, col] + ",";
                        if (col == cols - 1)
                            line += data[row,col];
                    }
                    sb.AppendLine(line);
                }
                return sb.ToString();
            }
        }

        public class SymbolDatePrice
        {
            public String sym;
            public String industryCode;
            public DateTime date;
            public double price;
            //Value Condition Less than 0 x is less than y.  0 x equals y.  Greater than
            //0 x is greater than y.
            static public int compare(SymbolDatePrice x, SymbolDatePrice y)
            {
                if (x.date == y.date)
                    return 0;
                else
                    return (x.date < y.date) ? 1 : -1;
            }
            //bbc revisit move this
            static public int compare(FileInfo x, FileInfo y)
            {
                if (x.Length == y.Length)
                    return 0;
                else
                    return (x.Length < y.Length) ? 1 : -1;
            }


            
        };
       
        static void RunFileRead(string[] args)
        {
            int minRequiredTicsForAnalysis = 255 * 4; //BBC where to get trading calendar?

            string[] files = Directory.GetFiles(@"C:\kl\klTSDB\SPY\DAILY");
            List<FileInfo> lfio = new List<FileInfo>();
            foreach (string s in files)
            {
                FileInfo fio = new FileInfo(s);
                lfio.Add(fio);
            }

          

            
            lfio.Sort(
                         delegate(FileInfo x, FileInfo y)
                        {
                            if (x.Length == y.Length)
                                return 0;
                            else
                                return (x.Length < y.Length) ? 1 : -1;
                        }
            );
            
            Dictionary<string, string> symbolIndustryCodes = new Dictionary<string, string>();
            StreamReader codeStream = new StreamReader(@"C:\kl\klTSDB\SPY\IndustryCodes.csv");
            string codeline = codeStream.ReadLine();
            while (codeline != null)
            {
                char[] splitter = { ',' };
                string[] splitcodeline = codeline.Split(splitter);
                symbolIndustryCodes.Add(splitcodeline[0], splitcodeline[1]);
                codeline = codeStream.ReadLine();
            }

            Dictionary<string, List<SymbolDatePrice>> sdDictionary = new Dictionary<string, List<SymbolDatePrice>>();
            int minTics = int.MaxValue;
            for (int i = 0; i < lfio.Count; i++)
            {
                FileInfo finfo = lfio[i];
                //if (!finfo.Name.Contains("DAILY"))
                if (!finfo.Name.Contains("INTRADAY"))
                    continue;
                else
                {
                    StringBuilder sb = new StringBuilder(finfo.Name);
                    sb.Replace(".txt", "");
                    char[] splitter = { '_' };
                    string[] split = sb.ToString().Split(splitter);
                    String symbolName = split[0];
                    StreamReader sr = new StreamReader(files[i]);
                    string line = null;
                    line = sr.ReadLine();
                    List<SymbolDatePrice> sdpList = new List<SymbolDatePrice>();
                    while (line != null)
                    {
                        SymbolDatePrice sdp = new SymbolDatePrice();
                        sdp.sym = symbolName;
                        try
                        {
                            sdp.industryCode = symbolIndustryCodes[symbolName];
                        }
                        catch (Exception e)
                        {
                            sdp.industryCode = "NotInSPYAnymore";
                            symbolIndustryCodes.Add(sdp.sym, sdp.industryCode);
                        }
                        char[] linesplitter = { ',' };
                        string[] splitline = line.Split(linesplitter);
                        line = sr.ReadLine();
                        char[] datesplitter = { '/' };
                        string[] datesplit = splitline[0].Split(datesplitter);
                        DateTime dt = new DateTime(Convert.ToInt32(datesplit[0]), Convert.ToInt32(datesplit[1]), Convert.ToInt32(datesplit[2]));
                        double price = Convert.ToDouble(splitline[2]);
                        sdp.price = price;
                        sdp.date = dt;
                        sdpList.Add(sdp);
                    }
                    sdpList.Sort(SymbolDatePrice.compare);
                    if (sdpList.Count < minRequiredTicsForAnalysis)
                        continue;
                    minTics = Math.Min(minTics, sdpList.Count);
                    sdDictionary.Add(symbolName, sdpList);
                    System.Console.WriteLine(symbolName);
                }

            }

            List<String> symbolsSortedByIndustryCode = new List<string>();
            foreach (String key in sdDictionary.Keys)
            {
                symbolsSortedByIndustryCode.Add(key);
            }
            symbolsSortedByIndustryCode.Sort(
                delegate(String s1, String s2)
                {
                    return symbolIndustryCodes[s1].CompareTo(symbolIndustryCodes[s2]);
                }
            );
            
            int numStreams = sdDictionary.Count;
            double[,] data = new double[ numStreams,minTics];
            double[,] returns = new double[ numStreams,minTics-1];
            //Dictionary<string, List<SymbolDatePrice>>.Enumerator dictEnumerate = sdDictionary.GetEnumerator();
            for (int k = 0; k < sdDictionary.Count; k++)
            {
                //dictEnumerate.MoveNext();
                //List<SymbolDatePrice> pdata = dictEnumerate.Current.Value;
                //String symbol = dictEnumerate.Current.Key;
                String symbol = symbolsSortedByIndustryCode[k];
                List<SymbolDatePrice> pdata = sdDictionary[symbolsSortedByIndustryCode[k]];
                System.Console.WriteLine("    " + symbol);
                for (int l = 0; l < minTics; l++)
                {
                    data[k, l] = pdata[l].price;
                    if(l<minTics-1)
                        returns[k, l] = pdata[l].price ==0? 0: (pdata[l+1].price - pdata[l].price)/pdata[l].price;
                }
            }

            unsafe
            {
                Data serializable = new Data();
                serializable.cols = minTics;
                serializable.rows = numStreams;
                serializable.data = data;
                Stream stream = File.Open("data.xml", FileMode.Create);
                SoapFormatter formatter = new SoapFormatter();

                formatter.Serialize(stream, serializable);
                stream.Close();

                StreamWriter sr = new StreamWriter("Data.csv");
                sr.Write(serializable.ToString());
                sr.Close();

                serializable.cols = minTics - 1;
                serializable.data = returns;
                sr = new StreamWriter("Returns.csv");
                sr.Write(serializable.ToString());
                sr.Close();

                fixed (double* pData = &data[0, 0])
                {
                    //We should be able to call klDll unmanaged here 
                    //[ ? With no memory copy? ]
                }

            }
        }//End Main() 

    }
}
