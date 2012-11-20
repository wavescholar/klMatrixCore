using System;
using System.Threading;
using System.Collections;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Messaging;
using System.Runtime.InteropServices;
using klWorkFlow;
using klSymbolCalculus;
namespace klBasketWorkFlow
{

    //These analysis classes render algorithm graphs.  Some render partial graphs as part of a Model.

    [Serializable]
    public class klBasketAnalysis : klAnalysis
    {

        //Fill these in and the run method will set up the info in the klBasket parameter.
        public static ArrayList m_mqhostIDs;
        public static ArrayList m_mqMappings;
        public static ArrayList m_dbhostIDs;
        public static ArrayList n_dbMappings;

        public klBasketAnalysis()
        {
            m_mqhostIDs = new ArrayList();
            m_mqMappings = new ArrayList();
            m_dbhostIDs = new ArrayList();
            n_dbMappings = new ArrayList();
        }
        [STAThread]
        public static void run(klBasket B)
        {
            klMessageHost klmh = new klMessageHost();
            int size = m_mqhostIDs.Count;
            for (int i = 0; i < size; i++)
            {
                klmh.m_HostIdMapping.Add(i, m_mqhostIDs[i]);
                klmh.m_MessageQueueMappings.Add(i, m_mqMappings[i]);

            }

            klDataHost kldh = new klDataHost();
            for (int i = 0; i < m_mqhostIDs.Count; i++)
            {
                kldh.m_DBMappings.Add(i, n_dbMappings[i]);
                kldh.m_HostIdMapping.Add(i, m_dbhostIDs[i]);
            }


            klMQAnalysis kla = new klMQAnalysis();
            kla.m_messageHostResources = klmh;  //Something on grid03

            klSQLServerAnalysis klssa = new klSQLServerAnalysis();
            klssa.m_dataHostResources = kldh;  //something on shuttle


            klDTCAnalysisResources kldtc = new klDTCAnalysisResources();
            kldtc.m_GridElements.Add(klmh);
            kldtc.m_Servers.Add(kldh);

            //Algorithm Graph 
            klAlgorithmGraph klag = new klAlgorithmGraph();
            //sources
            klADOSource kladosrc = new klADOSource();
            kladosrc.setName(kladosrc.GetType().FullName);

            klFileSource klfsrc = new klFileSource("in.txt");
            klfsrc.setName(klfsrc.GetType().FullName);
            //transforms
            klMatlabAlgorithm klma = new klMatlabAlgorithm();
            klma.setName(klma.GetType().FullName);

            klRAlgorithm klra = new klRAlgorithm();
            klra.setName(klra.GetType().FullName);
            //sinks
            klFileSink klfsnk = new klFileSink("moo.txt");
            klfsnk.setName(klfsnk.GetType().FullName);

            klADOSink kladosnk = new klADOSink();
            kladosnk.setName(kladosnk.GetType().FullName);

            klLocalTransform kllt = new klLocalTransform();
            kllt.setName(kllt.GetType().FullName);

            klag.m_Sources.Add(0, klfsrc);

            klag.m_Sources.Add(1, kladosrc);

            klag.m_Transforms.Add(0, klma);

            klag.m_Transforms.Add(1, klra);

            klag.m_Transforms.Add(2, kllt);

            klag.m_Sinks.Add(0, klfsnk);

            klag.m_Sinks.Add(1, kladosnk);


            //Make Connections
            klma.m_ICP.connect(kladosrc.m_OCP);

            klra.m_ICP.connect(klfsrc.m_OCP);

            kllt.m_ICP.connect(klma.m_OCP);

            kllt.m_ICP2.connect(klra.m_OCP);

            kladosnk.m_ICP.connect(kllt.m_OCP2);

            klfsnk.m_ICP.connect(kllt.m_OCP);




            kladosnk.render();

            klfsnk.render();

            kladosnk.persist();

            klfsnk.persist();

            //			ADOSRC-------matlab--------|                                       |----------FILESINK
            //                                     |                                       |
            //				                       |---------MYLocalTransform -------------|  
            //                                     |                                       |
            //									   |                                       |----------ADOSINK
            //			FILESRC--------R-----------|

        }
    }

    [Serializable]
    public class klCatTicsByExpiryAnalysis : klAnalysis
    {
        klBasket basket;
        DateTime beginDate;//=null;//= monthCalendar1.SelectionStart;
        DateTime endDate;//=null;//= monthCalendar2.SelectionStart;
        public void setBasket(klBasket pbasket, DateTime b, DateTime e)
        {
            basket = pbasket;
            beginDate = b;
            endDate = e;
        }

        public new void run()
        {
            //klBasket basket=new klBasket();
            //basket.m_basketSymbol=new klSymbolCalculus.IWM_BasketSymbol();
            //basket.m_basketSymbol="SPY";
            //basket.m_components=klSymbolCalculus.SymbolSet.RUTSymbols();
            string baseFileName = "c:/klTicData/";

            klBasketWorkFlow.klCatTicsByExpiryAnalysis catDriver = new klBasketWorkFlow.klCatTicsByExpiryAnalysis();
            ArrayList files = catDriver.ProcessNAS(basket, baseFileName, beginDate, endDate);

        }
        //baseFileName is where raw tics are located
        public ArrayList ProcessNAS(klBasket basket, string baseFileName, DateTime beginDate, DateTime endDate)
        {
            ArrayList files = new ArrayList();

            // System.Windows.Forms.DateTimePicker dateTimePicker1;
            // System.Windows.Forms.MonthCalendar monthCalendar1;
            // System.Windows.Forms.DateTimePicker dateTimePicker2;
            // System.Windows.Forms.MonthCalendar monthCalendar2;
            //DateTime beginDate= monthCalendar1.SelectionStart;
            //DateTime endDate= monthCalendar2.SelectionStart;
            //ArrayList IWMsymbols=klSymbolCalculus.SymbolSet.RUTSymbols();
            //ArrayList SPYsymbols=klSymbolCalculus.SymbolSet.SandPSymbols();

            ArrayList symbols = basket.m_basketSymbol.getSymbols();
            //klSymbolCalculus.SymbolSet.SandPSymbols();
            int symbolsSize = symbols.Count;
            //SPYsymbols.Count;

            //long[] timeTic=null;
            //long[] priceTic=null;
            //long[] volTic = null;


            long[] timeTic = new long[16000000];
            double[] priceTic = new double[16000000];
            long[] volTic = new long[16000000];
            long ticsSpaceLeft = 16000000;
            long TIC_MAX = 16000000;
            long ticsRecorded = 0;

            for (int symbolCount = 0; symbolCount < symbolsSize; symbolCount++)
            {
                DateTime d2 = DateTime.Now;

                System.TimeSpan ts = endDate - beginDate;

                //Find the number of days in the range

                DateTime d = beginDate;
                double ndays = ts.TotalDays;
                //Call a function to see if the next date is a trading day.
                for (int i = 0; i < ndays; i++)
                {

                    System.DateTime di = d;
                    System.TimeSpan tsi = new System.TimeSpan(i, 0, 0, 0);
                    di = di.Add(tsi);
                    string month_yr = baseFileName + di.Month + "_" + di.Year;
                    string fileName = month_yr + "/" + (string)symbols[symbolCount]; ;
                    string fileDate = +di.Month + "_" + di.Day + "_" + di.Year; ;
                    fileName += "_TICKS__" + fileDate;
                    fileName += ".txt";
                    try
                    {
                        if (!File.Exists(fileName))
                        {
                            //Console.WriteLine("{0} does not exist.", fileName);
                            throw new System.Exception("No File");
                        }

                        // Make a reference to a directory.
                        //DirectoryInfo di = new DirectoryInfo(baseFileName);
                        // Get a reference to each file in that directory.
                        //FileInfo[] fiArr = di.GetFiles();
                        // Display the names and sizes of the files.
                        //Console.WriteLine("The directory {0} contains the following files:", di.Name);
                        //foreach (FileInfo f in fiArr)
                        //	Console.WriteLine("The size of {0} is {1} bytes.", f.Name, f.Length);

                        StreamReader lsw = new StreamReader(fileName);
                        bool eof = false;
                        while (!eof)
                        {
                            string line = lsw.ReadLine();
                            long ticb = beginDate.Ticks;
                            string delimStr = ",";
                            char[] delimiter = delimStr.ToCharArray();
                            string[] split = null;
                            string[] vals = line.Split(delimiter);

                            //Get the dt for this trade
                            //long ticc=;
                            //DateTime ticn=
                            string dto = vals[0] + " " + vals[1];
                            DateTime pdtt = DateTime.Parse(dto);
                            long pdtt_t = pdtt.Ticks;

                            if (line == null)
                            {
                                eof = true;

                            }
                            ticsSpaceLeft--;
                            if (ticsSpaceLeft <= 0)
                                throw new System.Exception("klProcessNAS: out of space for tics");
                            timeTic.SetValue(pdtt_t, ticsRecorded);
                            volTic.SetValue(long.Parse(vals[3]), ticsRecorded);
                            double dd = double.Parse(vals[2]);
                            priceTic.SetValue(dd, ticsRecorded);
                            ticsRecorded++;
                        }

                    }
                    catch
                    {
                        //"SorryBoss";
                    }


                }
                //end days loop


                //Now we should have the full series in timeTic,volTic,priceTic
                //Write to a file
                {

                    string fileName = "c:/klTSDB/" + basket.m_basketSymbol.getBaseSymbol() + "/TICKS_BY_EXPIRY/" + endDate.Month + "_" + endDate.Year;
                    fileName += "/" + (string)symbols[symbolCount];
                    fileName += "_" + beginDate.Month + "_" + beginDate.Day + "_" + beginDate.Year;
                    fileName += "_" + endDate.Month + "_" + endDate.Day + "_" + endDate.Year;
                    fileName += ".txt";

                    files.Add(fileName);

                    StreamWriter sw = new StreamWriter(fileName);
                    for (int l = 0; l < ticsRecorded; l++)
                    {
                        string line = timeTic[l] + ", " + volTic[l] + ", " + priceTic[l];
                        sw.WriteLine(line);
                    }
                    sw.Close();
                    ticsRecorded = 0;
                    ticsSpaceLeft = TIC_MAX;




                }


            }

            return files;
        }


    }

    [Serializable]
    public class klModifiedAltmanCRMAnalysis : klAnalysis
    {
        //A Modified Altman Scheme
        //This should still use ratios - but possibly we extend into more than two 
        //groups, and do an ordinal model for four stages of health.  We can incorporate 
        //dummy ratios for exogenous factors not considered in the original model.
        //The original Altman Model specifies four ratios.
        //Z=\alpha_1 x_1 + \alpha_2 x_2 + \alpha_3 x_3 + \alpha_4 + x_4 + \alpha_5 x_5 
        //where: 
        //x_1= working capital/total assets
        //x_2= retained earnings/ total assets
        //x_3= earnings before interest & taxes / total assets
        //x_4= market value / book value of liabilities
        //x_5= sales / assets



    }

    [Serializable]
    public class klVolatilitySurfaceAnalysis : klAnalysis
    {



    }
    [Serializable]
    public class klPortfolioAnalysis : klAnalysis
    {


    }

    [Serializable]
    public class klBurstDetectionAnalysis : klAnalysis
    {


    }

    [Serializable]
    public class klDFTCorrelationAnalysis : klAnalysis
    {


    }

    [Serializable]
    public class klPCAAnalysis : klAnalysis
    {


    }

    [Serializable]
    public class klMultivariateVARAnalysis : klAnalysis
    {


    }

    [Serializable]
    public class klEquityFundamentalsAnalysis : klAnalysis
    {

    }

    [Serializable]
    public class klBasketETFModelAnalysis : klAnalysis
    {

    }

    [Serializable]
    public class klKineticMarketAnalysis : klAnalysis
    {

    }



    //



    ////////////////////Models 
    ///
    public class klDIAModel : klModel
    {
        klDIAModel()
        {
            //Steps :
            //		Instantiate algorithm graph
            //		Add to analysis classes
        }

        //Analysis classes go here

        //renders various components of algorithm graph via calling run on analysis classes
        //run populates RI's advertized by data desc(same keys are used as object keys in getData transform calls)
        public new void run()
        {
        }

    }

    [Serializable]
    public class klBasket : RenderedItem
    {

        //Use below for binary serilaization
        //see elsewhere for soap code
        //		IFormatter formatter = new BinaryFormatter();
        //		Stream basketStream = new FileStream("klBasketFile.bin", FileMode.Create, FileAccess.Write, FileShare.None);
        //		formatter.Serialize(basketStream, klBasketInstance);
        //		basketStream.Close();



        public klBasketSymbol m_basketSymbol;
        //stores industry code for components
        public Hashtable m_componentsIndustryCode;
        public ArrayList m_components;
        //public enum m_weightType{MARKETCAP, PRICE, UNIFORM};//[]
        public string m_weightType;//{MARKETCAP, PRICE, UNIFORM};//[]
        public Hashtable m_weights;


        //DataFiles

        //The tables with fundamental snapshots
        public ArrayList m_FactTables;

        //Flat Files		
        public ArrayList m_DailyFiles;

        public ArrayList m_TicDataFiles;

        public ArrayList m_QuoteChainFiles;

        public ArrayList m_OptionHashFiles;

        public ArrayList m_15minOptionBarFiles;

        //Serialized xml from klLiveDataVolatilitysurface.
        public ArrayList m_VolatilitySurfaceSnapshots;



    }
    [Serializable]
    public class klOptionInstance : RenderedItem
    {
        public klOptionInstance()
        {
        }



        //bbcrevisit
        //later we persist with OR mapper
        //String m_barFilename;//Filename for 15 day 5min bar
        //tic filenames

        //		get 
        //		{
        //		return m_openIntererst;
        //	}
        //	set 
        //{	
        //	m_openInterest=value;
        //}
        public String m_symbol;
        public String m_underlying;
        //TAL NOTE 1109	PUTCALLIND	STRING	
        //Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier
        public String m_instrumentType;

        public int m_openInterest;
        public DateTime m_now;
        public DateTime m_expiry;
        public double m_strike;

        public double m_price;
        public double m_bid;
        public double m_ask;

        public double m_underlyingPrice;
        public double m_riskFreeRate;
        public double m_dividendYield;
    }

    [Serializable]
    public class klVolatilityRI : RenderedItem
    {
        public Hashtable m_symbolHash;

        public string m_fileName;//This is the key used in rendering mechanism
    }

    [Serializable]
    public class klBasketTicAggRI : RenderedItem
    {
        //public ArrayList m_DailyFiles;

        public ArrayList TIC_FILES;
        public ArrayList PRICE_FILES;
        public ArrayList VOL_FILES;
        public ArrayList RETURN_FILES;
        public klBasketTicAggRI()
        {
            TIC_FILES = new ArrayList();
            PRICE_FILES = new ArrayList();
            VOL_FILES = new ArrayList();
            RETURN_FILES = new ArrayList();
        }


        //public ArrayList m_15minOptionBarFiles;

        //public ArrayList m_VolatilitySurfaceSnapshotsResultsFile;



    }


    [Serializable]
    public class klDIADataSource : klAlgorithmDataSource
    {



        public klBasket m_basket;


        //Populates flat file Lists - revisit in preparation for nHibernate version
        klDIADataSource(string ticFileDir, string dailyFileDir, string optionHashFileDir)
        {
            klSymbolCalculus.DIA_BasketSymbol basketSymbol = new klSymbolCalculus.DIA_BasketSymbol();
            m_basket = new klBasket();
            m_basket.m_basketSymbol = basketSymbol;
            //basket.m_componentsIndustryCode=klSymbolCalculus.SymbolSet.
            //ArrayList sortedSPY =new ArrayList();
            m_basket.m_components = klSymbolCalculus.SymbolSet.DIA_SYMBOLS();

            //Only Add non zero files - code is robust to nan's 

            //This code sorts DIA files by filesize/tics
            {
                string dirName = ticFileDir;
                //"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_TicDataFiles.Add(val);
                }
            }


            {
                string dirName = dailyFileDir;//"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_DailyFiles.Add(val);
                }
            }

            //basket.m_FactTables.Add("");

            //basket.m_QuoteChainFiles.Add("");



        }


        public override RenderedItem getData(object key)
        {
            return m_basket;
        }

    }


    [Serializable]
    public class klSPYDataSource : klAlgorithmDataSource
    {



        public klBasket m_basket;


        //Populates flat file Lists - revisit in preparation for nHibernate version
        klSPYDataSource(string ticFileDir, string dailyFileDir, string optionHashFileDir)
        {
            klSymbolCalculus.SPY_BasketSymbol basketSymbol = new klSymbolCalculus.SPY_BasketSymbol();
            m_basket = new klBasket();
            m_basket.m_basketSymbol = basketSymbol;
            m_basket.m_componentsIndustryCode = klSymbolCalculus.SymbolSet.SandPSymCode();
            //ArrayList sortedSPY =new ArrayList();
            m_basket.m_components = klSymbolCalculus.SymbolSet.SandPSymbols();

            //Only Add non zero files - code is robust to nan's 


            {
                string dirName = ticFileDir;
                //"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_TicDataFiles.Add(val);
                }
            }


            {
                string dirName = dailyFileDir;//"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_DailyFiles.Add(val);
                }
            }

            //basket.m_FactTables.Add("");

            //basket.m_QuoteChainFiles.Add("");



        }


        public override RenderedItem getData(object key)
        {
            return m_basket;
        }

    }

    [Serializable]
    public class klIWMDataSource : klAlgorithmDataSource
    {



        public klBasket m_basket;


        //Populates flat file Lists - revisit in preparation for nHibernate version
        klIWMDataSource(string ticFileDir, string dailyFileDir, string optionHashFileDir)
        {
            klSymbolCalculus.IWM_BasketSymbol basketSymbol = new klSymbolCalculus.IWM_BasketSymbol();
            m_basket = new klBasket();
            m_basket.m_basketSymbol = basketSymbol;
            m_basket.m_components = klSymbolCalculus.SymbolSet.RUTSymbols();

            {
                string dirName = ticFileDir;
                //"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_TicDataFiles.Add(val);
                }
            }


            {
                string dirName = dailyFileDir;//"c:/klTSDB/"+basket.m_basketSymbol.getBaseSymbol()+"/TICKS_BY_EXPIRY/"+endDate.Month+"_"+endDate.Year;
                DirectoryInfo di = new DirectoryInfo(dirName);
                FileSystemInfo[] dirs = di.GetFileSystemInfos();
                SortedList fileLengths = new SortedList();
                Random eRand = new Random(4);
                for (int q = 0; q < dirs.Length; q++)
                {
                    FileSystemInfo fio = dirs[q];
                    string fname = fio.FullName;
                    StreamReader sr = new StreamReader(fname);
                    double len = sr.BaseStream.Length;
                    if (len == 0)
                        continue;
                    if (fileLengths.ContainsKey(len))
                    {
                        len = len + eRand.NextDouble();
                        fileLengths.Add(len, fname);
                    }
                    else
                    {
                        fileLengths.Add(len, fname);
                    }

                }
                //fileLengths.
                IList srf = fileLengths.GetValueList();

                IEnumerator fIdx = srf.GetEnumerator();
                for (int i = 0; i < srf.Count; i++)
                {
                    fIdx.MoveNext();
                    //string sy=(string)fIdx.Key;
                    string val = (string)fIdx.Current;
                    m_basket.m_DailyFiles.Add(val);
                }
            }

            //basket.m_FactTables.Add("");

            //basket.m_QuoteChainFiles.Add("");



        }


        public override RenderedItem getData(object key)
        {
            return m_basket;
        }

    }

    [Serializable]
    public class klQuantLibOptionsTransform : klAlgorithmTransform
    {

        //Takes portfolio analysis results and option hash files to produce
        //vannila prices via quantlib.

        //Secondary ICP - 
        public ICP m_ICP2;

        public klQuantLibOptionsTransform()
            : base()
        {
            m_ICP2 = new ICP();
            m_OCP2 = new OCP();
            m_OCP2.setTransform(this);



        }

        //Secondary OCP

        public OCP m_OCP2;

        public override RenderedItem getData(object key)
        {
            //We need to know which ocp was  used to make this call if multiple outputs are allowed!

            //Tic Data
            RenderedItem ri1 = m_ICP2.getData(key);

            klEquityInstance ei = (klEquityInstance)ri1;


            //Options instance - 
            RenderedItem ri2 = m_ICP.getData(key);

            klOptionInstance oi = (klOptionInstance)ri2;

            //Do our stuff 

            RenderedItem ri = new RenderedItem();

            //call into quatlib



            return ri;

        }




    }

    [Serializable]
    public class klProcessNASSSource : klFileListSource
    {
        klBasket m_basket;
        string m_baseFileName;
        DateTime m_beginDate;
        DateTime m_endDate;

        klProcessNASSSource(klBasket basket, string baseFileName, DateTime beginDate, DateTime endDate)
        {
            m_basket = basket;
            m_baseFileName = baseFileName;
            m_beginDate = beginDate;
            m_endDate = endDate;
        }


        public override RenderedItem getData(object key)
        {
            klFileListRI ri = new klFileListRI();

            ri.m_baseFileName = m_baseFileName;

            ri.m_files = ProcessNAS(m_basket, m_baseFileName, m_beginDate, m_endDate);

            return ri;
        }
        public ArrayList ProcessNAS(klBasket basket, string baseFileName, DateTime beginDate, DateTime endDate)
        {
            ArrayList files = new ArrayList();

            // System.Windows.Forms.DateTimePicker dateTimePicker1;
            // System.Windows.Forms.MonthCalendar monthCalendar1;
            // System.Windows.Forms.DateTimePicker dateTimePicker2;
            // System.Windows.Forms.MonthCalendar monthCalendar2;
            //DateTime beginDate= monthCalendar1.SelectionStart;
            //DateTime endDate= monthCalendar2.SelectionStart;
            //ArrayList IWMsymbols=klSymbolCalculus.SymbolSet.RUTSymbols();
            //ArrayList SPYsymbols=klSymbolCalculus.SymbolSet.SandPSymbols();

            ArrayList symbols = basket.m_basketSymbol.getSymbols();
            //klSymbolCalculus.SymbolSet.SandPSymbols();
            int symbolsSize = symbols.Count;
            //SPYsymbols.Count;

            //long[] timeTic=null;
            //long[] priceTic=null;
            //long[] volTic = null;


            long[] timeTic = new long[16000000];
            double[] priceTic = new double[16000000];
            long[] volTic = new long[16000000];
            long ticsSpaceLeft = 16000000;
            long TIC_MAX = 16000000;
            long ticsRecorded = 0;

            for (int symbolCount = 0; symbolCount < symbolsSize; symbolCount++)
            {
                DateTime d2 = DateTime.Now;

                System.TimeSpan ts = endDate - beginDate;

                //Find the number of days in the range

                DateTime d = beginDate;
                double ndays = ts.TotalDays;
                //Call a function to see if the next date is a trading day.
                for (int i = 0; i < ndays; i++)
                {

                    System.DateTime di = d;
                    System.TimeSpan tsi = new System.TimeSpan(i, 0, 0, 0);
                    di = di.Add(tsi);
                    string month_yr = baseFileName + di.Month + "_" + di.Year;
                    string fileName = month_yr + "/" + (string)symbols[symbolCount]; ;
                    string fileDate = +di.Month + "_" + di.Day + "_" + di.Year; ;
                    fileName += "_TICKS__" + fileDate;
                    fileName += ".txt";
                    try
                    {
                        if (!File.Exists(fileName))
                        {
                            //Console.WriteLine("{0} does not exist.", fileName);
                            throw new System.Exception("No File");
                        }

                        // Make a reference to a directory.
                        //DirectoryInfo di = new DirectoryInfo(baseFileName);
                        // Get a reference to each file in that directory.
                        //FileInfo[] fiArr = di.GetFiles();
                        // Display the names and sizes of the files.
                        //Console.WriteLine("The directory {0} contains the following files:", di.Name);
                        //foreach (FileInfo f in fiArr)
                        //	Console.WriteLine("The size of {0} is {1} bytes.", f.Name, f.Length);

                        StreamReader lsw = new StreamReader(fileName);
                        bool eof = false;
                        while (!eof)
                        {
                            string line = lsw.ReadLine();
                            long ticb = beginDate.Ticks;
                            string delimStr = ",";
                            char[] delimiter = delimStr.ToCharArray();
                            string[] split = null;
                            string[] vals = line.Split(delimiter);

                            //Get the dt for this trade
                            //long ticc=;
                            //DateTime ticn=
                            string dto = vals[0] + " " + vals[1];
                            DateTime pdtt = DateTime.Parse(dto);
                            long pdtt_t = pdtt.Ticks;

                            if (line == null)
                            {
                                eof = true;

                            }
                            ticsSpaceLeft--;
                            if (ticsSpaceLeft <= 0)
                                throw new System.Exception("klProcessNAS: out of space for tics");
                            timeTic.SetValue(pdtt_t, ticsRecorded);
                            volTic.SetValue(long.Parse(vals[3]), ticsRecorded);
                            double dd = double.Parse(vals[2]);
                            priceTic.SetValue(dd, ticsRecorded);
                            ticsRecorded++;
                        }

                    }
                    catch
                    {
                        //"SorryBoss";
                    }


                }
                //end days loop


                //Now we should have the full series in timeTic,volTic,priceTic
                //Write to a file
                {

                    string fileName = "c:/klTSDB/" + basket.m_basketSymbol.getBaseSymbol() + "/TICKS_BY_EXPIRY/" + endDate.Month + "_" + endDate.Year;
                    fileName += "/" + (string)symbols[symbolCount];
                    fileName += "_" + beginDate.Month + "_" + beginDate.Day + "_" + beginDate.Year;
                    fileName += "_" + endDate.Month + "_" + endDate.Day + "_" + endDate.Year;
                    fileName += ".txt";

                    files.Add(fileName);

                    StreamWriter sw = new StreamWriter(fileName);
                    for (int l = 0; l < ticsRecorded; l++)
                    {
                        string line = timeTic[l] + ", " + volTic[l] + ", " + priceTic[l];
                        sw.WriteLine(line);
                    }
                    sw.Close();
                    ticsRecorded = 0;
                    ticsSpaceLeft = TIC_MAX;




                }


            }

            return files;
        }


    }

    [Serializable]
    public class klTicSlicerTransform : klAlgorithmTransform
    {
        //makes flat bin files  
        //hooks up to process nas, and feeds portfolio, matlab, burst detection, and correlation algorithms 
    }

    [Serializable]
    public class klCRMTransform : klAlgorithmTransform
    {
        //Connects up to ADO source for FACT TABLE 
    }

    [Serializable]
    public class klPortfolioTransform : klAlgorithmTransform
    {
        public string m_xmlFile;

        klPortfolioTransform(string xmlFile)
        {
            m_xmlFile = xmlFile;
        }

        //makes a call into kldll - runs portfolio analysis on single flat blob -
        public override RenderedItem getData(object key)
        {
            klDLLXMLRI ri = new klDLLXMLRI();

            //ri.m_xmlResults=klDLLImports.klRunWorkFlow2(m_xmlFile);

            return ri;
        }

    }





}