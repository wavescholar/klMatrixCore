using System;
using System.Threading;
using System.Collections;
using System.IO;
using System.Data;
using System.Data.SqlClient; 
using System.Messaging;
using System.Runtime.InteropServices;

using klWorkFlow;
namespace klWorkFlow
{

	public class klDLLImports
	{
		//[DllImport("kldll.dll", EntryPoint="#3",CharSet=CharSet.Auto,CallingConvention=CallingConvention.Cdecl)] public static extern void klMatlabWrapper(String xmlFile);
		[DllImport("kldll.dll", EntryPoint="#3",CharSet=CharSet.Auto,CallingConvention=CallingConvention.Cdecl)] public static extern void klRunWorkFlow();
		//[DllImport("kldll.dll", EntryPoint="#3",CharSet=CharSet.Auto,CallingConvention=CallingConvention.Cdecl)] public static extern String klRunWorkFlow2(String xmlFile);
		//[DllImport("klqldll.dll", EntryPoint="#3",CharSet=CharSet.Auto,CallingConvention=CallingConvention.Cdecl)] public static extern void klQuantlibWrapper(string xmlFile);

	}

	public class klDataHost
	{
		public Hashtable m_HostIdMapping;
		public Hashtable m_DBMappings;
		//public Hashtable m_uidMapping;
		//public Hashtable m_PwdMapping;


		public klDataHost()
		{
			m_HostIdMapping=new Hashtable();
			m_DBMappings=new Hashtable();
			//m_uidMapping=new Hashtable();
			//m_PwdMapping=new Hashtable();
		}
		
		public SqlConnection getSQLClientConnection(Object key)
		{
			String connectionString="";

			System.Data.SqlClient.SqlConnection DBConnection=new SqlConnection(connectionString);

			return DBConnection;
		}
		
	}
	
	
	public class klMessageHost
	{
		public Hashtable m_HostIdMapping;
		public Hashtable m_MessageQueueMappings;
		//public Hashtable m_uidMapping;
		//public Hashtable m_PwdMapping;

		public klMessageHost()
		{
			m_HostIdMapping=new Hashtable();
			m_MessageQueueMappings=new Hashtable();
			//m_uidMapping=new Hashtable();
			//m_PwdMapping=new Hashtable();
		}
		
		
		MessageQueue getMessageQueue(Object key)
		{
			MessageQueue messageQueue= new System.Messaging.MessageQueue();
			return messageQueue;

		}

	}
		
	//An analysis has resources, an algorithm graph, and a run method which renders the graph
	public class klAnalysis :MarshalByRefObject
	{
		
	
		//All of HostID's in the data host container must be represented in the messaging resources.
		public klMessageHost m_messageHostResources;

		public klAlgorithmGraph m_AlgoGraph;

		public virtual void run()
		{

		}
				
	}


	//Representes an analysis contained entirely on a single machine running SQL Server 
	public class klSQLServerAnalysis : klAnalysis
	{
		public klDataHost m_dataHostResources;

		public new  void run()
		{

		}


	}
	
	
	//Represents an analysis on one of the grid resources
	public class klMQAnalysis : klAnalysis
	{
		public new  void run()
		{

		}

	}



	//Data Descriptors describe the data inside a rendered item.  They are used
	//as keys in the rendering and caching mechanisms. Data descriptors contain symbol,
	//time, and scale parameters for all rendered items [ flat files, blobs, reports, movies, ... etc]
	public class klDataDesc
	{

	}
	public class klBufferDesc: klDataDesc
	{

	}
	public class klMRTimeDesc : klDataDesc
	{

	}
	public class klModelParamDesc :klDataDesc
	{

	}

	[Serializable]
	public class klModel :MarshalByRefObject
	{
		public klAnalysis m_analysis;

		public klDataDesc m_dataDesc;
        
		//Analysis results - blobs,reports, movies, flat files, RI's are
		//further rendered in the model.  This includes realtime algorithmmics, 
		//running a classifier trainined in the analysis, aggregating several analysis
		//and UI output
		public virtual void run()
		{
			//Default behvaior - typically this is overridden in a derived class
			//where model parameters are used to run in realtime or classification mode
			//
		}
	}
	
	//Represents distributed analysis resources.  
	//This object gets used by driver classes
	//One analysis object gets created by the user, and the
	//drivers are resposible for calling render on the parts of the 
	//analysis chain that it can render.
	public class klDTCAnalysisResources 
	{

		//Deploys the workflow elements {server,grid} on the Cluster

		public ArrayList m_Servers;//klSQLServerAnalysis
		public ArrayList m_GridElements;//klMQAnalysis

		public String m_xmlFile;//workflow graph

		public klDTCAnalysisResources()
		{
			m_Servers= new	ArrayList();
			
			m_GridElements=new ArrayList();
		}
	}

	public class klAlgorithmGraph
	{
		public Hashtable m_Sources;
		public Hashtable m_Sinks;
		public Hashtable m_Transforms;

		public klAlgorithmGraph()
		{
			m_Sources=new Hashtable();
			m_Sinks=new Hashtable();
			m_Transforms=new Hashtable();
		}
		
	}

	public abstract class klTransform :MarshalByRefObject
	{
		public abstract RenderedItem getData(Object key);

		public string persist()
		{
			return "";

		}

		private string m_Name;
		public string getName()
		{
			return m_Name;
		}
		public void setName(string name)
		{
			m_Name=name;
		}

	}
	[Serializable]
	public class RenderedItem
	{
		protected string m_Key;

		public RenderedItem()
		{
			m_Key="";
		}


		public string getKey()
		{
			return m_Key;
		}
        
	}

	public class ICP
	{
		public  OCP m_OCP;
		public RenderedItem getData(object key)
		{
			return m_OCP.getData( key);
		}
		public void connect (OCP src)
		{
			m_OCP=src;
		}
	}

	public class OCP
	{
		//Rendered item container.
		private Hashtable m_Cache;

		public void setCache(string key)
		{
			RenderedItem ri=new RenderedItem();
			m_Cache.Add(key,ri);//ri is filled later.
		}


		private klTransform m_xform;
		public void setTransform(klTransform xform)
		{
			m_xform=xform;
		}

		public RenderedItem getData(object key)
		{
			RenderedItem ri=m_xform.getData(key);
			
			String lkey=ri.getKey();

			if(m_Cache[lkey]!=null)
				m_Cache[lkey]=ri;
			
			return ri;
		}
		

		public OCP()
		{
			m_Cache=new Hashtable();
		}
	}

	public class klAlgorithmDataSource :klTransform
	{
		public OCP m_OCP;
		
		public klAlgorithmDataSource()
		{
			m_OCP=new OCP();
			m_OCP.setTransform(this);
		}


		public  override RenderedItem getData(object key)
		{
			//The beginning of all real algorithmics. 
			RenderedItem ri=new RenderedItem();

			return ri;

		}
	}

	public class klAlgorithmDataSink : klTransform
	{
		
		
		public ICP m_ICP;

		public klAlgorithmDataSink()
		{
			m_ICP=new ICP();
		}

		
		//Fills the RenderItems.
		public void generateDependencies()
		{
		}

		public  override  RenderedItem getData(object key)
		{
			//Sink get data
			RenderedItem ri= m_ICP.getData(key);
			return ri;
		}

		public virtual void render()
		{
			string key="";
			RenderedItem ri = getData(key);

		}

		public  void MultiThreadedRender()
		{

		}
		//a priority list of all the renderables required to generate output.
		private Hashtable RenderItems;


	}

	public class klAlgorithmTransform : klTransform
	{
		public ICP m_ICP;
		public OCP m_OCP;
		
		public klAlgorithmTransform()
		{
			m_ICP=new ICP();
			
			m_OCP=new OCP();
			
			m_OCP.setTransform(this);
		}

		public override RenderedItem getData(object key)
		{
			RenderedItem ri=m_ICP.getData(key);
			return ri;
		}

	}

	
	public class klMatlabAlgorithm : klAlgorithmTransform
	{
		public string m_script;

		void runScript(string script)
		{
			//klDLLImports.klMatlabWrapper(m_script);
		}
		
		public override RenderedItem getData(object key)
		{
			RenderedItem ri=m_ICP.getData(key);
			return ri;
		}

	}

	public class klRAlgorithm : klAlgorithmTransform
	{
		public override RenderedItem getData(object key)
		{
			RenderedItem ri=m_ICP.getData(key);
			return ri;
		}

	}

	public class klLocalTransform : klAlgorithmTransform
	{
		//Secondary ICP - 
		public ICP m_ICP2;

		public klLocalTransform() :base()
		{
			m_ICP2=new ICP();
			m_OCP2=new OCP();
			m_OCP2.setTransform(this);


					
		}

		//Secondary OCP

		public OCP m_OCP2;

		public override RenderedItem getData(object key)
		{
			//We need to know which ocp was  used to make this call if multiple outputs are allowed!

			//Get inputs 
			RenderedItem ri1=m_ICP2.getData(key);

			RenderedItem ri2=m_ICP.getData(key);

			//Do our stuff 

			RenderedItem ri=new RenderedItem();

			return ri;

		}

				


	}

	public class klADOSource :klAlgorithmDataSource
	{		
		public System.Data.DataSet m_DataSet;

		public System.Data.SqlClient.SqlDataAdapter m_DataAdapter;
		
		public System.Data.SqlClient.SqlConnection m_sqlConnection;

		public override RenderedItem getData(object key)
		{
			//
			System.Console.WriteLine("GET DATA:");
			
			RenderedItem ri =new RenderedItem();

			return ri;
		}


	}

	public class klFileSource : klAlgorithmDataSource
	{
		public klFileSource()
		{

		}

		public void setFileName(string name)
		{
			m_FileName=name;
		}

		public klFileSource(string name)
		{
			m_FileName=name;
		}

		public override RenderedItem getData(object key)
		{
			//
			System.Console.WriteLine("GET DATA:");
			
			RenderedItem ri =new RenderedItem();

			return ri;
		}


		protected string  m_FileName;


	}

	public class klFileListSource : klAlgorithmDataSource
	{
		public klFileListSource() 
		{
			

		}


		public override RenderedItem getData(object key)
		{
			klFileListRI ri =new klFileListRI();

			return ri;
		}


	}


	public class klADOSink : klAlgorithmDataSink
	{
		public System.Data.DataSet m_DataSet;

		public System.Data.SqlClient.SqlDataAdapter m_DataAdapter;

	}

	public class klFileSink : klAlgorithmDataSink
	{
		
		public klFileSink(String filename)
		{
			m_fileName=filename;
		}
		
		private String m_fileName;
	}

	public class klEquityInstance :RenderedItem
	{

	}
	

	[Serializable]
	public class klDLLXMLRI :RenderedItem
	{
		public string m_xmlResults;
	}

	
	[Serializable]
	public class klFileListRI :RenderedItem
	{
		public string m_baseFileName;

		public ArrayList m_files;

	}



    [Serializable]
    public class klUnitTestFn : klAlgorithmTransform
    {
        public string m_xmlFile;

        klUnitTestFn(string xmlFile)
        {
            m_xmlFile = xmlFile;
        }

        //makes a call into kldll - runs portfolio analysis on single flat blob -
        public override RenderedItem getData(object key)
        {
            klDLLXMLRI ri = new klDLLXMLRI();

            ri.m_xmlResults = null;
            
            //Just call main driver for now
            klDLLImports.klRunWorkFlow();

            return ri;
        }

    }



	[Serializable]
	public class klDFTAggTransform : klAlgorithmTransform
	{
		//Aggs FFT sec data in basic windows. Corr calculated by inner prod. alg. 

		public string m_xmlFile;

		klDFTAggTransform(string xmlFile)
		{
			m_xmlFile=xmlFile;
		}

		//makes a call into kldll - runs portfolio analysis on single flat blob -
		public override RenderedItem getData(object key)
		{
			klDLLXMLRI ri=new klDLLXMLRI();
			
			//ri.m_xmlResults=klDLLImports.klRunWorkFlow2(m_xmlFile);

			return ri;
		}
 
	}

	[Serializable]
	public class klCorrelationMovieTransform : klMatlabAlgorithm
	{

	}

	//For Realtime algorithms - see klTownsendAnalytics
}

namespace testWorkflow
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	public class klWorkFlowFactory
	{

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		public static void runTestWorkflow(string[] args)
		{
			klMessageHost klmh=new klMessageHost();

			klmh.m_HostIdMapping.Add(0,"grid00");
			klmh.m_MessageQueueMappings.Add(0,"grid00_MQ");
			
			klmh.m_HostIdMapping.Add(1,"grid01");
			klmh.m_MessageQueueMappings.Add(1,"grid00_MQ");
			
			klmh.m_HostIdMapping.Add(2,"grid02");
			klmh.m_MessageQueueMappings.Add(2,"grid02_MQ");
			
			klmh.m_HostIdMapping.Add(3,"IWM");
			klmh.m_MessageQueueMappings.Add(3,"IWM_MQ");
			
			klmh.m_HostIdMapping.Add(4,"SPY");
			klmh.m_MessageQueueMappings.Add(4,"SPY_MQ");
			
			klmh.m_HostIdMapping.Add(5,"OIH");
			klmh.m_MessageQueueMappings.Add(5,"OIH_MQ");
			
			klmh.m_HostIdMapping.Add(6,"tickerplant");
			klmh.m_MessageQueueMappings.Add(6,"tickerplant_MQ");
		
			klmh.m_HostIdMapping.Add(7,"dtchost");
			klmh.m_MessageQueueMappings.Add(7,"dtchost_MQ");
			
			klDataHost kldh=new klDataHost();

			kldh.m_DBMappings.Add(0,"OIH");
			kldh.m_HostIdMapping.Add(0,"OIH");
			
			kldh.m_DBMappings.Add(1,"IWM");
			kldh.m_HostIdMapping.Add(1,"IWM");
			
			kldh.m_DBMappings.Add(2,"SPY");
			kldh.m_HostIdMapping.Add(2,"SPY");
			
			
			klMQAnalysis kla=new klMQAnalysis();
			kla.m_messageHostResources=klmh;  //Something on grid03
			
			klSQLServerAnalysis klssa=new klSQLServerAnalysis();
			klssa.m_dataHostResources=kldh;  //something on shuttle


			klDTCAnalysisResources kldtc=new klDTCAnalysisResources();
			kldtc.m_GridElements.Add(klmh);
			kldtc.m_Servers.Add(kldh);

			//Algorithm Graph 
			klAlgorithmGraph klag=new klAlgorithmGraph();
			//sources
			klADOSource kladosrc=new klADOSource();
			kladosrc.setName(kladosrc.GetType().FullName);
			
			klFileSource klfsrc=new klFileSource("in.txt");
			klfsrc.setName(klfsrc.GetType().FullName);
			//transforms
			klMatlabAlgorithm klma =new klMatlabAlgorithm();
			klma.setName(klma.GetType().FullName);

			klRAlgorithm klra=new klRAlgorithm();
			klra.setName(klra.GetType().FullName);
			//sinks
			klFileSink klfsnk=new klFileSink("moo.txt");
			klfsnk.setName(klfsnk.GetType().FullName);

			klADOSink kladosnk=new klADOSink();
			kladosnk.setName(kladosnk.GetType().FullName);
            
			klLocalTransform kllt=new klLocalTransform();	
			kllt.setName(kllt.GetType().FullName);
			klag.m_Sources.Add(0,klfsrc);
			klag.m_Sources.Add(1,kladosrc);

			klag.m_Transforms.Add(0,klma);
			klag.m_Transforms.Add(1,klra);

			klag.m_Transforms.Add(2,kllt);

			klag.m_Sinks.Add(0,klfsnk);

			klag.m_Sinks.Add(1,kladosnk);
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
}