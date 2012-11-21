
using System;
using System.Threading;
using System.Collections;
using TalTrade.Toolkit;
using TalTrade.Toolkit.ClientAdapter;
using BenTools.Data;
using System.IO;
using System.Data;
using System.Data.SqlClient; 
using System.ComponentModel;
using System.Xml;
using System.Xml.Serialization;
using System.Messaging;
using klSymbolCalculus;

using klWorkFlow;

namespace klTownsendAnalytics
{	
 
	//klLiveQuoteEquityTickFileSource used to be a sub-class of klLiveDataApp;
	//There is one procressor per connection.  
	//Multiple Async queries can be attached to the V2 Application
	//Change service (TA_SRV ACCOUNT_GATEWAY NEWSSERV)
	//and topic (LIVECHART LIVE_DATA) 
	//to get additional connections to tal server via klTALStreamAttributes.   
	//Multiple AsyncQureies can be attached to a connection.  These are maintaied in the 
	//HashTable in the klLiveQuoteEquityTickFileSource associated with each connection 
	//Use AsyncQuery.Request for Synchronous requests.
	//Derive from AyncQuery and write a new delegate method to direct the stream to different places.
	//see klAsyncQuery klAsyncVTKStreamQuery klAsyncADOStreamQuery
		
	public class klTALStreamAttributes
	{
		private string m_szMachine;
		private string m_szService;
		private string m_szTopic;
		private string m_szItem;
		   	
		private StreamWriter m_sw;

		

		public klTALStreamAttributes(string szMachine, string szService, string szTopic, string szItem, StreamWriter sw)
		{
			m_szMachine = szMachine;
			m_szService = szService;
			m_szTopic = szTopic;
			m_szItem = szItem;
			m_sw = sw;
		}

		public klTALStreamAttributes(string szMachine, string szService, string szTopic, string szItem)
		{
			m_szMachine = szMachine;
			m_szService = szService;
			m_szTopic = szTopic;
			m_szItem = szItem;
			m_sw = null;
		}

			

		public string Machine()
		{
			return(m_szMachine);
		}

		public StreamWriter myStreamWriter()
		{
			return m_sw;
		}


		public string Service()
		{
			return(m_szService);
		}
		public string Topic()
		{
			return(m_szTopic);
		}
		public string Item()
		{
			return(m_szItem);
		}
			


	}
	

	//Set these base class handlers as needed.
	//TalTrade.Toolkit.AsyncQuery.OnAdviseData 
	//TalTrade.Toolkit.AsyncQuery.OnExecuteAck 
	//TalTrade.Toolkit.AsyncQuery.OnOtherAck 
	//TalTrade.Toolkit.AsyncQuery.OnRequestData 
	//TalTrade.Toolkit.AsyncQuery.OnTerminate
	public class klAsyncQuery : AsyncQuery
	{
		private klTALStreamAttributes m_SA;

		public void setStreamAttributes( klTALStreamAttributes SA)
		{
			m_SA=SA;
				


		}

		private StreamWriter m_SW;

		private Hashtable m_TQL_FID_Persist;
		//klTALStreamAttributes stores the TQL 
		//The business end of the query is TABLE;WhatFID's,WhereFID's
		//add set/get metods.
		public Hashtable m_WhereFID; 
		public Hashtable m_WhatFID;

		//For QUOTECHAIN queries - this gives the total number of option stmbols returned from 
		public int m_howMany;

		//FUNDAMENTAL fact table string - sets the table to put the fundamental data in
		public string m_FactTableName;

		public AutoResetEvent m_ARE;

		//Make attribute - or impl set/get
		public bool m_bReadyForData;

		public klAsyncQuery(klTALStreamAttributes SA)
		{
			m_SA=SA;
			m_SW= m_SA.myStreamWriter();
		}



		//bypasses the stream attributes/writer mechanism.
		//Typically for use with synchornous calls to the TQL servers.
		public klAsyncQuery(Hashtable WhatFID,Hashtable WhereFID)
		{
			m_WhereFID=WhereFID;
			m_WhatFID=WhatFID;
		}

		public Hashtable myWhatFID()
		{
			return m_WhatFID;
		}

		public Hashtable myWhereFID()
		{
			return m_WhereFID;
		}



		public string myTQL()
		{
			return m_SA.Item();
		}

		public StreamWriter myStreamWriter()
		{
			return m_SW;
		}

		public void setMyStreamWriter(StreamWriter lsw)
		{
			m_SW=lsw;
		}

		//Set the items we would like the query to persist.
		//Some kl queries that filter realtime will persis a subset of the data so
		//there is a container of the TQL what FIDs and where FID's this query will be persisting

		public Hashtable myTQLPersist()
		{
			return m_TQL_FID_Persist;
		}

		public void setTQLPersist(Hashtable sp)
		{
			m_TQL_FID_Persist=sp;

		}


		public void ThreadMain()
		{
			System.Console.WriteLine(myTQL());

		}




	}



		

	//This is the TAL application class for iocp pooled message processing 
	//of live TAL stream data.  This is designed to be used to connect to the LIVEQUOTE TAL topoic.
	//It uses the klLiveQuoteEquityTickFileSource class to 
	//
	public class klTALApplication : ClientAdapterToolkitApp
	{


		private Hashtable m_TALConnections;
		private Hashtable m_DataProcessors;
		private bool m_bRunning;

		private long GetTALConnectionKey(string szMachine, string szService, string szTopic)
		{
			return(szMachine.GetHashCode() ^ szService.GetHashCode() ^ szTopic.GetHashCode());
				
		}

		private long GetDataProcessorKey(Connection con)
		{
			return(con.GetHashCode());
		}
			
		public klLiveQuoteEquityTickFileSource FindMyLiveDataProcessor(Connection con)
		{
			long lKey = GetDataProcessorKey(con);

			klLiveQuoteEquityTickFileSource ldp;

			if (m_DataProcessors.Contains(lKey))
			{
				ldp = (klLiveQuoteEquityTickFileSource) m_DataProcessors[lKey];
			}
			else
			{
					
				ldp = new klLiveQuoteEquityTickFileSource();
				m_DataProcessors.Add(lKey, ldp);
			}
			return(ldp);
		}

			
		public Connection FindMyConnection(string szMachine, string szService, string szTopic)
		{
			long lKey = GetTALConnectionKey(szMachine, szService, szTopic);
			Connection con;

			if (m_TALConnections.Contains(lKey))
			{
				con = (Connection) m_TALConnections[lKey];
			}
			else
			{
				//This is where we connect to one onf the servers.
				//Change service (TA_SRV ACCOUNT_GATEWAY NEWSSERV)
				//and topic (LIVECHART LIVE_DATA) 
				con = GetConnection(szMachine, szService, szTopic);
				m_TALConnections.Add(lKey, con);
			}
			return(con);
		}

		public klTALApplication()
		{
			m_TALConnections = new Hashtable();
			m_DataProcessors = new Hashtable();
			m_bRunning = true;
		}

		public bool IsRunning()
		{
			return m_bRunning;
		}	
			
		//Main Driver Routine for File Source 
		public  void LIVEQUOTEFileSourceQuery(ArrayList symbols,Hashtable WhatFID,	Hashtable WhereFID)
		{
			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
				DateTime d1 = DateTime.Now;

				string fileName = (string)symbols[symbolCount];
				string fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+=fileDate;
				fileName+="_RAW_TICK_STREAM.txt";
				StreamWriter lsw =new StreamWriter(fileName);
				
				//Set the symbol element of the TVP 
				TQLTypeValuePair tvp=(TQLTypeValuePair)WhereFID[1003];
				tvp.m_TQLValue=(string)symbols[symbolCount];

				string lTQL=TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);

				klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL,lsw);	

				ConnectAndSubscribe(tsb);
				Thread.Sleep(300);

			}


		}

		
		public void ConnectAndSubscribe(klTALStreamAttributes tsa)
		{
			Connection con = FindMyConnection(tsa.Machine(), tsa.Service(), tsa.Topic());

			//Instantiate the right derivative in V2
			StreamWriter lsw=tsa.myStreamWriter();

			//Again - instantiate the right derivative in V2
			klLiveQuoteEquityTickFileSource ldp = FindMyLiveDataProcessor( con);

			klAsyncQuery q = new klAsyncQuery(tsa);
			con.AttachQuery(q);

			ldp.addQuery(q,tsa.Item());

			AutoResetEvent hData = new AutoResetEvent(false);

			string TQL=tsa.Item();

			long lKey=TQL.GetHashCode();//(TQL);

			ldp.Add(lKey,hData);

			Dequeue AQ_DataContainer = new Dequeue();

			ldp.Add(lKey,AQ_DataContainer);

							
		}

		public static string TQLfromWhatWhereFIDHashes(string TOPIC, Hashtable WhereFID,Hashtable WhatFID)
		{
			string lTQL=TOPIC;
			lTQL+=";";

			IDictionaryEnumerator whatIdx=WhatFID.GetEnumerator();
	
			for(int i=0;i<WhatFID.Count;i++)
			{
				whatIdx.MoveNext();
				int lKey=(int)whatIdx.Key;

				lTQL+=lKey;
				if(i!=WhatFID.Count-1)
					lTQL+=", ";
			}
			lTQL+=";";

			IDictionaryEnumerator whereIdx=WhereFID.GetEnumerator();

			for(int i=0;i<WhereFID.Count;i++)
			{
				whereIdx.MoveNext();
				
				int lKey=(int)whereIdx.Key;

				TQLTypeValuePair tvp=(TQLTypeValuePair)WhereFID[lKey];

				lTQL+= lKey + "=" + tvp.m_TQLValue;

				if(i!=WhereFID.Count-1)
					lTQL+=", ";
			}
			

			return lTQL;
		}


	}


	
	
	public class TQLTypeValuePair
	{	
		
		public TQLTypeValuePair(string TQLType,string TQLValue)
		{ 
			m_TQLType=TQLType;
			m_TQLValue=TQLValue;
		}
		public string m_TQLType; 
		public string m_TQLValue;
	}

		

	public class klLiveQuoteEquityFundamentalsADOSource : klADOSource
	{
		//Requires a what fid hastable, the FUNDAMENTAL table has no where fid other than symbol 1003
		public void LIVEQUOTE_FUNDAMENTAL_MakeFactTable(string tableName ,Hashtable myTQLPersist)
		{		
			IEnumerator pe=myTQLPersist.GetEnumerator();
			String sqlText ="CREATE TABLE [dbo].["+ tableName  +"] ( ";
			sqlText+="DISP_NAME" +" [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL   ,";

			ICollection vpv=myTQLPersist.Values;
			IEnumerator epv=vpv.GetEnumerator();
			ICollection kpv=myTQLPersist.Keys;
			IEnumerator epk=kpv.GetEnumerator();
			for(int op=0;op<myTQLPersist.Count;op++)
			{ 	
				//This is what we sent to the column builder along with the type.
				pe.MoveNext();
				Type mt= pe.GetType();
				epv.MoveNext();
				string fid=(string)epv.Current;

				epk.MoveNext();
				int key=(int)epk.Current;

				string colType="";
				//We look up the type from  klSymbolCalculus.TALFIDS using the persist hashtable key.
				for(int j=0;j<klSymbolCalculus.TALFIDS.nFundamentalIDs;j++)
				{
					if(klSymbolCalculus.TALFIDS.fundamentalFIDs[j,0]==key.ToString())
						colType=klSymbolCalculus.TALFIDS.fundamentalFIDs[j,2];
				}
				int ival=0;
				double dval=0;
					
				//Only two types of fid in the FUNDAMENTALS table 
				if(colType=="LONG")
				{
					sqlText+=fid +" [int] NULL ,";
					//ival= row[fid].IntValue;
				}
				if(colType=="DOUBLE")
				{
					sqlText+=fid +" [float] NULL ,";
					//dval=row[fid].DoubleValue;
				}

			}
			sqlText+=") ON [PRIMARY]";
				
			//bbcrevisit - this is a parameter of class
			SqlConnection klDBConn = m_sqlConnection;
			//new SqlConnection("workstation id=DUALCOREP4;packet size=8192;user id=sa;data source=i;persist security info=False;initial catalog=kl");
			//"Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");

			SqlCommand catCMD = klDBConn.CreateCommand();
					
			catCMD.CommandText = sqlText;
			
			try
			{
				klDBConn.Open();
				int result  = catCMD.ExecuteNonQuery();
			}
			catch(Exception exception)
			{
				string msg=exception.Message;
				System.Console.WriteLine(msg);
			}
			klDBConn.Close();

			///////////////////////////////////////END MAKE TABLE GET COL VALS
		}

		//Handles a single query
		public  void LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(klTALApplication a,	Hashtable whatFID, Hashtable whereFID,string factTableName)
		{
			string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("FUNDAMENTAL",whereFID,whatFID);

			klTALStreamAttributes tsb_sync = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL);	
				
			klAsyncQuery laqq=new klAsyncQuery(tsb_sync);
				
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			Hashtable sp=new Hashtable();
			
			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVEQUOTE_FUNDAMENTAL_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEQUOTE_FUNDAMENTAL_OnTerminate);

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;

			//Set the fact table name so the ado callback knows were to put the data
			laqq.m_FactTableName=factTableName;

			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked
			Thread.Sleep(3000);
		}
			
			
		//This is hard coded to 
		public  void LIVEQUOTE_FUNDAMENTAL_Callback(object sender, DataEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;	
				
			String TQL=laq.myTQL();
			
			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable myTQLPersist=laq.m_WhatFID;
			
			IEnumerator  ie=row.GetEnumerator();

			IEnumerator pe=myTQLPersist.GetEnumerator();

			String DISP_NAME=row["DISP_NAME"].StringValue;
					
			string tableName=laq.m_FactTableName;

			//bbcrevisit - this is a parameter
			SqlConnection klDBConn = m_sqlConnection;
			//new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
	
			///////////////////////////////////////Set up DataSet, insert SQL , select SQL, column mappings, and find out what the query returned.

			SqlCommand sqlSelectCommand1 = klDBConn.CreateCommand();
			SqlCommand sqlInsertCommand1 = klDBConn.CreateCommand();
			SqlDataAdapter sqlDataAdapter1 = new System.Data.SqlClient.SqlDataAdapter();
			DataSet dataSet1 = new System.Data.DataSet();
			DataTable dataTable1 = new System.Data.DataTable();
			//	((System.ComponentModel.ISupportInitialize)(dataSet1)).BeginInit();
			//	((System.ComponentModel.ISupportInitialize)(dataTable1)).BeginInit();

			sqlInsertCommand1.UpdatedRowSource = System.Data.UpdateRowSource.FirstReturnedRecord;
				
			sqlInsertCommand1.Connection = klDBConn;
			sqlSelectCommand1.Connection = klDBConn;
				
			sqlDataAdapter1.InsertCommand = sqlInsertCommand1;
			sqlDataAdapter1.SelectCommand = sqlSelectCommand1;
			int numberofColumns=myTQLPersist.Count+1;//We set the key property on the symbol column - make sure the What FId hastable does not have 1003 in it
				
			System.Data.Common.DataColumnMapping[] dcm=new System.Data.Common.DataColumnMapping[numberofColumns];
				
			//Firat one - the rest are done in the loop below.
			dcm[0]=new System.Data.Common.DataColumnMapping("DISP_NAME", "DISP_NAME");
																								
				

			sqlDataAdapter1.RowUpdated += new System.Data.SqlClient.SqlRowUpdatedEventHandler(sqlDataAdapter_RowUpdated);
				
			dataSet1.DataSetName = "NewDataSet";
			dataSet1.Locale = new System.Globalization.CultureInfo("en-US");
				
			dataTable1.TableName = tableName;

			// Declare variables for DataColumn and DataRow objects.
			DataColumn myDataColumn;
			DataRow myDataRow;

			//Symbol Column    
			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");
			myDataColumn.ColumnName = "DISP_NAME";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false; 
			dataTable1.Columns.Add(myDataColumn);

			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@DISP_NAME", System.Data.SqlDbType.Char, 255, "DISP_NAME"));
					
				
			ICollection vpv=myTQLPersist.Values;
			IEnumerator epv=vpv.GetEnumerator();
			ICollection kpv=myTQLPersist.Keys;
			IEnumerator epk=kpv.GetEnumerator();
			//epv.Reset();
			//kpv.Reset();
			//epk.Reset();
			//we iterate over the fids again making  select command, insertcommand,  tablemapping and adding data columns
			pe.Reset();

			//These hold the cloumns
			String vals="DISP_NAME, ";
			String atvals="@DISP_NAME, ";
				
			for(int op=0;op<myTQLPersist.Count;op++)
			{ 
				try
				{
					pe.MoveNext();
					epv.MoveNext();
				
					string fid=(string)epv.Current;

					//This adds the colum to the data column mapping.  +1 because we added one for the symb0l fid above.
					dcm[op+1]=new System.Data.Common.DataColumnMapping(fid, fid);

					epk.MoveNext();
					int key=(int)epk.Current;

					string colType="";
					//We look up the type from  klSymbolCalculus.TALFIDS using the persist hashtable key.
					for(int j=0;j<klSymbolCalculus.TALFIDS.nFundamentalIDs;j++)
						if(klSymbolCalculus.TALFIDS.fundamentalFIDs[j,0]==key.ToString())
							colType=klSymbolCalculus.TALFIDS.fundamentalFIDs[j,2];
					int ival=0;
					double dval=0;
					
									
					myDataColumn = new DataColumn();
					myDataColumn.ColumnName = fid;
					myDataColumn.ReadOnly = false;
					myDataColumn.Unique = false;
				
					//Only two types of fid in the FUNDAMENTALS table 
					if(colType=="LONG")
					{	
						myDataColumn.DataType = System.Type.GetType("System.Int32");
								
						try
						{
							ival= row[fid].IntValue;
						}
						catch(Exception exception)
						{
							string msg=exception.Message;
							System.Console.WriteLine(msg);
							ival=0;
						}
						
						sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@"+fid, System.Data.SqlDbType.Int, 255, fid));
					}
					if(colType=="DOUBLE")
					{
						
						myDataColumn.DataType = System.Type.GetType("System.Double");
	
						try
						{
							dval=row[fid].DoubleValue;
						}
						catch(Exception exception)
						{
							string msg=exception.Message;
							System.Console.WriteLine(msg);
							dval=0;
							System.Type fidType = row[fid].GetType();
							System.Console.WriteLine(fidType.Name);

							
							
						}
						
						sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@"+fid, System.Data.SqlDbType.Float, 255, fid));
					}
					dataTable1.Columns.Add(myDataColumn);
	
					//Using fid and TYPE we create the columns
					if(op<myTQLPersist.Count-1)
					{
						vals += fid+", ";
						atvals+="@"+fid+" , ";
					}
					else
					{
						vals += fid+" ";
						atvals+="@"+fid+" ";
					}
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}
			}
			

			string selectCommand="SELECT "+vals+ "FROM "+ tableName;

			string insertCommand="INSERT INTO " +tableName +" (" + vals + ") VALUES (" + atvals +"); "+selectCommand;

			sqlSelectCommand1.CommandText = selectCommand;

			sqlInsertCommand1.CommandText = insertCommand;
	
			System.Data.Common.DataTableMapping[] dtm = new System.Data.Common.DataTableMapping[1];
			try
			{
				dtm[0]=new System.Data.Common.DataTableMapping(tableName, tableName,dcm);
				sqlDataAdapter1.TableMappings.AddRange(dtm);
			}
			catch(Exception exception)
			{
				string msg=exception.Message;
				System.Console.WriteLine(msg);
			}		
				
			//dataTable1.AcceptChanges();

			DataColumn[] PrimaryKeyColumns = new DataColumn[1];
			PrimaryKeyColumns[0] = dataTable1.Columns["DISP_NAME"];
			dataTable1.PrimaryKey = PrimaryKeyColumns;

			string dataExp=dataTable1.DisplayExpression;
				
			//dataTable1.AcceptChanges();
			dataSet1.Tables.AddRange(new System.Data.DataTable[] {dataTable1});
			//dataSet1.AcceptChanges();
			string xmlExp=dataSet1.GetXml();
			string expSchema=dataSet1.GetXmlSchema();

			DataTable dtt=dataSet1.Tables[dataTable1.TableName];		

			///////////The data dable and column mappigs are complete.  Now fill the data table with a row.
			//Here we fill the DataTable
			DataRow workRow;
			epv.Reset();
			epk.Reset();
			//we iterate over the fids again making  select command, insertcommand,  tablemapping and adding data columns
			pe.Reset();

			workRow=dataTable1.NewRow();
				
			workRow[0] =row["DISP_NAME"].StringValue;
	
			//Start at 1 since the first col is the symbol key.
			for(int op=1;op<=myTQLPersist.Count;op++)
			{ 
				try
				{
					pe.MoveNext();
					epv.MoveNext();
				
					string fid=(string)epv.Current;
						
					epk.MoveNext();
					int key=(int)epk.Current;

					string colType="";
					//We look up the type from  klSymbolCalculus.TALFIDS using the persist hashtable key.
					for(int j=0;j<klSymbolCalculus.TALFIDS.nFundamentalIDs;j++)
						if(klSymbolCalculus.TALFIDS.fundamentalFIDs[j,0]==key.ToString())
							colType=klSymbolCalculus.TALFIDS.fundamentalFIDs[j,2];
														
					//Only two types of fid in the FUNDAMENTALS table 
					if(colType=="LONG")
					{	
						try
						{
							workRow[op] =row[fid].IntValue;
								
						}
						catch(Exception exception)
						{
							string msg=exception.Message;
							System.Console.WriteLine(msg);
							//workRow[op]=0;
						}
					}
					if(colType=="DOUBLE")
					{
						try
						{
							workRow[op] =row[fid].DoubleValue;
						}
						catch(Exception exception)
						{
							string msg=exception.Message;
							System.Console.WriteLine(msg);
							workRow[op]=0;
							System.Type fidType = row[fid].GetType();
							System.Console.WriteLine(fidType.Name);
						}
					}
						
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}
			}

			dataTable1.Rows.Add(workRow);
			DataRowState rw=dataTable1.Rows[0].RowState;
			//dataTable1.Rows[i].AcceptChanges();
	
			//((System.ComponentModel.ISupportInitialize)(dataSet1)).EndInit();
			//((System.ComponentModel.ISupportInitialize)(dataTable1)).EndInit();
			try
			{	
				int  status = sqlDataAdapter1.Update(dataSet1,dataTable1.TableName);
				SqlCommandBuilder cb = new SqlCommandBuilder(sqlDataAdapter1);
				SqlCommand cbInsert=cb.GetInsertCommand();
				//SqlCommand cbUpdate=cb.GetUpdateCommand();
				//SqlCommand myUpdateCommand= sqlDataAdapter1.UpdateCommand;
				//sqlDataAdapter1.UpdateCommand=cbUpdate;
					

	
				System.Console.WriteLine(status);
			}
			catch(Exception exception)
			{
				string msg=exception.Message;
				System.Console.WriteLine(msg);
			}
				
			klDBConn.Close();
	
		}


		public  void LIVEQUOTE_FUNDAMENTAL_OnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		//This method does not require a where fid hastable.
		//The FUNDAMENTAL table only need symbol in the were portion of TQL string 
		public void LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(klTALApplication a,Hashtable FUNDAMENTALS_WhatFID_Hash, ArrayList symbols,string tableName )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");
			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
			
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();
		
				Hashtable FUNDAMENTALS_WhereFID_Hash=new Hashtable();
				FUNDAMENTALS_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));

				string fundamentalsTQL=klTALApplication.TQLfromWhatWhereFIDHashes("FUNDAMENTALS", FUNDAMENTALS_WhereFID_Hash ,FUNDAMENTALS_WhatFID_Hash);

				klTALStreamAttributes tsb_sync = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",fundamentalsTQL);	
				
				klAsyncQuery fundamentalQuery=new klAsyncQuery(FUNDAMENTALS_WhatFID_Hash,FUNDAMENTALS_WhereFID_Hash);
				fundamentalQuery.setStreamAttributes(tsb_sync);
				fundamentalQuery.m_FactTableName=tableName;
				
				AsyncQuery.DataHandler primeDBHandler=new AsyncQuery.DataHandler(LIVEQUOTE_FUNDAMENTAL_Callback);
				AsyncQuery.TerminateHandler primeDBCallback=new AsyncQuery.TerminateHandler(LIVEQUOTE_FUNDAMENTAL_OnTerminate);

				fundamentalQuery.OnAdviseData += primeDBHandler;
				fundamentalQuery.OnRequestData += primeDBHandler;
				fundamentalQuery.OnTerminate += primeDBCallback;
				
				fundamentalQuery.m_FactTableName=tableName;
				livechartConnection.AttachQuery(fundamentalQuery);
                    				
				//This is a hateful way to do it - fix this. 
				//AutoResetEvent hData =new AutoResetEvent(true);
				//dailyQuery.m_ARE=hData;
				fundamentalQuery.Request(fundamentalsTQL+" ","TAL4");
				//hData.Reset();
				Thread.Sleep(200);
					
								
			}

		}

			
		private void sqlDataAdapter_RowUpdated(object sender, System.Data.SqlClient.SqlRowUpdatedEventArgs e)
		{
			//
		
		}


	}


	public class klLiveChartEquityADOSource : klADOSource
	{

		///TQL Notes:
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS
		//Change service (TA_SRV ACCOUNT_GATEWAY NEWSSERV)
		//and topic (LIVECHART LIVE_DATA) 
		//Suppose you wanted to retrieve 1 day of trades for IBM. Your TQL request would read: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM' 
		//If you request data at times just before or just after the open, you may get an additional day of data
		//																								   
		//due to the way TA_SRV caches data before writing it. As a workaround, use FIDs 9009 and 9010 to 
		//restrict the times for the chart. The following query would restrict data to the date specified, between 9:30 AM and 4:00 PM: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM',9009='09:30',9010='16:00'


		//Where FIDs: 
		//1003- ticker symbol search condition 
		//9008 - stop date condition 
		//9009 - chart start time 
		//9010 - chart stop time 
		//
		//The TICKS table stores data for 7 calendar days back from the current day, but you may only request 1 day of 
		//TICKS data at a time. The default day is today. If you specify a different day in 9008, that day will be returned instead. 
		//The day you specify in 9008 can be up to 7 calendar days back from the current day. 
		//
		//You may use 24-hour or minutes-from-midnight time formats in FIDs 9009 and 9010. Additionally, FIDs 9009 and 9010 
		//may also be used in queries against the INTRADAY table.
		//
		//If you want to see the before-market or after-market trades on NASDAQ stocks, add FID 6040 to the WHAT clause, and
		//specify times before/after the market. All before/after-market trades will have a 20 in the 6040 FID, indicating that 
		//it is a FormT trade. If you wish to only see updates on the before/after-market FormT trades, make your code advise on 
		//the string with 6040, SALE_CONDITION, in the WHAT clause.


		//Map of FID's for TICS WHAT FID's
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY
		//What FIDs:
		//FID	Label	Type	Ticks	Intraday	Daily	Retrieves
		//1032	ACVOL_1	LONG	 	x	x	Session's total trading volume for a particular instrument 
		//1416	CONTRIB_1	INT	    x	 	 	Contributor (ForEx data only, in place of TRDXID_1)
		//1003	DISP_NAME	STRING	x	x	x	Symbol name for instrument
		//1012	HIGH_1	LPACK	 	x	x	This session's highest transaction value
		//1013	LOW_1	LPACK	 	x	x	This session's lowest transaction value
		//6025	MONEY_FLOW	LONG	 	 	x	Money flow (stocks only, in place of OPIN1)
		//1077	NUM_MOVES	INT	 	x	 	Number of trades this session for a particular instrument
		//1019	OPEN_PRC	LPACK	 	x	x	This session's opening price
		//1070	SETTLE	LPACK	 	x	x	Settlement price. The official closing price of a commodity
		//6040   SALE_CONDITION	INT	x	 	 	Sale Conditions for Nasdaq
		//9020    TICK_TYPE	INT	    x	 	 	Quote condition indicator
		//1016	TRD_DATE	DATE	x	x	x	Trade Date
		//1044	TRDXID_1	STRING	x	 	 	Exchange Identifier of the latest trade (U.S. composite stock only)
		//1006	TRDPRC_1	LPACK	x	 	 	Last trade price or value
		//1018	TRDTIM_1	TIME	x	x	x	Time of the last reported trade (listed in TRDPRC_1)
		//1178	TRDVOL_1	LONG	x	 	 	Transactional volume of the last trade price reported in TRDPRC_1
		//Where FID's for TIC's  - these are used to set up XML file for dataset transaction
		//Where FIDS:
		//FID	Label	Type	Ticks	Intraday	Daily	Format
		//9011	CHART_ALL_TICKS	BOOL	x	 	 	(Returns trades, bids and asks in the TRDPRC_1 field)
		//9005    CHART_BAR_INTERVAL	INT	 	x	x	'0' (daily)
		//9012	CHART_FILL_BARS	BOOL	 	x	x	Fills gaps between bars
		//9006	STARTONBOUND	BOOL	 	x	x	Starts bars at beginning of session (used for odd interval bars)
		//9007	CHART_STARTDATE	DATE	 	x	x	'1996/01/01' (TICKS table can request only one day at a time. The desired date is indicated in CHART_STOPDATE).
		//9009	CHART_STARTTIME	TIME	x	x	 	'14:00'
		//9008	CHART_STOPDATE	DATE	x	x	x	'1996/01/15'
		//9010	CHART_STOPTIME	TIME	x	x	 	'14:00'
		//9004	DAYS_BACK	INT	 	    x	x	'5' (number of days back for INTRADAY, and daily bars. Number of weeks or months back for weekly or monthly bar data.)
		//1003	DISP_NAME	STRING	    x	x	x	'IBM' (Symbol formats are datafeed-specific. Check your Feed Handler help file for the correct format.)
		//1016	TRD_DATE	DATE	    x	x	x	'1996/01/15'
		//1018	TRDTIM_1	TIME	    x	x	x	'14:00'
		//1044	TRDXID_1	STRING	    x	 	 	'N'

		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY 
		//FID = 9020 codes only are valid in the WHAT clause:
		//Description Code 
		//Trade 0 
		//Bid 1 
		//Ask 2 
		//Regional bid  3 
		//Regional ask 4 
		//Deleted 11 
		//Inserted 12 
		//Irregular Delete 44 
		//FormT Trade 32 




		private string m_datahost ;
		private string m_initCatalog;
		private string m_userid; 
		private string m_pwd;
		public void setDBInfo(string datahost, string initCatalog,string userid, string pwd)
		{				
			m_datahost=datahost ;
			m_initCatalog=initCatalog;
			m_userid=userid; 
			m_pwd=pwd;
		}


		//This method handles a single synchronous query the callback is hard coded to handle tick data.
		//It does a thread wait at the end so the callback has time to be invoked.
		public  void LIVECHARTSynchronousADOSinkQuery(klTALApplication a,string lTQL)
		{
			klTALStreamAttributes tsb_qqqq_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL);	
			klAsyncQuery laqq=new klAsyncQuery(tsb_qqqq_sync);
				
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			Hashtable sp=new Hashtable();
			sp.Add(1006,"TRDPRC_1");
			sp.Add(1016,"TRD_DATE");
			sp.Add(1178,"TRDVOL_1");
			sp.Add(1018, "TRDTIM_1");
			laqq.setTQLPersist(sp);
			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_OnTerminate);

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;
			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked
			Thread.Sleep(250);


		}
		private void sqlDataAdapter_RowUpdated(object sender, System.Data.SqlClient.SqlRowUpdatedEventArgs e)
		{

		}
		
		public  void LIVECHART_Callback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			
			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable myTQLPersist=laq.myTQLPersist();
			
			IEnumerator  ie=row.GetEnumerator();

			IEnumerator persistTags=myTQLPersist.GetEnumerator();
		
			for(int op=0;op<myTQLPersist.Count;op++)
			{ 
				persistTags.MoveNext();
				Type mt= persistTags.GetType();

				//TQLTypeValuePair tag=(TQLTypeValuePair)persistTags.Current;
				// tag=()persistTags.Current;
				//string fid=(string)myTQLPersist[tag];
			}
			int elements = 0;

			if (row["TRDPRC_1"] != null)
				elements = row["TRDPRC_1"].PriceVectorValue.Length;
			
			int first_element=0;
			
			//here we take out our vectors

			Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
			int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
			TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
			DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
			String DISP_NAME=row["DISP_NAME"].StringValue;
			//We have the data.  Now open a connection to the database and transact the data to the appropriate place.

			//OLEDB ADO - MS says use SQL NET provider instead
			//but I believe we will need the OLE DB provider for DTC later
			//
			//OleDbConnection dbConn = new OleDbConnection("Provider=SQLOLEDB;Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
			//OleDbCommand catCMD = dbConn.CreateCommand();
			//Create a table based on TQL persist tags.
			//catCMD.CommandText = "SELECT CategoryID, CategoryName FROM Categories";
			//dbConn.Open();
			//OleDbDataReader myReader = catCMD.ExecuteReader();
			//while (myReader.Read())
			//{
			//	Console.WriteLine("\t{0}\t{1}", myReader.GetInt32(0), myReader.GetString(1));
			//}
			//myReader.Close();
			//dbConn.Close();

			SqlConnection klDBConn = new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
			SqlCommand catCMD = klDBConn.CreateCommand();

			string tableName = DISP_NAME+"_TICKS";
			string sqlText = 

				"CREATE TABLE [dbo].["+ tableName  +"] ( "+
				"[index] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,"+
				"[TRD_DATE] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
				"[TRDTIM_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
				"[TRDPRC_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
				"[TRDVOL_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
				") ON [PRIMARY]";

					
			catCMD.CommandText = sqlText;
			klDBConn.Open();
			try
			{
				int result  = catCMD.ExecuteNonQuery();
			}
			catch(Exception exception)
			{
				string msg=exception.Message;
				System.Console.WriteLine(msg);
			}

			SqlCommand sqlSelectCommand1 = klDBConn.CreateCommand();//)new System.Data.SqlClient.SqlCommand();
			SqlCommand sqlInsertCommand1 = klDBConn.CreateCommand();//new System.Data.SqlClient.SqlCommand();
			SqlDataAdapter sqlDataAdapter1 = new System.Data.SqlClient.SqlDataAdapter();
			DataSet dataSet1 = new System.Data.DataSet();
			DataTable dataTable1 = new System.Data.DataTable();
			//	((System.ComponentModel.ISupportInitialize)(dataSet1)).BeginInit();
			//	((System.ComponentModel.ISupportInitialize)(dataTable1)).BeginInit();

			// sqlSelectCommand1
			// 
			sqlSelectCommand1.CommandText = "SELECT [index], TRD_DATE, TRDTIM_1, TRDPRC_1, TRDVOL_1 FROM QQQQ_TICKS";
			sqlSelectCommand1.Connection = klDBConn;
			// 
			// sqlConnection1
			// 
			//	klDBConn.ConnectionString = "workstation id=SHUTTLE;packet size=4096;integrated security=SSPI;data source=SHUT" +
			//		"TLE;persist security info=True;initial catalog=kl";
			// 
			// sqlInsertCommand1
			// TRD_DATE TRDTIM_1 TRDPRC_1 TRDVOL_1
			sqlInsertCommand1.CommandText = "INSERT INTO QQQQ_TICKS (index, TRD_DATE, TRDTIM_1, TRDPRC_1, TRDVOL_1" +
				") VALUES (@index, @TRD_DATE, @TRDTIM_1, @TRDPRC_1, @TRDVOL_1); SELECT index, TRD_DATE, " +
				"TRDTIM_1, TRDPRC_1, TRDVOL_1 FROM QQQQ_TICKS";
			sqlInsertCommand1.Connection = klDBConn;
			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@index", System.Data.SqlDbType.VarChar, 255, "index"));
			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRD_DATE", System.Data.SqlDbType.VarChar, 255, "TRD_DATE"));
			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRDTIM_1", System.Data.SqlDbType.VarChar, 255, "TRDTIM_1"));
			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRDPRC_1", System.Data.SqlDbType.VarChar, 255, "TRDPRC_1"));
			sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRDVOL_1", System.Data.SqlDbType.VarChar, 255, "TRDVOL_1"));
			sqlInsertCommand1.UpdatedRowSource = System.Data.UpdateRowSource.FirstReturnedRecord;



			// 

			// sqlDataAdapter1
			// 
			sqlDataAdapter1.InsertCommand = sqlInsertCommand1;
			sqlDataAdapter1.SelectCommand = sqlSelectCommand1;





			//				sqlDataAdapter1.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
			//																									 new System.Data.Common.DataTableMapping("Table", "ISBC_DAILY__3_22_2007", new System.Data.Common.DataColumnMapping[] {
			//																																																							  new System.Data.Common.DataColumnMapping("Col001", "Col001"),																																								   new System.Data.Common.DataColumnMapping("Col002", "Col002"),
			//																																																							  new System.Data.Common.DataColumnMapping("Col003", "Col003"),
			//																																																							  new System.Data.Common.DataColumnMapping("Col004", "Col004"),
			//																																																							  new System.Data.Common.DataColumnMapping("Col005", "Col005"),
			//																																																							  new System.Data.Common.DataColumnMapping("Col006", "Col006")
			//																																																						  }
			//																									 )
			//																								 }
			//					);
			//				
			sqlDataAdapter1.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
																								 new System.Data.Common.DataTableMapping(tableName, tableName, new System.Data.Common.DataColumnMapping[] {
																																																			  new System.Data.Common.DataColumnMapping("index", "index"),
																																																			  new System.Data.Common.DataColumnMapping("TRD_DATE", "TRD_DATE"),
																																																			  new System.Data.Common.DataColumnMapping("TRDTIM_1", "TRDTIM_1"),
																																																			  new System.Data.Common.DataColumnMapping("TRDPRC_1", "TRDPRC_1"),
																																																			  new System.Data.Common.DataColumnMapping("TRDVOL_1", "TRDVOL_1")
																																																		  }
																								 )
																							 }
				);
			//				
				
			//Unused for now
			sqlDataAdapter1.RowUpdated += new System.Data.SqlClient.SqlRowUpdatedEventHandler(sqlDataAdapter_RowUpdated);
				
			// 
			// dataSet1
			// 
			dataSet1.DataSetName = "NewDataSet";
			dataSet1.Locale = new System.Globalization.CultureInfo("en-US");
				
			// 
			// dataTable1
			// 
			dataTable1.TableName = tableName;

			// Declare variables for DataColumn and DataRow objects.
			DataColumn myDataColumn;
			DataRow myDataRow;
 
			// Create new DataColumn, set DataType, ColumnName and add to DataTable.   
			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
			myDataColumn.ColumnName = "index";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false;
			// Add the Column to the DataColumnCollection.
			dataTable1.Columns.Add(myDataColumn);


			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
			myDataColumn.ColumnName = "TRD_DATE";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false;
			// Add the Column to the DataColumnCollection.
			dataTable1.Columns.Add(myDataColumn);
 
			// Create second column.
			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
			myDataColumn.ColumnName = "TRDTIM_1";
			myDataColumn.AutoIncrement = false;
			myDataColumn.Caption = "TRDTIM_1";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false;
			// Add the column to the table.
			dataTable1.Columns.Add(myDataColumn);

			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");//Decimal");
			myDataColumn.ColumnName = "TRDPRC_1";
			myDataColumn.AutoIncrement = false;
			myDataColumn.Caption = "TRDPRC_1";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false;
			// Add the column to the table.
			dataTable1.Columns.Add(myDataColumn);

			myDataColumn = new DataColumn();
			myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
			myDataColumn.ColumnName = "TRDVOL_1";
			myDataColumn.AutoIncrement = false;
			myDataColumn.Caption = "TRDVOL_1";
			myDataColumn.ReadOnly = false;
			myDataColumn.Unique = false;
			// Add the column to the table.
			dataTable1.Columns.Add(myDataColumn);


			//dataTable1.AcceptChanges();

			// Make the ID column the primary key column.
			DataColumn[] PrimaryKeyColumns = new DataColumn[1];
			PrimaryKeyColumns[0] = dataTable1.Columns["index"];
			dataTable1.PrimaryKey = PrimaryKeyColumns;



			//Here we fill the DataTable
			DataRow workRow;
			for (int i = first_element; i < elements; i++)
			{
				try
				{
					workRow=dataTable1.NewRow();
					//						dataTable1.Rows.Add(new Object[]
					//{
					//	TRD_DATE[i].ToString("yyyy/MM/dd"),
					//	TRDTIM_1[i].Hours.ToString("D2")+TRDTIM_1[i].Minutes.ToString("D2"),
					//	TRDPRC_1[i].ToString(),
					//							TRDVOL_1[i].ToString()
					//
					//});//.NewRow();
					workRow[0]=i.ToString();
					workRow[1] = TRD_DATE[i].ToString("yyyy/MM/dd");
					workRow[2] = TRDTIM_1[i].Hours.ToString("D2")+TRDTIM_1[i].Minutes.ToString("D2");
					workRow[3] =TRDPRC_1[i].ToString();
					workRow[4] =TRDVOL_1[i].ToString();
					dataTable1.Rows.Add(workRow);
					DataRowState rw=dataTable1.Rows[i].RowState;
					//dataTable1.Rows[i].AcceptChanges();

				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}

						
			}
			string dataExp=dataTable1.DisplayExpression;
				
			//dataTable1.AcceptChanges();
			dataSet1.Tables.AddRange(new System.Data.DataTable[] {dataTable1});
			//dataSet1.AcceptChanges();
			string xmlExp=dataSet1.GetXml();
			string expSchema=dataSet1.GetXmlSchema();

			DataTable dtt=dataSet1.Tables[dataTable1.TableName];		

			//((System.ComponentModel.ISupportInitialize)(dataSet1)).EndInit();
			//((System.ComponentModel.ISupportInitialize)(dataTable1)).EndInit();
			try
			{
				SqlCommandBuilder cb = new SqlCommandBuilder(sqlDataAdapter1);
				//For cases where the SelectCommand is dynamically specified at runtime, such as through a query tool that takes a textual command from the user, 
				//you may not be able to specify the appropriate InsertCommand, UpdateCommand, or DeleteCommand at design time. 
				//If your DataTable maps to or is generated from a single database table, you can take advantage of the CommandBuilder object to automatically generate the DeleteCommand, InsertCommand, and UpdateCommand of the DataAdapter.
				//
				//As a minimum requirement, you must set the SelectCommand property in order for automatic command generation to work. The table schema retrieved by the SelectCommand determines the syntax of the automatically generated INSERT, UPDATE, and DELETE statements.
				//
				//The CommandBuilder must execute the SelectCommand in order to return the metadata necessary to construct the insert, update, and delete commands. As a result, an extra trip to the data source is necessary which can hinder performance. To achieve optimal performance, specify your commands explicitly rather than using the CommandBuilder.

				SqlCommand cbUpdate=cb.GetUpdateCommand();
				SqlCommand myUpdateCommand= sqlDataAdapter1.UpdateCommand;
				sqlDataAdapter1.UpdateCommand=cbUpdate;
				SqlCommand cbInsert=cb.GetInsertCommand();

					




				//int status = sqlDataAdapter1.Fill(dataSet1,dataTable1.TableName);
				int  status = sqlDataAdapter1.Update(dataSet1,dataTable1.TableName);
				System.Console.WriteLine(status);
			}
			catch(Exception exception)
			{
				string msg=exception.Message;
				System.Console.WriteLine(msg);
			}



			//Create a DataSet and a DataTable to put in it.
				
			klDBConn.Close();

	
		}


		public  void LIVECHART_OnTerminate(object sender, EventArgs e)
		{
		}


		//Tic Data is cached in local file system - then transacted to db in one shot.
		public  void LIVECHART_PrimeDBCallback(object sender, DataEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;	
	
			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable whatFID=laq.myWhatFID();//
			Hashtable whereFID=laq.myWhereFID();//

			//From TICKS table
			//			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			//			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44

			if(	whatFID.ContainsValue("TICK_TYPE"))
			{ //Only tick data uses this fid
				Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
				int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
				int elements = 0;
				
				//					StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				//					Monitor.Enter(sw);
				//
				if (row["TRDPRC_1"] != null)
					elements = row["TRDPRC_1"].PriceVectorValue.Length;
				//				
				int first_element=0;

				String connectionString ="Data Source="+ m_datahost +";User ID= "+ m_userid+"; Password= "+m_pwd+";Initial Catalog= "+ m_initCatalog;
				SqlConnection klDBConn = new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
					//new SqlConnection(connectionString);
				try 
				{
					klDBConn.Open();

									   
				}
				catch(Exception exc)
				{
					string mas=exc.Message;
					System.Console.WriteLine(mas);
				}

				klDBConn.Close();







			}

			//From INTRADAY TABLE
			//			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			//			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			//			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");
			if(	whatFID.ContainsValue("NUM_MOVES"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] NUM_MOVES = row["NUM_MOVES"] == null ? null : row["NUM_MOVES"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;
				int first_element=0;

				String DISP_NAME=row["DISP_NAME"].StringValue;

				//string m_datahost ;string m_initCatalog;string m_userid; string m_pwd;
				String connectionString ="Data Source="+ m_datahost +";User ID= "+ m_userid+" ;Password = "+ m_pwd+";Initial Catalog= "+ m_initCatalog;
				SqlConnection klDBConn =  new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
					//new SqlConnection(connectionString);
					

				//OPEN_PRC HIGH_1 LOW_1 SETTLE NUM_MOVES  TRDTIM_1 TRD_DATE
				////     Create Table
				SqlCommand catCMD = klDBConn.CreateCommand();
				string tableName = DISP_NAME+"_INTRADAY";
				string sqlText = 		"CREATE TABLE [dbo].["+ tableName  +"] ( "+
					"[OPEN_PRC] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[HIGH_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[LOW_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[SETTLE] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					"[NUM_MOVES] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					"[TRDTIM_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					"[TRD_DATE] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  "+
					") ON [PRIMARY]";
				catCMD.CommandText = sqlText;
				klDBConn.Open();
				try
				{
					int result  = catCMD.ExecuteNonQuery();
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}

				SqlCommand sqlSelectCommand1 = klDBConn.CreateCommand();
				SqlCommand sqlInsertCommand1 = klDBConn.CreateCommand();
				SqlDataAdapter sqlDataAdapter1 = new System.Data.SqlClient.SqlDataAdapter();
				DataSet dataSet1 = new System.Data.DataSet();
				DataTable dataTable1 = new System.Data.DataTable();
				//	((System.ComponentModel.ISupportInitialize)(dataSet1)).BeginInit();
				//	((System.ComponentModel.ISupportInitialize)(dataTable1)).BeginInit();

				sqlSelectCommand1.CommandText = "SELECT OPEN_PRC, HIGH_1, LOW_1, SETTLE, NUM_MOVES,  TRDTIM_1, TRD_DATE FROM " +tableName;
				sqlSelectCommand1.Connection = klDBConn;

				sqlInsertCommand1.CommandText = "INSERT INTO " +tableName + "  (OPEN_PRC, HIGH_1, LOW_1, SETTLE, NUM_MOVES,  TRDTIM_1, TRD_DATE)" +
					"VALUES (@OPEN_PRC, @HIGH_1, @LOW_1, @SETTLE, @NUM_MOVES,  @TRDTIM_1, @TRD_DATE ); "+
					"SELECT OPEN_PRC, HIGH_1, LOW_1, SETTLE, NUM_MOVES,  TRDTIM_1, TRD_DATE FROM " + tableName;
				sqlInsertCommand1.Connection = klDBConn;
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@OPEN_PRC", System.Data.SqlDbType.VarChar, 255, "OPEN_PRC"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@HIGH_1", System.Data.SqlDbType.VarChar, 255, "HIGH_1"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@LOW_1", System.Data.SqlDbType.VarChar, 255, "LOW_1"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@SETTLE", System.Data.SqlDbType.VarChar, 255, "SETTLE"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@NUM_MOVES", System.Data.SqlDbType.VarChar, 255, "NUM_MOVES"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRDTIM_1", System.Data.SqlDbType.VarChar, 255, "TRDTIM_1"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRD_DATE", System.Data.SqlDbType.VarChar, 255, "TRD_DATE"));
				sqlInsertCommand1.UpdatedRowSource = System.Data.UpdateRowSource.FirstReturnedRecord;

				sqlDataAdapter1.InsertCommand = sqlInsertCommand1;
				sqlDataAdapter1.SelectCommand = sqlSelectCommand1;
				
				sqlDataAdapter1.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
																									 new System.Data.Common.DataTableMapping(tableName, tableName, new System.Data.Common.DataColumnMapping[] {
																																																				  new System.Data.Common.DataColumnMapping("OPEN_PRC", "OPEN_PRC"),
																																																				  new System.Data.Common.DataColumnMapping("HIGH_1", "HIGH_1"),
																																																				  new System.Data.Common.DataColumnMapping("LOW_1", "LOW_1"),
																																																				  new System.Data.Common.DataColumnMapping("SETTLE", "SETTLE"),
																																																				  new System.Data.Common.DataColumnMapping("NUM_MOVES", "NUM_MOVES"),
																																																				  new System.Data.Common.DataColumnMapping("TRDTIM_1", "TRDTIM_1"),
																																																				  new System.Data.Common.DataColumnMapping("TRD_DATE", "TRD_DATE")
																																																			  }
																									 )
																								 }
					);
			
				
				//Unused for now
				sqlDataAdapter1.RowUpdated += new System.Data.SqlClient.SqlRowUpdatedEventHandler(sqlDataAdapter_RowUpdated);
				
				dataSet1.DataSetName = "NewDataSet";
				dataSet1.Locale = new System.Globalization.CultureInfo("en-US");
				
				dataTable1.TableName = tableName;
					
				DataColumn myDataColumn;
				//OPEN_PRC, HIGH_1, LOW_1, SETTLE, NUM_MOVES,  TRDTIM_1, TRD_DATE
				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "OPEN_PRC";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "HIGH_1";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "LOW_1";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "LOW_1";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//Decimal");
				myDataColumn.ColumnName = "SETTLE";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "SETTLE";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
				myDataColumn.ColumnName = "NUM_MOVES";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "NUM_MOVES";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
				myDataColumn.ColumnName = "TRDTIM_1";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "TRDTIM_1";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
				myDataColumn.ColumnName = "TRD_DATE";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "TRD_DATE";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				dataTable1.Columns.Add(myDataColumn);

				//dataTable1.AcceptChanges();

				// Make the ID column the primary key column.
				//DataColumn[] PrimaryKeyColumns = new DataColumn[1];
				//PrimaryKeyColumns[0] = dataTable1.Columns["index"];
				//dataTable1.PrimaryKey = PrimaryKeyColumns;

				//Here we fill the DataTable
				DataRow workRow;
				for (int i = first_element; i < elements; i++)
				{
					try
					{
						workRow=dataTable1.NewRow();
						//OPEN_PRC, HIGH_1, LOW_1, SETTLE, NUM_MOVES,  TRDTIM_1, TRD_DATE
						workRow[0] = OPEN_PRC[i].ToString();
						workRow[1] = HIGH_1[i].ToString();
						workRow[2] =LOW_1[i].ToString();
						workRow[3] =SETTLE[i].ToString();
						workRow[4] =NUM_MOVES[i].ToString();
						workRow[5] =TRDTIM_1[i].ToString();
						workRow[6]=TRD_DATE[i].ToString("yyyy/MM/dd");
							
						dataTable1.Rows.Add(workRow);
						DataRowState rw=dataTable1.Rows[i].RowState;
						//dataTable1.Rows[i].AcceptChanges();

					}
					catch(Exception exception)
					{
						string msg=exception.Message;
						System.Console.WriteLine(msg);
					}

						
				}
				string dataExp=dataTable1.DisplayExpression;
				
				//dataTable1.AcceptChanges();
				dataSet1.Tables.AddRange(new System.Data.DataTable[] {dataTable1});
				//dataSet1.AcceptChanges();
				string xmlExp=dataSet1.GetXml();
				string expSchema=dataSet1.GetXmlSchema();

				DataTable dtt=dataSet1.Tables[dataTable1.TableName];		

				//((System.ComponentModel.ISupportInitialize)(dataSet1)).EndInit();
				//((System.ComponentModel.ISupportInitialize)(dataTable1)).EndInit();
				try
				{
					//SqlCommandBuilder cb = new SqlCommandBuilder(sqlDataAdapter1);
					//For cases where the SelectCommand is dynamically specified at runtime, such as through a query tool that takes a textual command from the user, 
					//you may not be able to specify the appropriate InsertCommand, UpdateCommand, or DeleteCommand at design time. 
					//If your DataTable maps to or is generated from a single database table, you can take advantage of the CommandBuilder object to automatically generate the DeleteCommand, InsertCommand, and UpdateCommand of the DataAdapter.
					//
					//As a minimum requirement, you must set the SelectCommand property in order for automatic command generation to work. The table schema retrieved by the SelectCommand determines the syntax of the automatically generated INSERT, UPDATE, and DELETE statements.
					//
					//The CommandBuilder must execute the SelectCommand in order to return the metadata necessary to construct the insert, update, and delete commands. As a result, an extra trip to the data source is necessary which can hinder performance. To achieve optimal performance, specify your commands explicitly rather than using the CommandBuilder.

					//SqlCommand cbUpdate=cb.GetUpdateCommand();
					//SqlCommand myUpdateCommand= sqlDataAdapter1.UpdateCommand;
					//sqlDataAdapter1.UpdateCommand=cbUpdate;
					//SqlCommand cbInsert=cb.GetInsertCommand();

					//int status = sqlDataAdapter1.Fill(dataSet1,dataTable1.TableName);
					int  status = sqlDataAdapter1.Update(dataSet1,dataTable1.TableName);
					System.Console.WriteLine(status);
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}
				//Create a DataSet and a DataTable to put in it.
				
				klDBConn.Close();


			}
			


			//From Daily Hashtable
			//			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			//			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			//			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");
			if(	whatFID.ContainsValue("MONEY_FLOW"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] MONEY_FLOW = row["MONEY_FLOW"] == null ? null : row["MONEY_FLOW"].IntVectorValue;//Money Flow = (Settle-Open)*Volume
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;
				int first_element=0;

				String DISP_NAME=row["DISP_NAME"].StringValue;

				String connectionString ="Data Source="+ m_datahost +";User ID= "+ m_userid+" ;Password = "+ m_pwd+";Initial Catalog= "+ m_initCatalog;
				SqlConnection klDBConn =  new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
					//new SqlConnection(connectionString);
					
				////     Create Table
				SqlCommand catCMD = klDBConn.CreateCommand();
				string tableName = DISP_NAME+"_DAILY";
				string sqlText = 		"CREATE TABLE [dbo].["+ tableName  +"] ( "+
					"[TRD_DATE] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[OPEN_PRC] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[HIGH_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL  , "+
					"[LOW_1] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					"[SETTLE] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					"[MONEY_FLOW] [char] (10) COLLATE SQL_Latin1_General_CP1_CI_AS NULL , "+
					") ON [PRIMARY]";
				catCMD.CommandText = sqlText;
					
				try
				{
					klDBConn.Open();
					int result  = catCMD.ExecuteNonQuery();
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}

				SqlCommand sqlSelectCommand1 = klDBConn.CreateCommand();//)new System.Data.SqlClient.SqlCommand();
				SqlCommand sqlInsertCommand1 = klDBConn.CreateCommand();//new System.Data.SqlClient.SqlCommand();
				SqlDataAdapter sqlDataAdapter1 = new System.Data.SqlClient.SqlDataAdapter();
				DataSet dataSet1 = new System.Data.DataSet();
				DataTable dataTable1 = new System.Data.DataTable();
				//	((System.ComponentModel.ISupportInitialize)(dataSet1)).BeginInit();
				//	((System.ComponentModel.ISupportInitialize)(dataTable1)).BeginInit();

				// sqlSelectCommand1
				// 
				sqlSelectCommand1.CommandText = "SELECT TRD_DATE,OPEN_PRC,HIGH_1,LOW_1,SETTLE,MONEY_FLOW FROM " +tableName;
				sqlSelectCommand1.Connection = klDBConn;
				// 
				// sqlConnection1
				// 
				//	klDBConn.ConnectionString = "workstation id=SHUTTLE;packet size=4096;integrated security=SSPI;data source=SHUT" +
				//		"TLE;persist security info=True;initial catalog=kl";
				// 
				// sqlInsertCommand1
				// TRD_DATE TRDTIM_1 TRDPRC_1 TRDVOL_1
				sqlInsertCommand1.CommandText = "INSERT INTO " +tableName + "  (TRD_DATE,OPEN_PRC,HIGH_1,LOW_1,SETTLE,MONEY_FLOW)" +
					"VALUES (@TRD_DATE,@OPEN_PRC,@HIGH_1,@LOW_1,@SETTLE,@MONEY_FLOW ); "+
					"SELECT TRD_DATE,OPEN_PRC,HIGH_1,LOW_1,SETTLE,MONEY_FLOW FROM " + tableName;
				sqlInsertCommand1.Connection = klDBConn;
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@TRD_DATE", System.Data.SqlDbType.VarChar, 255, "TRD_DATE"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@OPEN_PRC", System.Data.SqlDbType.VarChar, 255, "OPEN_PRC"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@HIGH_1", System.Data.SqlDbType.VarChar, 255, "HIGH_1"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@LOW_1", System.Data.SqlDbType.VarChar, 255, "LOW_1"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@SETTLE", System.Data.SqlDbType.VarChar, 255, "SETTLE"));
				sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@MONEY_FLOW", System.Data.SqlDbType.VarChar, 255, "MONEY_FLOW"));
				sqlInsertCommand1.UpdatedRowSource = System.Data.UpdateRowSource.FirstReturnedRecord;

				sqlDataAdapter1.InsertCommand = sqlInsertCommand1;
				sqlDataAdapter1.SelectCommand = sqlSelectCommand1;
				
				sqlDataAdapter1.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
																									 new System.Data.Common.DataTableMapping(tableName, tableName, new System.Data.Common.DataColumnMapping[] {
																																																				  new System.Data.Common.DataColumnMapping("TRD_DATE", "TRD_DATE"),
																																																				  new System.Data.Common.DataColumnMapping("OPEN_PRC", "OPEN_PRC"),
																																																				  new System.Data.Common.DataColumnMapping("HIGH_1", "HIGH_1"),
																																																				  new System.Data.Common.DataColumnMapping("LOW_1", "LOW_1"),
																																																				  new System.Data.Common.DataColumnMapping("SETTLE", "SETTLE"),
																																																				  new System.Data.Common.DataColumnMapping("MONEY_FLOW", "MONEY_FLOW")
																																																			  }
																									 )
																								 }
					);
				//				
				
				//Unused for now
				sqlDataAdapter1.RowUpdated += new System.Data.SqlClient.SqlRowUpdatedEventHandler(sqlDataAdapter_RowUpdated);
				
				// 
				// dataSet1
				// 
				dataSet1.DataSetName = "NewDataSet";
				dataSet1.Locale = new System.Globalization.CultureInfo("en-US");
				
				// 
				// dataTable1
				// 
				dataTable1.TableName = tableName;

				// Declare variables for DataColumn and DataRow objects.
				DataColumn myDataColumn;
				DataRow myDataRow;
 
				// Create new DataColumn, set DataType, ColumnName and add to DataTable.   
				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "TRD_DATE";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the Column to the DataColumnCollection.
				dataTable1.Columns.Add(myDataColumn);


				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "OPEN_PRC";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the Column to the DataColumnCollection.
				dataTable1.Columns.Add(myDataColumn);

				// Create second column.
				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//DateTime");
				myDataColumn.ColumnName = "HIGH_1";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "HIGH_1";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the column to the table.
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//Decimal");
				myDataColumn.ColumnName = "LOW_1";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "LOW_1";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the column to the table.
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
				myDataColumn.ColumnName = "SETTLE";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "SETTLE";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the column to the table.
				dataTable1.Columns.Add(myDataColumn);

				myDataColumn = new DataColumn();
				myDataColumn.DataType = System.Type.GetType("System.String");//.Decimal");
				myDataColumn.ColumnName = "MONEY_FLOW";
				myDataColumn.AutoIncrement = false;
				myDataColumn.Caption = "MONEY_FLOW";
				myDataColumn.ReadOnly = false;
				myDataColumn.Unique = false;
				// Add the column to the table.
				dataTable1.Columns.Add(myDataColumn);


				//dataTable1.AcceptChanges();

				// Make the ID column the primary key column.
				//DataColumn[] PrimaryKeyColumns = new DataColumn[1];
				//PrimaryKeyColumns[0] = dataTable1.Columns["index"];
				//dataTable1.PrimaryKey = PrimaryKeyColumns;



				//Here we fill the DataTable
				DataRow workRow;
				for (int i = first_element; i < elements; i++)
				{
					try
					{
						workRow=dataTable1.NewRow();
						//TRD_DATE,OPEN_PRC,HIGH_1,LOW_1,SETTLE,MONEY_FLOW
						workRow[0]=TRD_DATE[i].ToString("yyyy/MM/dd");
						workRow[1] = OPEN_PRC[i].ToString();
						workRow[2] = HIGH_1[i].ToString();
						workRow[3] =LOW_1[i].ToString();
						workRow[4] =SETTLE[i].ToString();
						workRow[5] =MONEY_FLOW[i].ToString();
						dataTable1.Rows.Add(workRow);
						DataRowState rw=dataTable1.Rows[i].RowState;
						//dataTable1.Rows[i].AcceptChanges();

					}
					catch(Exception exception)
					{
						string msg=exception.Message;
						System.Console.WriteLine(msg);
					}

						
				}
				string dataExp=dataTable1.DisplayExpression;
				
				//dataTable1.AcceptChanges();
				dataSet1.Tables.AddRange(new System.Data.DataTable[] {dataTable1});
				//dataSet1.AcceptChanges();
				string xmlExp=dataSet1.GetXml();
				string expSchema=dataSet1.GetXmlSchema();

				DataTable dtt=dataSet1.Tables[dataTable1.TableName];		

				//((System.ComponentModel.ISupportInitialize)(dataSet1)).EndInit();
				//((System.ComponentModel.ISupportInitialize)(dataTable1)).EndInit();
				try
				{
					//SqlCommandBuilder cb = new SqlCommandBuilder(sqlDataAdapter1);
					//For cases where the SelectCommand is dynamically specified at runtime, such as through a query tool that takes a textual command from the user, 
					//you may not be able to specify the appropriate InsertCommand, UpdateCommand, or DeleteCommand at design time. 
					//If your DataTable maps to or is generated from a single database table, you can take advantage of the CommandBuilder object to automatically generate the DeleteCommand, InsertCommand, and UpdateCommand of the DataAdapter.
					//
					//As a minimum requirement, you must set the SelectCommand property in order for automatic command generation to work. The table schema retrieved by the SelectCommand determines the syntax of the automatically generated INSERT, UPDATE, and DELETE statements.
					//
					//The CommandBuilder must execute the SelectCommand in order to return the metadata necessary to construct the insert, update, and delete commands. As a result, an extra trip to the data source is necessary which can hinder performance. To achieve optimal performance, specify your commands explicitly rather than using the CommandBuilder.

					//SqlCommand cbUpdate=cb.GetUpdateCommand();
					//SqlCommand myUpdateCommand= sqlDataAdapter1.UpdateCommand;
					//sqlDataAdapter1.UpdateCommand=cbUpdate;
					//SqlCommand cbInsert=cb.GetInsertCommand();

					




					//int status = sqlDataAdapter1.Fill(dataSet1,dataTable1.TableName);
					int  status = sqlDataAdapter1.Update(dataSet1,dataTable1.TableName);
					System.Console.WriteLine(status);
				}
				catch(Exception exception)
				{
					string msg=exception.Message;
					System.Console.WriteLine(msg);
				}



				//Create a DataSet and a DataTable to put in it.
				
				klDBConn.Close();

			}
	

	
			//}//end lock scope


			//		
		}

						 	
		public  void LIVECHART_PrimeDBOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		public void LIVECHARTSynchronousADOSinkQuery(klTALApplication a, ArrayList symbols )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");

			//Here we create hash tables for What and Where FID's for the types of ADO transactions.
			//TIC, INTRA,DAILY
			Hashtable TICKS_WhatFID_Hash=new Hashtable();
			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			TICKS_WhatFID_Hash.Add(1006,"TRDPRC_1");
			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");

			//TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44
			
			Hashtable INTRADAY_WhatFID_Hash=new Hashtable();
			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");

			Hashtable DAILY_WhatFID_Hash=new Hashtable();
			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			DAILY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");

					

			//For each symbol we make queries for 5days tic, 15days 5min, 2000 daily
			//the tic queries are cached until all five days are available - then the tic table is created.

			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
				//Get todays date - then get 5days tic, 15days 5min, 5000 daily.
				Hashtable TICKS_WhereFID_Hash=new Hashtable();
				
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();

				TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash.Add(9008,tvp);//For tic data only the end date is used to get the data
		 
				tvp=new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'");
				TICKS_WhereFID_Hash.Add(1003,tvp);


				Hashtable INTRADAY_WhereFID_Hash=new Hashtable();
				INTRADAY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				INTRADAY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				//INTRADAY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//INTRADAY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//INTRADAY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");
				INTRADAY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","15"));
				INTRADAY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","5"));//5min bars




				Hashtable DAILY_WhereFID_Hash=new Hashtable();
				DAILY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				DAILY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				DAILY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","5000"));
				DAILY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","0"));//0 for daily
				//DAILY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//DAILY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//DAILY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");

				//We need to construct the TQL for each query - the persist, what and where FID Hashes have been
				//set up at this point.  The query can expand on the FID's, but it can not leave any out.
				string dailyTQL=klTALApplication.TQLfromWhatWhereFIDHashes("DAILY", DAILY_WhereFID_Hash ,DAILY_WhatFID_Hash);

				string intradayTQL=klTALApplication.TQLfromWhatWhereFIDHashes("INTRADAY", INTRADAY_WhereFID_Hash ,INTRADAY_WhatFID_Hash);

				string tics1TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
							
				klAsyncQuery dailyQuery=new klAsyncQuery(DAILY_WhatFID_Hash,DAILY_WhereFID_Hash);
			
				klAsyncQuery intradayQuery=new klAsyncQuery(INTRADAY_WhatFID_Hash,INTRADAY_WhereFID_Hash);

				klAsyncQuery day1TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);				
					
				//Change the date in the Where tvp fid hash
				//Date Arithematic
				System.TimeSpan sts=new TimeSpan(7,0,0,0);//days, hours, minutes, and seconds
								
				DateTime d2=d1.Subtract( sts);

				//A DayOfWeek enumerated constant that indicates the day of the week. 
				//This property value ranges from zero, indicating Sunday, to six, indicating Saturday.
				if( d2.DayOfWeek == (DayOfWeek)6 )
					d2=d2.AddDays(2);
				if( d2.DayOfWeek==0)
					d2=d2.AddDays(1);
				date=d2.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics2TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);
				klAsyncQuery day2TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
					

				DateTime d3=d2.AddDays(1);
				if( d3.DayOfWeek==(DayOfWeek)6)
					d3=d3.AddDays(2);
				if(  (d3.DayOfWeek==0 ))
					d3=d3.AddDays(1);
				date=d3.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics3TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day3TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
					
				DateTime d4=d3.AddDays(1);
				if( d4.DayOfWeek==(DayOfWeek)6)
					d4=d4.AddDays(2);
				if(  (d4.DayOfWeek==0 ))
					d4=d4.AddDays(1);
				date=d4.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics4TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day4TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				
				DateTime d5=d4.AddDays(1);
				if( d5.DayOfWeek==(DayOfWeek)6)
					d5=d5.AddDays(2);
				if(  (d5.DayOfWeek==0 ))
					d5=d5.AddDays(1);
				date=d5.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics5TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day5TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
							
				//This is safe if we're running when the US equitiy markets are open.
				//bbcrevisit - protect it later
				DateTime d6=d5.AddDays(1);
				if( d6.DayOfWeek==(DayOfWeek)6)
					d6=d6.AddDays(2);
				if(  (d6.DayOfWeek==0 ))
					d6=d6.AddDays(1);
				date=d6.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics6TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day6TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
					
				
				//There were originally two types of callback used by this method - one that caches tics
				//and one that does the ADO transaction for INTRADAY and DAILY
				//AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_PrimeDBCallback);
				//AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBOnTerminate);


				AsyncQuery.DataHandler kladodhtic=new AsyncQuery.DataHandler(LIVECHART_PrimeDBCallback);
				AsyncQuery.TerminateHandler kladothtic=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBOnTerminate);
				
				dailyQuery.OnAdviseData += kladodhtic;
				dailyQuery.OnRequestData += kladodhtic;
				dailyQuery.OnTerminate += kladothtic;
				
				intradayQuery.OnAdviseData += kladodhtic;
				intradayQuery.OnRequestData += kladodhtic;
				intradayQuery.OnTerminate += kladothtic;
	
				day1TicsQuery.OnAdviseData += kladodhtic;
				day1TicsQuery.OnRequestData += kladodhtic;
				day1TicsQuery.OnTerminate += kladothtic;

				day2TicsQuery.OnAdviseData += kladodhtic;
				day2TicsQuery.OnRequestData += kladodhtic;
				day2TicsQuery.OnTerminate += kladothtic;

				day3TicsQuery.OnAdviseData += kladodhtic;
				day3TicsQuery.OnRequestData += kladodhtic;
				day3TicsQuery.OnTerminate += kladothtic;

				day4TicsQuery.OnAdviseData += kladodhtic;
				day4TicsQuery.OnRequestData += kladodhtic;
				day4TicsQuery.OnTerminate += kladothtic;

				day5TicsQuery.OnAdviseData += kladodhtic;
				day5TicsQuery.OnRequestData += kladodhtic;
				day5TicsQuery.OnTerminate += kladothtic;

				day6TicsQuery.OnAdviseData += kladodhtic;
				day6TicsQuery.OnRequestData += kladodhtic;
				day6TicsQuery.OnTerminate += kladothtic;


				//Add the TICKS queries
				livechartConnection.AttachQuery(day1TicsQuery);
				livechartConnection.AttachQuery(day2TicsQuery);
				livechartConnection.AttachQuery(day3TicsQuery);
				livechartConnection.AttachQuery(day4TicsQuery);
				livechartConnection.AttachQuery(day5TicsQuery);
				livechartConnection.AttachQuery(day6TicsQuery);

				//add the DAILY and INTRADAY queries
				livechartConnection.AttachQuery(dailyQuery);
				livechartConnection.AttachQuery(intradayQuery);

				//This is a hateful way to do it. 
				//AutoResetEvent hData =new AutoResetEvent(true);
				//dailyQuery.m_ARE=hData;
				dailyQuery.Request(dailyTQL+" ","TAL4");
				//hData.Reset();
				Thread.Sleep(5000);
				intradayQuery.Request(intradayTQL+" ","TAL4");
				Thread.Sleep(5000);
				
				//
				//day6TicsQuery.Request(tics6TQL+" ", "TAL4");
				//Thread.Sleep(5000);
				//day5TicsQuery.Request(tics5TQL+" ", "TAL4");
				//Thread.Sleep(5000);	
				//day4TicsQuery.Request(tics4TQL+" ", "TAL4");
				//Thread.Sleep(5000);
				//day3TicsQuery.Request(tics3TQL+" ", "TAL4");
				//Thread.Sleep(5000);
				//day2TicsQuery.Request(tics2TQL+" ", "TAL4");
				//Thread.Sleep(5000);
				//day1TicsQuery.Request(tics1TQL+" ", "TAL4");
				//Thread.Sleep(5000);
			}
		}


	}

		
	public class klOptionsChainADOSource :klADOSource
	{
		//First query QUOTECHAIN for symbols, then do a request (not advise) on LIVEQUOTE table for the data.

		//Options FIDS
		//1067 EXPIR_DATE DATE The date on which the future or option expires 
		//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
		//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
		//1064 OPINT_1 LONG Open interest 
		//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
		//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
		//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 
		//We have to set the LIVEQUOTE WhereFID when doing the request for price data
		//FID Label Type Format 
		//2000 STYP INT '5' 
		//Here's the table of types
		//2000 (STYP)
		//TOPIC = LIVECHART or LIVEQUOTE 
		//TABLE = TICKS or LIVEQUOTE 
		//FID = 2000 codes are valid in both the WHAT and WHERE clauses:
		//Instrument Type Label Code 
		//Stock T_IT_STOCK 1 
		//Stock Option T_IT_STOCKOPT 2 
		//Future T_IT_FUTURE 3 
		//Future Option T_IT_FUTOPT 4 
		//Spot T_IT_SPOT 5 
		//Warrant T_IT_WARRANT 6 
		//Corp. Bond T_IT_CORPBOND 7 
		//Mutual Fund T_IT_MUTFUND 8 
		//Gov't Bond T_IT_GOVTBOND 9 
		//Index T_IT_INDEX 10 
		//Muni Bond T_IT_MUNIBOND 11 
		//News   12 
		//Unused   13 
		//Statistics T_IT_STATISTICS 14 
		//Money Mkt. Fund T_IT_MMKTFUND 15 
		//Unspecified Bond T_IT_UNSPECBOND 16 
		//Unspecified Fund T_IT_UNSPECFUND 17 
		//Miscellaneous T_IT_MISC 18 
		//SingleStock Future T_IT_SINGLE_STOCK_FUTURE 19 
		//Forex T_IT_FOREX 20 
		//Synthetic T_IT_SYNTHETIC 21 
		//Top 10 List T_IT_CHAIN 22 

		public klOptionsChainADOSource()
		{

			m_QueryHash= new Hashtable();
			m_ThreadHash= new Hashtable();
		}

		//This guy needs to be a hash table as well.
		//One thread per query.
		private Thread t;

		//Queries and threads are matched 1:1 by TQL.
		//Do not duplicate TQL queries in an instance of this class.
		private Hashtable m_QueryHash;
		private Hashtable m_ThreadHash;

		private long GetItemKey(string szItem)
		{
			return(szItem.GetHashCode());
				
		}

		private Connection livequoteConnection;

		//Options FIDS
		//1067 EXPIR_DATE DATE The date on which the future or option expires 
		//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
		//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
		//1064 OPINT_1 LONG Open interest 
		//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
		//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
		//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 
		//We have to set the LIVEQUOTE WhereFID when doing the request for price data
		//FID Label Type Format 
		//2000 STYP INT '5' 
		//Here's the table of types
		//2000 (STYP)
		//TOPIC = LIVECHART or LIVEQUOTE 
		//TABLE = TICKS or LIVEQUOTE 
		//FID = 2000 codes are valid in both the WHAT and WHERE clauses:
		//Instrument Type Label Code 
		//Stock T_IT_STOCK 1 
		//Stock Option T_IT_STOCKOPT 2 
		//Future T_IT_FUTURE 3 
		//Future Option T_IT_FUTOPT 4 
		//Spot T_IT_SPOT 5 
		//Warrant T_IT_WARRANT 6 
		//Corp. Bond T_IT_CORPBOND 7 
		//Mutual Fund T_IT_MUTFUND 8 
		//Gov't Bond T_IT_GOVTBOND 9 
		//Index T_IT_INDEX 10 
		//Muni Bond T_IT_MUNIBOND 11 
		//News   12 
		//Unused   13 
		//Statistics T_IT_STATISTICS 14 
		//Money Mkt. Fund T_IT_MMKTFUND 15 
		//Unspecified Bond T_IT_UNSPECBOND 16 
		//Unspecified Fund T_IT_UNSPECFUND 17 
		//Miscellaneous T_IT_MISC 18 
		//SingleStock Future T_IT_SINGLE_STOCK_FUTURE 19 
		//Forex T_IT_FOREX 20 
		//Synthetic T_IT_SYNTHETIC 21 
		//Top 10 List T_IT_CHAIN 22 

		//First get the options Symbols from QUOTECHAIN table and then do a LIVEQUOTE request -not an advise!
		public  void LIVEQUOTEOptionsSyncADOSinkQuery(klTALApplication a, StreamReader sr,Hashtable WhatFID,	Hashtable WhereFID,int howMany,StreamWriter sw)
		{
			string symbol;
			long filen= sr.BaseStream.Length;
			for(int i=0;i<howMany;i++)
			{
				symbol=sr.ReadLine();
				DateTime d1 = DateTime.Now;

				string fileName = symbol;
				string fileDate="_OPTION_STREAM"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+=fileDate;
				fileName+="_.txt";
					
				
				//Set the symbol element of the TVP 
				TQLTypeValuePair tvp=(TQLTypeValuePair)WhereFID[1003];
				tvp.m_TQLValue=symbol;

				string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);

				klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL,sw);
				
				klAsyncQuery laqq = new klAsyncQuery(tsb);
				
				if(i==0)//fix this
					livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
				AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);
				AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_OptionsOnTerminate);
			
				laqq.OnAdviseData += kladodh;
				laqq.OnRequestData += kladodh;
				laqq.OnTerminate += kladoth;

				//laqq.setMyStreamWriter(sw);
				
				addQuery(laqq,lTQL);
					
				//Nope - we have to 
				//a.ConnectAndSubscribe(tsb);
				Thread.Sleep(1000);
				//lsw=laqq.myStreamWriter();
				//lsw.Flush();
				//lsw.Close();

			}


		}


		public  void QUOTECHAIN_Callback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			int howMany=0;
			
			StreamWriter sw=laq.myStreamWriter();//bbcremove debug code 

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

		

			IEnumerator  blkEnumerator=blk.GetEnumerator();
		
			for(int i=0;i<blk.Count;i++)
			{
				blkEnumerator.MoveNext();



				Type mt= blkEnumerator.GetType();

				string moo=blkEnumerator.Current.ToString();


				//for(int j=0;j<blkEnumerator.Current.ToString()

				IRow rowl =(IRow)blkEnumerator.Current;

				IEnumerator  rowlEnumerator=rowl.GetEnumerator();	

				for(int j=0;j<rowl.Count;j++)

				{
					rowlEnumerator.MoveNext();

					Type mtr= rowlEnumerator.GetType();
					Field mlf=(Field)rowlEnumerator.Current;
				
					Type mtrr=mlf.GetType();

					string mlfn=mlf.Name;
				
				
					string mlfs="'"+mlf.StringValue+"'";

					//I thought the + before the symbol was a problem.  
					//Development when the markets are closed is difficult at times.
					//System.Diagnostics.Debug.WriteLine( mlfn );
					//int mo=mlfs.Length;mlfs=mlfs.Substring(1,mo-1);
					//System.Diagnostics.Debug.WriteLine( mlfs );
				
					howMany++;

					sw.WriteLine(mlfs);
					

					//Connect a query					 
			
				}
			
			}

			

			laq.m_howMany=howMany;

		
			sw.Flush();
		
		}


		public  void QUOTECHAIN_OnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}

		
		public  int QUOTECHAINSynchronousADOSinkQuery(klTALApplication a,string lTQL, StreamWriter sw)
		{
			//Options are only available through LIVEQUOTE.  
			//Use QUOTECHAIN;1003;6050='QQQQ' to get all symbols. 
			//Options FIDS
			//1067 EXPIR_DATE DATE The date on which the future or option expires 
			//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
			//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
			//1064 OPINT_1 LONG Open interest 
			//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
			//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
			//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 

			int howMany=0;

			klTALStreamAttributes tsb_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL,sw);	
			//Make a query and call request
			klAsyncQuery laqq=new klAsyncQuery(tsb_sync);
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			

			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(QUOTECHAIN_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(QUOTECHAIN_OnTerminate);

			//Connect and subscribe - this uses the data processor class
			//a.ConnectAndSubscribe(tsb_qqqq_sync);

			//instead of connecting and subscribing to the stream - install the transactional callback.
			//Circumvent async iocp callback pool and direct to ADO transaction 			
			
			//Since We did not connect - we have to manually make connection
			////Connection livechartConnection =a.FindMyConnection("", "TA_SRV", "LIVECHART");
			///

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "QUOTECHAIN");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;
			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked


			Thread.Sleep(3000);

			//This is a bad thing fix the threading!
			howMany=laqq.m_howMany;
			
			return howMany;

		}

		
			
		public  void LIVEDATA_OptionsCallback(object sender, DataEventArgs e)
		{
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
	
			StreamWriter lsw=laq.myStreamWriter();
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
					lsw.WriteLine(moo);
					lsw.Flush();
									
				}
			}


			

	
		}


		public  void LIVEDATA_OptionsOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		private void ThreadMain()
		{
			System.Console.WriteLine("Threadmain");


			while(m_bRunning)
			{
				Thread myThread=System.Threading.Thread.CurrentThread;
				string myName=myThread.Name;
				System.Console.WriteLine(myName);



				Thread.Sleep(1000);
			}
				 
		}

		private void SetupForData(object sender)
		{
			string TQL;//

			klAsyncQuery laq=(klAsyncQuery)sender;
					
			TQL=laq.myTQL();
				
			//
					
			StreamWriter lsw=laq.myStreamWriter();
					
			//
				
			ThreadStart ts = new ThreadStart(ThreadMain);
			t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			t.Start();  //DataProc gets called when there's a message after this gets fired.

			laq.m_bReadyForData = true;//This query is marked ready for data.
		}

		//Verify dated comments
		public void addQuery(klAsyncQuery AQ,string szItem)
		{
			//The query was initialized and added to the connection by the application class klLiveDataApp
			//Now we add it to the hash table of queries associated with this data processor.
			//Again the benefit of having the app manage the connection-query relationship is the data stream
			//is now decoupled from the device.  We can re-direct data streams on the fly by the TQL - AsyncQuery relationship.
			//AQ.Advise(szItem, "TAL4");
				

			//TLS Solution - each klAsync***Query Gets it's own stream - we create the named TLS entry based on the TQL query - hence the Dig This comment - not to add the same Query twice to a klLiveData***Processor instance.
			//Threads use a local store memory mechanism to store thread-specific data. 
			//The common language runtime allocates a multi-slot data store array to each process when it is created. 
			//The thread can allocate a data slot in the data store, store and retrieve a data value in the slot, 
			//and free the slot for reuse after the thread expires. Data slots are unique per thread. No other thread 
			//(not even a child thread) can get that data.
			//AllocateDataSlot  Allocates an unnamed data slot on all the threads. 
			//AllocateNamedDataSlot  Allocates a named data slot on all threads. 
			//LocalDataStoreSlot
			//LocalDataStoreSlot ldts=System.Threading.Thread.AllocateNamedDataSlot(szItem);
			//Now there's a 1:1 relation between TQL and IOCP threads used.
			//StreamWriter lsw=AQ.myStreamWriter();
			//System.Threading.Thread.SetData(ldts,lsw );

			//The thread is named with the TQL string and we use the thread name to retrieve the TLS object.
			//This could be a little cleaner.
				
			//Sets the handles to Query callbacks.  Same callback used for all queries used by an instance of klLiveData***Processor. 
			//AQ.OnAdviseData +=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);

			//AQ.OnOtherAck +=new AsyncQuery.AckHandler(m_query_OnOtherAck);

			long lKey=GetItemKey(AQ.myTQL());

			m_QueryHash.Add(lKey,AQ);
				
			string TQL=AQ.myTQL();
				
			//
					
								
			livequoteConnection.AttachQuery(AQ);
	
			AQ.Request(TQL, "TAL4");//Callback gets invoked
			//Thread.Sleep(500);
				
			//ThreadStart ts = new ThreadStart(ThreadMain);
			//t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			//t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			//t.Start();  //DataProc gets called when there's a message after this gets fired.

			//AQ.m_bReadyForData = true;//This query is marked ready for data.

			m_bRunning = true;//This means there is at least one query attached a connection in this data processor.

		}

		bool m_bRunning;
            
		private Hashtable m_AutoResetEventHash;
			
		private void m_query_OnOtherAck(object sender, AckEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;

			if (1 == e.Status)
			{
				if (!(laq.m_bReadyForData))
				{
					//We have to set up the stream object and get it into the thread for DataProc?
					//This is bad.  It seems like there ultimately needs to be a binding between TQL C# function
					//and data processing streams.  
					//There needs to be a way to fill a hash table of thread mains written for each TQL query. 
					//
					//sender is always the query type associated with this klLiveData***Processor
					SetupForData(sender);
				}
			}
		}
			

		
	}


	public class klLiveQuoteEquityTickADOSource :klADOSource 
	{		
			
		//private klLiveDataDelegate m_delegate;
				
		//one per - make thread safe as the threads will use th TQL string to get the stream object from the table
		static private Hashtable m_QueryHash;
				
		//private Dequeue m_buf;

		//one buffer Dequeue per stream 
		//Deque is sometimes written dequeue, but this is generally not preferred because 
		//dequeue is also a verb meaning "to remove from a queue". Nevertheless, several libraries 
		//and some writers, such as Aho, Hopcroft, and Ullman use dequeue.
		static private Hashtable m_DequeHash;
	
		public void Add(long lKey,Dequeue AQDataContainer)
		{
			m_DequeHash.Add(lKey,AQDataContainer);
		}


		//This guy needs to be a hash table as well.
		//One thread per query.
		private Thread t;

		static private Hashtable m_ThreadHash;

		//private AutoResetEvent hData;
		//add accessors
		static private Hashtable m_AutoResetEventHash;

		public void Add(long lKey,AutoResetEvent hData)
		{
			m_AutoResetEventHash.Add(lKey,hData);
		}

		private bool m_bReadyForData;
		private bool m_bRunning;
			
		private void m_query_OnAdviseData(object sender, DataEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
					
			long lKey=GetItemKey(TQL);
					
				   
			Dequeue AQ_DataContainer =(Dequeue) m_DequeHash[lKey];
				
			lock(AQ_DataContainer)
			{
				AutoResetEvent hData= (AutoResetEvent)m_AutoResetEventHash[lKey];
				lock(hData)
				{


					if (!(laq.m_bReadyForData))
					{
						SetupForData(sender);
					}
					lock(AQ_DataContainer.SyncRoot)
					{
						StreamWriter lsw=laq.myStreamWriter();
						RawData rd=e.Data;
						IDataBlock block= rd.GetDataAsBlock();
						int iRows = 1;
						int iFields = 0;
						foreach(IRow row in block)
						{
							System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
							foreach(Field fld in row)
							{
								string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
								lsw.WriteLine(moo);
								lsw.Flush();
									
							}
						}


						//AQ_DataContainer.EnqueueTail(e.Data);


					}
				
					hData.Set();
				}
			}
			
		}
				
		//((klLiveData.klLiveDataMain.klAsync***Query)(sender))
		private void m_query_OnOtherAck(object sender, AckEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;

			if (1 == e.Status)
			{
				if (!(laq.m_bReadyForData))
				{
					//We have to set up the stream object and get it into the thread for DataProc?
					//This is bad.  It seems like there ultimately needs to be a binding between TQL C# function
					//and data processing streams.  
					//There needs to be a way to fill a hash table of thread mains written for each TQL query. 
					//
					//sender is always the query type associated with this klLiveData***Processor
					SetupForData(sender);
				}
			}
		}
			

			
		private void DataProc()
		{
			//				try
			//				{
			//
			//
			//
			//					Object o;
			//
			//					while (m_bRunning)
			//					{
			//						//Alelu - get the NAME of this thread - it's the TQL string
			//						//Get the named TLS object - of type klDataStreamWriter (StreamWriter for V0.1)
			//						string TQL= System.Threading.Thread.CurrentThread.Name;
			//						////
			//						LocalDataStoreSlot ldts=System.Threading.Thread.GetNamedDataSlot(TQL);
			//						//Now there's a 1:1 relation between TQL and IOCP threads used.
			//					
			//						//bbcrevisit this is not working for now - 
			//						//as a workaround - poosibly a better solution
			//						// we are using the static hash table member to get the stream
			//						StreamWriter lsw=(StreamWriter)System.Threading.Thread.GetData(ldts);
			//
			//						long lKey=GetItemKey(TQL);
			//
			//						AutoResetEvent hData= (AutoResetEvent)m_AutoResetEventHash[lKey];
			//						hData.WaitOne();
			//						
			//
			//						if (m_QueryHash.Contains(lKey))
			//						{
			//							klAsyncQuery las = (klAsyncQuery) m_QueryHash[lKey];
			//							lsw=las.myStreamWriter();
			//						}		
			//						
			//						do
			//						{
			//							Dequeue m_buf=(Dequeue)m_DequeHash[lKey];
			//							
			//							//debug
			//							System.Console.WriteLine(TQL);
			//							System.Diagnostics.Debug.WriteLine(TQL);
			//							
			//							
			//							lock(m_buf.SyncRoot)
			//							{
			//									//o = m_buf.DequeueHead();
			//								o=null;
			//							}
			//							//RawData rd=(RawData)o;
			//							//IDataBlock block= rd.GetDataAsBlock();
			//
			//							if (!(null == o))
			//							{
			//								RawData r=(RawData) o;
			//								IDataBlock block= r.GetDataAsBlock();
			//								int iRows = 1;
			//								int iFields = 0;
			//								foreach(IRow row in block)
			//								{
			//									System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
			//									foreach(Field fld in row)
			//									{
			//										string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
			//										lsw.WriteLine(moo);
			//										lsw.Flush();
			//										System.Console.WriteLine(String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue));
			//										iFields = 1;
			//										System.Diagnostics.Debug.WriteLine( String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue) );
			//									}
			//								}
			//
			//
			//							}
			//						}
			//						while (!(null == o));
			//				
			//					}
			//				}
			//				catch
			//				{
			//					m_bReadyForData = false;
			//				}

		}
		//sender is always the query type associated with this klLiveData***Processor
		private void SetupForData(object sender)
		{
			string TQL;//

			klAsyncQuery laq=(klAsyncQuery)sender;
					
			TQL=laq.myTQL();
					
			StreamWriter lsw=laq.myStreamWriter();
					
			ThreadStart ts = new ThreadStart(DataProc );
			t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			t.Start();  //DataProc gets called when there's a message after this gets fired.

			laq.m_bReadyForData = true;//This query is marked ready for data.
		}
		//Dig this - do not duplicate TQL stream objects in the klLiveQuoteEquityTickFileSource Hash!  
		private long GetItemKey(string szItem)
		{
			return(szItem.GetHashCode());
				
		}
		public klAsyncQuery findQuery( string szItem)
		{
			long lKey=GetItemKey(szItem);
			 
			if (m_QueryHash.Contains(lKey))
			{
				klAsyncQuery las = (klAsyncQuery) m_QueryHash[lKey];
				return las;
				//lsw=las.myStreamWriter();
			}
			else 
				return null;
		}
			
		//Verify dated comments
		public void addQuery(klAsyncQuery AQ,string szItem)
		{
			//The query was initialized and added to the connection by the application class klLiveDataApp
			//Now we add it to the hash table of queries associated with this data processor.
			//Again the benefit of having the app manage the connection-query relationship is the data stream
			//is now decoupled from the device.  We can re-direct data streams on the fly by the TQL - AsyncQuery relationship.
			AQ.Advise(szItem, "TAL4");
				

			//TLS Solution - each klAsync***Query Gets it's own stream - we create the named TLS entry based on the TQL query - hence the Dig This comment - not to add the same Query twice to a klLiveData***Processor instance.
			//Threads use a local store memory mechanism to store thread-specific data. 
			//The common language runtime allocates a multi-slot data store array to each process when it is created. 
			//The thread can allocate a data slot in the data store, store and retrieve a data value in the slot, 
			//and free the slot for reuse after the thread expires. Data slots are unique per thread. No other thread 
			//(not even a child thread) can get that data.
			//AllocateDataSlot  Allocates an unnamed data slot on all the threads. 
			//AllocateNamedDataSlot  Allocates a named data slot on all threads. 
			//LocalDataStoreSlot
			LocalDataStoreSlot ldts=System.Threading.Thread.AllocateNamedDataSlot(szItem);
			//Now there's a 1:1 relation between TQL and IOCP threads used.
			StreamWriter lsw=AQ.myStreamWriter();
			System.Threading.Thread.SetData(ldts,lsw );

			//The thread is named with the TQL string and we use the thread name to retrieve the TLS object.
			//This could be a little cleaner.
				
			//Sets the handles to Query callbacks.  Same callback used for all queries used by an instance of klLiveData***Processor. 
			AQ.OnAdviseData +=new AsyncQuery.DataHandler(m_query_OnAdviseData);

			AQ.OnOtherAck +=new AsyncQuery.AckHandler(m_query_OnOtherAck);

			long lKey=GetItemKey(AQ.myTQL());

			m_QueryHash.Add(lKey,AQ);

			m_bRunning = true;//This means there is at least one query attached a connection in this data processor.

		}
            
		
		public klLiveQuoteEquityTickADOSource()
		{
			m_bReadyForData = false;
			m_QueryHash= new Hashtable();
			m_DequeHash= new Hashtable();
			m_AutoResetEventHash = new Hashtable();
				
		}
	}
		

	public class klLiveChartEquityDailyIntradayFileSource :klFileSource
	{
		private String m_baseFileName;

		public klLiveChartEquityDailyIntradayFileSource(String baseFileName) :base("")
		{
			m_baseFileName=baseFileName;
		}

		//klLiveChartEquityFileSource

		///TQL Notes:
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS
		//Change service (TA_SRV ACCOUNT_GATEWAY NEWSSERV)
		//and topic (LIVECHART LIVE_DATA) 
		//Suppose you wanted to retrieve 1 day of trades for IBM. Your TQL request would read: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM' 
		//If you request data at times just before or just after the open, you may get an additional day of data
		//																								   
		//due to the way TA_SRV caches data before writing it. As a workaround, use FIDs 9009 and 9010 to 
		//restrict the times for the chart. The following query would restrict data to the date specified, between 9:30 AM and 4:00 PM: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM',9009='09:30',9010='16:00'


		//Where FIDs: 
		//1003- ticker symbol search condition 
		//9008 - stop date condition 
		//9009 - chart start time 
		//9010 - chart stop time 
		//
		//The TICKS table stores data for 7 calendar days back from the current day, but you may only request 1 day of 
		//TICKS data at a time. The default day is today. If you specify a different day in 9008, that day will be returned instead. 
		//The day you specify in 9008 can be up to 7 calendar days back from the current day. 
		//
		//You may use 24-hour or minutes-from-midnight time formats in FIDs 9009 and 9010. Additionally, FIDs 9009 and 9010 
		//may also be used in queries against the INTRADAY table.
		//
		//If you want to see the before-market or after-market trades on NASDAQ stocks, add FID 6040 to the WHAT clause, and
		//specify times before/after the market. All before/after-market trades will have a 20 in the 6040 FID, indicating that 
		//it is a FormT trade. If you wish to only see updates on the before/after-market FormT trades, make your code advise on 
		//the string with 6040, SALE_CONDITION, in the WHAT clause.


		//Map of FID's for TICS WHAT FID's
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY
		//What FIDs:
		//FID	Label	Type	Ticks	Intraday	Daily	Retrieves
		//1032	ACVOL_1	LONG	 	x	x	Session's total trading volume for a particular instrument 
		//1416	CONTRIB_1	INT	    x	 	 	Contributor (ForEx data only, in place of TRDXID_1)
		//1003	DISP_NAME	STRING	x	x	x	Symbol name for instrument
		//1012	HIGH_1	LPACK	 	x	x	This session's highest transaction value
		//1013	LOW_1	LPACK	 	x	x	This session's lowest transaction value
		//6025	MONEY_FLOW	LONG	 	 	x	Money flow (stocks only, in place of OPIN1)
		//1077	NUM_MOVES	INT	 	x	 	Number of trades this session for a particular instrument
		//1019	OPEN_PRC	LPACK	 	x	x	This session's opening price
		//1070	SETTLE	LPACK	 	x	x	Settlement price. The official closing price of a commodity
		//6040   SALE_CONDITION	INT	x	 	 	Sale Conditions for Nasdaq
		//9020    TICK_TYPE	INT	    x	 	 	Quote condition indicator
		//1016	TRD_DATE	DATE	x	x	x	Trade Date
		//1044	TRDXID_1	STRING	x	 	 	Exchange Identifier of the latest trade (U.S. composite stock only)
		//1006	TRDPRC_1	LPACK	x	 	 	Last trade price or value
		//1018	TRDTIM_1	TIME	x	x	x	Time of the last reported trade (listed in TRDPRC_1)
		//1178	TRDVOL_1	LONG	x	 	 	Transactional volume of the last trade price reported in TRDPRC_1
		//Where FID's for TIC's  - these are used to set up XML file for dataset transaction
		//Where FIDS:
		//FID	Label	Type	Ticks	Intraday	Daily	Format
		//9011	CHART_ALL_TICKS	BOOL	x	 	 	(Returns trades, bids and asks in the TRDPRC_1 field)
		//9005    CHART_BAR_INTERVAL	INT	 	x	x	'0' (daily)
		//9012	CHART_FILL_BARS	BOOL	 	x	x	Fills gaps between bars
		//9006	STARTONBOUND	BOOL	 	x	x	Starts bars at beginning of session (used for odd interval bars)
		//9007	CHART_STARTDATE	DATE	 	x	x	'1996/01/01' (TICKS table can request only one day at a time. The desired date is indicated in CHART_STOPDATE).
		//9009	CHART_STARTTIME	TIME	x	x	 	'14:00'
		//9008	CHART_STOPDATE	DATE	x	x	x	'1996/01/15'
		//9010	CHART_STOPTIME	TIME	x	x	 	'14:00'
		//9004	DAYS_BACK	INT	 	    x	x	'5' (number of days back for INTRADAY, and daily bars. Number of weeks or months back for weekly or monthly bar data.)
		//1003	DISP_NAME	STRING	    x	x	x	'IBM' (Symbol formats are datafeed-specific. Check your Feed Handler help file for the correct format.)
		//1016	TRD_DATE	DATE	    x	x	x	'1996/01/15'
		//1018	TRDTIM_1	TIME	    x	x	x	'14:00'
		//1044	TRDXID_1	STRING	    x	 	 	'N'

		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY 
		//FID = 9020 codes only are valid in the WHAT clause:
		//Description Code 
		//Trade 0 
		//Bid 1 
		//Ask 2 
		//Regional bid  3 
		//Regional ask 4 
		//Deleted 11 
		//Inserted 12 
		//Irregular Delete 44 
		//FormT Trade 32 




		//Helper function for 
		public  void LIVECHARTSynchronousFileSinkQuery(klTALApplication a,string lTQL, StreamWriter sw)
		{
			klTALStreamAttributes tsb_qqqq_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL,sw);	
			//Make a query and call request
			klAsyncQuery laqq=new klAsyncQuery(tsb_qqqq_sync);
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			Hashtable sp=new Hashtable();
			sp.Add(1006,"TRDPRC_1");
			sp.Add(1016,"TRD_DATE");
			sp.Add(1178,"TRDVOL_1");
			sp.Add(1018, "TRDTIM_1");
			laqq.setTQLPersist(sp);
			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_OnTerminate);

			//Connect and subscribe - this uses the data processor class
			//a.ConnectAndSubscribe(tsb_qqqq_sync);

			//instead of connecting and subscribing to the stream - install the transactional callback.
			//Circumvent async iocp callback pool and direct to ADO transaction 			
			
			//Since We did not connect - we have to manually make connection
			////Connection livechartConnection =a.FindMyConnection("", "TA_SRV", "LIVECHART");
			///

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;
			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked


			Thread.Sleep(3000);


		}

		//Gets option symbols for an underlying.
		

		
		public  void LIVECHART_Callback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			
			StreamWriter sw=laq.myStreamWriter();//bbcremove debug code 

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable myTQLPersist=laq.myTQLPersist();
			
			IEnumerator  ie=row.GetEnumerator();

			myTQLPersist.GetEnumerator();

     			
		
			int rc=row.Count;
			int pc=myTQLPersist.Count;// row.Count; typically has more FID's that we want to write to the transaction
			int op=0;
			for(op=0;op<pc;op++)
			{ 
				
				string fid=(string)myTQLPersist[op];
				//sp.Add(1006,"TRDPRC_1");
				//sp.Add(1016,"TRD_DATE");
				//sp.Add(1178,"TRDVOL_1");

					


				//IEnumerator  ie=row.GetEnumerator();
				//.m_arFields[i];
			}


			int elements = 0;

			if (row["TRDPRC_1"] != null)
				elements = row["TRDPRC_1"].PriceVectorValue.Length;
			
			int first_element=0;
			
			//here we take out our vectors

			Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
			int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
			TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
			DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;

			sw.WriteLine("Date,Time,Trade Price,Trade Vol");
						
			for (int i = first_element; i < elements; i++)
			{

				//System.Console.WriteLine(String.Format("{0},{1}:{2},{3},{4},{5},{6},{7}",
				sw.WriteLine(String.Format("{0},{1}:{2},{3},{4}",
					TRD_DATE[i].ToString("yyyy/MM/dd"),
					TRDTIM_1[i].Hours.ToString("D2"),
					TRDTIM_1[i].Minutes.ToString("D2"),
					TRDPRC_1[i].ToString(),
					TRDVOL_1[i].ToString()));
						
						
			}
				
			sw.Flush();
		
		}


		public  void LIVECHART_OnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		//Tic Data is cached in local file system - then transacted to db in one shot.
		public  void LIVECHART_PrimeDBTicCallback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	

			//lock(laq)//Do a wait one - or threadblock here - 
			//{
			//We created this string from the TABLE and What/Where FID's
			//String TQL=laq.myTQL();
			
			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			//Unused here - but we will use this in the realitme filters 
			//Hashtable myTQLPersist=laq.myTQLPersist();
			//IEnumerator  ie=row.GetEnumerator();
			//myTQLPersist.GetEnumerator();

			Hashtable whatFID=laq.myWhatFID();//
			Hashtable whereFID=laq.myWhereFID();//


			//int rc=row.Count;
			//int pc=myTQLPersist.Count;// row.Count; typically has more FID's that we want to write to the transaction
			//int op=0;
			//for(op=0;op<pc;op++)
			//{ 
				
			//string fid=(string)myTQLPersist[op];
			//sp.Add(1006,"TRDPRC_1");
			//sp.Add(1016,"TRD_DATE");
			//sp.Add(1178,"TRDVOL_1");
			//IEnumerator  ie=row.GetEnumerator();
			//.m_arFields[i];
			//}

	
			//Here we take out our vectors.
			//We only pull what we need - but there is a hard coupling to the FID's in the TICKS,INTRADAY,DAILY tbles.
		
			//This is the set we currently support.
			
			//From TICKS table
			//			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			//			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44

			if(	whatFID.ContainsValue("TICK_TYPE"))
			{ //Only tick data uses this fid
				Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
				int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
				int elements = 0;
				
				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				if (row["TRDPRC_1"] != null)
					elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
				int first_element=0;

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						TRDPRC_1[i].ToString(),
						TRDVOL_1[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);

			}

			//From INTRADAY TABLE
			//			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			//			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			//			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");
			if(	whatFID.ContainsValue("NUM_MOVES"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] NUM_MOVES = row["NUM_MOVES"] == null ? null : row["NUM_MOVES"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;
				int first_element=0;

				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4},{5},{6},{7}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						OPEN_PRC[i].ToString(),
						HIGH_1[i].ToString(),
						LOW_1[i].ToString(),
						SETTLE[i].ToString(),
						NUM_MOVES[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);
			}
			


			//From Daily Hashtable
			//			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			//			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			//			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");
			if(	whatFID.ContainsValue("MONEY_FLOW"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] MONEY_FLOW = row["MONEY_FLOW"] == null ? null : row["MONEY_FLOW"].IntVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;

				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);


				int first_element=0;	
				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1},{2},{3},{4},{5}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						OPEN_PRC[i].ToString(),
						HIGH_1[i].ToString(),
						LOW_1[i].ToString(),
						SETTLE[i].ToString(),
						MONEY_FLOW[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);
				//laq.m_ARE.Set();

			}


		

		}

						 	
		public  void LIVECHART_PrimeDBTicOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		public void LIVECHARTSynchronousFileSinkQuery(klTALApplication a, ArrayList symbols )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");

			//Here we create hash tables for What and Where FID's for the types of ADO transactions.
			//TIC, INTRA,DAILY
			//			Hashtable TICKS_WhatFID_Hash=new Hashtable();
			//			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			//			TICKS_WhatFID_Hash.Add(1006,"TRDPRC_1");
			//			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");

			//TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44
			
			Hashtable INTRADAY_WhatFID_Hash=new Hashtable();
			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");

			Hashtable DAILY_WhatFID_Hash=new Hashtable();
			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			DAILY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");

					

			//For each symbol we make queries for 5days tic, 15days 5min, 2000 daily
			//the tic queries are cached until all five days are available - then the tic table is created.

			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
			
			
				//Get todays date - then get 5days tic, 15days 5min, 2000 daily.

				//TQL Item String components (TABLE;WHAT;WHERE)
			
				Hashtable TICKS_WhereFID_Hash=new Hashtable();
				
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();

				TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash.Add(9008,tvp);//For tic data only the end date is used to get the data
		 
				tvp=new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'");
				TICKS_WhereFID_Hash.Add(1003,tvp);


				Hashtable INTRADAY_WhereFID_Hash=new Hashtable();
				INTRADAY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				INTRADAY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				//INTRADAY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//INTRADAY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//INTRADAY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");
				INTRADAY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","15"));
				INTRADAY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","5"));//5min bars




				Hashtable DAILY_WhereFID_Hash=new Hashtable();
				DAILY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				DAILY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				DAILY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","10000"));
				DAILY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","0"));//0 for daily
				//DAILY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//DAILY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//DAILY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");

				//We need to construct the TQL for each query - the persist, what and where FID Hashes have been
				//set up at this point.  The query can expand on the FID's, but it can not leave any out.
				string dailyTQL=klTALApplication.TQLfromWhatWhereFIDHashes("DAILY", DAILY_WhereFID_Hash ,DAILY_WhatFID_Hash);

				string intradayTQL=klTALApplication.TQLfromWhatWhereFIDHashes("INTRADAY", INTRADAY_WhereFID_Hash ,INTRADAY_WhatFID_Hash);

				//string tics1TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
							
				
				//Construct the Queries.
				//These queries bypass the file stream and use ADO transactions.
				klAsyncQuery dailyQuery=new klAsyncQuery(DAILY_WhatFID_Hash,DAILY_WhereFID_Hash);
			
				klAsyncQuery intradayQuery=new klAsyncQuery(INTRADAY_WhatFID_Hash,INTRADAY_WhereFID_Hash);

				//The TicQueries use a stream writer we instantiate amd manage here. 
				
				//Use the symbol, date items in the WhereFID_Hash to make filename
				//				string fileName = (string)symbols[symbolCount];
				//				string fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				StreamWriter lsw =new StreamWriter(fileName);


				//For now one for DAYS and INTRADAY 
				string fileName = (string)symbols[symbolCount];
				string fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+= "_DAILY_"+fileDate;
				fileName+=".txt";
				StreamWriter dsw =new StreamWriter(m_baseFileName+fileName);

				fileName = (string)symbols[symbolCount];
				fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+= "_INTRADAY_"+fileDate;
				fileName+=".txt";
				StreamWriter isw =new StreamWriter(m_baseFileName+fileName);



				//                
				//				klAsyncQuery day1TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);				
				//				//Set the stream writer
				//				day1TicsQuery.setMyStreamWriter(lsw);

				//Change the date in the Where tvp fid hash
				//Date Arithematic
				System.TimeSpan sts=new TimeSpan(7,0,0,0);//days, hours, minutes, and seconds
								
				DateTime d2=d1.Subtract( sts);

				//A DayOfWeek enumerated constant that indicates the day of the week. 
				//This property value ranges from zero, indicating Sunday, to six, indicating Saturday.
				//				if( d2.DayOfWeek == (DayOfWeek)6 )
				//					d2=d2.AddDays(2);
				//				if( d2.DayOfWeek==0)
				//					d2=d2.AddDays(1);
				//				date=d2.ToShortDateString();
				//				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				//				TICKS_WhereFID_Hash[9008]=tvp;
				//				string tics2TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);
				//				klAsyncQuery day2TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				//				fileName = (string)symbols[symbolCount];
				//				fileDate="_"+d2.Month+"_"+d2.Day+"_"+d2.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				lsw =new StreamWriter(fileName);
				//				day2TicsQuery.setMyStreamWriter(lsw);
				//
				//
				//				DateTime d3=d2.AddDays(1);
				//				if( d3.DayOfWeek==(DayOfWeek)6)
				//					d3=d3.AddDays(2);
				//				if(  (d3.DayOfWeek==0 ))
				//					d3=d3.AddDays(1);
				//				date=d3.ToShortDateString();
				//				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				//				TICKS_WhereFID_Hash[9008]=tvp;
				//				string tics3TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				//				klAsyncQuery day3TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				//				fileName = (string)symbols[symbolCount];
				//				fileDate="_"+d3.Month+"_"+d3.Day+"_"+d3.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				lsw =new StreamWriter(fileName);
				//				day3TicsQuery.setMyStreamWriter(lsw);				
				//				
				//				DateTime d4=d3.AddDays(1);
				//				if( d4.DayOfWeek==(DayOfWeek)6)
				//					d4=d4.AddDays(2);
				//				if(  (d4.DayOfWeek==0 ))
				//					d4=d4.AddDays(1);
				//				date=d4.ToShortDateString();
				//				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				//				TICKS_WhereFID_Hash[9008]=tvp;
				//				string tics4TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				//				klAsyncQuery day4TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				//				fileName = (string)symbols[symbolCount];
				//				fileDate="_"+d4.Month+"_"+d4.Day+"_"+d4.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				lsw =new StreamWriter(fileName);
				//				day4TicsQuery.setMyStreamWriter(lsw);
				//
				//
				//				DateTime d5=d4.AddDays(1);
				//				if( d5.DayOfWeek==(DayOfWeek)6)
				//					d5=d5.AddDays(2);
				//				if(  (d5.DayOfWeek==0 ))
				//					d5=d5.AddDays(1);
				//				date=d5.ToShortDateString();
				//				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				//				TICKS_WhereFID_Hash[9008]=tvp;
				//				string tics5TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				//				klAsyncQuery day5TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				//				fileName = (string)symbols[symbolCount];
				//				fileDate="_"+d5.Month+"_"+d5.Day+"_"+d5.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				lsw =new StreamWriter(fileName);
				//				day5TicsQuery.setMyStreamWriter(lsw);
				//				
				//				//This is safe if we're running when the US equitiy markets are open.
				//				//bbcrevisit - protect it later
				//				DateTime d6=d5.AddDays(1);
				//				if( d6.DayOfWeek==(DayOfWeek)6)
				//					d6=d6.AddDays(2);
				//				if(  (d6.DayOfWeek==0 ))
				//					d6=d6.AddDays(1);
				//				date=d6.ToShortDateString();
				//				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				//				TICKS_WhereFID_Hash[9008]=tvp;
				//				string tics6TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				//				klAsyncQuery day6TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				//				fileName = (string)symbols[symbolCount];
				//				fileDate="_"+d6.Month+"_"+d6.Day+"_"+d6.Year;;
				//				fileName+="_TICKS_"+fileDate;
				//				fileName+=".txt";
				//				lsw =new StreamWriter(fileName);
				//				day6TicsQuery.setMyStreamWriter(lsw);
				//
				//Temporary
				dailyQuery.setMyStreamWriter(dsw);
				intradayQuery.setMyStreamWriter(isw);

		
				//There were originally two types of callback used by this method - one that caches tics
				//and one that does the ADO transaction for INTRADAY and DAILY
				//AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_PrimeDBCallback);
				//AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBOnTerminate);

				//Uses the stream writer to cache tics.  Then loads them up for the transaction. 
				AsyncQuery.DataHandler kladodhtic=new AsyncQuery.DataHandler(LIVECHART_PrimeDBTicCallback);
				AsyncQuery.TerminateHandler kladothtic=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBTicOnTerminate);


				//Set the query handler for all of the INTRADAY and DAILY queries 
				//				dailyQuery.OnAdviseData += kladodh;
				//				dailyQuery.OnRequestData += kladodh;
				//				dailyQuery.OnTerminate += kladoth;
				//
				//				intradayQuery.OnAdviseData += kladodh;
				//				intradayQuery.OnRequestData += kladodh;
				//				intradayQuery.OnTerminate += kladoth;

				//temporary - 
				dailyQuery.OnAdviseData += kladodhtic;
				dailyQuery.OnRequestData += kladodhtic;
				dailyQuery.OnTerminate += kladothtic;
				
				intradayQuery.OnAdviseData += kladodhtic;
				intradayQuery.OnRequestData += kladodhtic;
				intradayQuery.OnTerminate += kladothtic;
	
				//Set the query handler for all of the tic queries.
				//				day1TicsQuery.OnAdviseData += kladodhtic;
				//				day1TicsQuery.OnRequestData += kladodhtic;
				//				day1TicsQuery.OnTerminate += kladothtic;
				//
				//				day2TicsQuery.OnAdviseData += kladodhtic;
				//				day2TicsQuery.OnRequestData += kladodhtic;
				//				day2TicsQuery.OnTerminate += kladothtic;
				//
				//				day3TicsQuery.OnAdviseData += kladodhtic;
				//				day3TicsQuery.OnRequestData += kladodhtic;
				//				day3TicsQuery.OnTerminate += kladothtic;
				//
				//				day4TicsQuery.OnAdviseData += kladodhtic;
				//				day4TicsQuery.OnRequestData += kladodhtic;
				//				day4TicsQuery.OnTerminate += kladothtic;
				//
				//				day5TicsQuery.OnAdviseData += kladodhtic;
				//				day5TicsQuery.OnRequestData += kladodhtic;
				//				day5TicsQuery.OnTerminate += kladothtic;
				//
				//				day6TicsQuery.OnAdviseData += kladodhtic;
				//				day6TicsQuery.OnRequestData += kladodhtic;
				//				day6TicsQuery.OnTerminate += kladothtic;


				//Add the TICKS queries
				//				livechartConnection.AttachQuery(day1TicsQuery);
				//				livechartConnection.AttachQuery(day2TicsQuery);
				//				livechartConnection.AttachQuery(day3TicsQuery);
				//				livechartConnection.AttachQuery(day4TicsQuery);
				//				livechartConnection.AttachQuery(day5TicsQuery);
				//				livechartConnection.AttachQuery(day6TicsQuery);

				//add the DAILY and INTRADAY queries
				livechartConnection.AttachQuery(dailyQuery);
				livechartConnection.AttachQuery(intradayQuery);
				//This is a hateful way to do it. bbcrevisit - this needs to work - 
				//AutoResetEvent hData =new AutoResetEvent(true);
				//dailyQuery.m_ARE=hData;


				dailyQuery.Request(dailyTQL+" ","TAL4");
				//				//hData.Reset();
				Thread.Sleep(2000);
				dsw.Flush();
				dsw.Close();
						
				intradayQuery.Request(intradayTQL+" ","TAL4");
				Thread.Sleep(2000);
				isw.Flush();
				isw.Close();
	
			
				//				day6TicsQuery.Request(tics6TQL+" ", "TAL4");
				//				Thread.Sleep(5000);
				//				lsw=day6TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();	
				//				
				//				day5TicsQuery.Request(tics5TQL+" ", "TAL4");
				//				Thread.Sleep(5000);
				//				lsw=day5TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();			
				//				
				//				day4TicsQuery.Request(tics4TQL+" ", "TAL4");
				//
				//				Thread.Sleep(5000);
				//				lsw=day4TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();
				//					
				//				day3TicsQuery.Request(tics3TQL+" ", "TAL4");
				//				Thread.Sleep(5000);
				//				lsw=day3TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();	
				//					
				//				day2TicsQuery.Request(tics2TQL+" ", "TAL4");
				//				Thread.Sleep(5000);
				//				lsw=day2TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();	
				//				
				//				day1TicsQuery.Request(tics1TQL+" ", "TAL4");
				//				Thread.Sleep(5000);
				//				lsw=day1TicsQuery.myStreamWriter();
				//				lsw.Flush();lsw.Close();	
				
				
				
			}


		}


	}

		
	public class klLiveChartEquityFileSource :klFileSource
	{

		public klLiveChartEquityFileSource() :base("")
		{
		}

		//klLiveChartEquityFileSource

		///TQL Notes:
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS
		//Change service (TA_SRV ACCOUNT_GATEWAY NEWSSERV)
		//and topic (LIVECHART LIVE_DATA) 
		//Suppose you wanted to retrieve 1 day of trades for IBM. Your TQL request would read: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM' 
		//If you request data at times just before or just after the open, you may get an additional day of data
		//																								   
		//due to the way TA_SRV caches data before writing it. As a workaround, use FIDs 9009 and 9010 to 
		//restrict the times for the chart. The following query would restrict data to the date specified, between 9:30 AM and 4:00 PM: 
		//TICKS;1003,1016,1006,1044,1178;9008='2005/01/02',1003='IBM',9009='09:30',9010='16:00'


		//Where FIDs: 
		//1003- ticker symbol search condition 
		//9008 - stop date condition 
		//9009 - chart start time 
		//9010 - chart stop time 
		//
		//The TICKS table stores data for 7 calendar days back from the current day, but you may only request 1 day of 
		//TICKS data at a time. The default day is today. If you specify a different day in 9008, that day will be returned instead. 
		//The day you specify in 9008 can be up to 7 calendar days back from the current day. 
		//
		//You may use 24-hour or minutes-from-midnight time formats in FIDs 9009 and 9010. Additionally, FIDs 9009 and 9010 
		//may also be used in queries against the INTRADAY table.
		//
		//If you want to see the before-market or after-market trades on NASDAQ stocks, add FID 6040 to the WHAT clause, and
		//specify times before/after the market. All before/after-market trades will have a 20 in the 6040 FID, indicating that 
		//it is a FormT trade. If you wish to only see updates on the before/after-market FormT trades, make your code advise on 
		//the string with 6040, SALE_CONDITION, in the WHAT clause.


		//Map of FID's for TICS WHAT FID's
		//SERVICE = TA_SRV
		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY
		//What FIDs:
		//FID	Label	Type	Ticks	Intraday	Daily	Retrieves
		//1032	ACVOL_1	LONG	 	x	x	Session's total trading volume for a particular instrument 
		//1416	CONTRIB_1	INT	    x	 	 	Contributor (ForEx data only, in place of TRDXID_1)
		//1003	DISP_NAME	STRING	x	x	x	Symbol name for instrument
		//1012	HIGH_1	LPACK	 	x	x	This session's highest transaction value
		//1013	LOW_1	LPACK	 	x	x	This session's lowest transaction value
		//6025	MONEY_FLOW	LONG	 	 	x	Money flow (stocks only, in place of OPIN1)
		//1077	NUM_MOVES	INT	 	x	 	Number of trades this session for a particular instrument
		//1019	OPEN_PRC	LPACK	 	x	x	This session's opening price
		//1070	SETTLE	LPACK	 	x	x	Settlement price. The official closing price of a commodity
		//6040   SALE_CONDITION	INT	x	 	 	Sale Conditions for Nasdaq
		//9020    TICK_TYPE	INT	    x	 	 	Quote condition indicator
		//1016	TRD_DATE	DATE	x	x	x	Trade Date
		//1044	TRDXID_1	STRING	x	 	 	Exchange Identifier of the latest trade (U.S. composite stock only)
		//1006	TRDPRC_1	LPACK	x	 	 	Last trade price or value
		//1018	TRDTIM_1	TIME	x	x	x	Time of the last reported trade (listed in TRDPRC_1)
		//1178	TRDVOL_1	LONG	x	 	 	Transactional volume of the last trade price reported in TRDPRC_1
		//Where FID's for TIC's  - these are used to set up XML file for dataset transaction
		//Where FIDS:
		//FID	Label	Type	Ticks	Intraday	Daily	Format
		//9011	CHART_ALL_TICKS	BOOL	x	 	 	(Returns trades, bids and asks in the TRDPRC_1 field)
		//9005    CHART_BAR_INTERVAL	INT	 	x	x	'0' (daily)
		//9012	CHART_FILL_BARS	BOOL	 	x	x	Fills gaps between bars
		//9006	STARTONBOUND	BOOL	 	x	x	Starts bars at beginning of session (used for odd interval bars)
		//9007	CHART_STARTDATE	DATE	 	x	x	'1996/01/01' (TICKS table can request only one day at a time. The desired date is indicated in CHART_STOPDATE).
		//9009	CHART_STARTTIME	TIME	x	x	 	'14:00'
		//9008	CHART_STOPDATE	DATE	x	x	x	'1996/01/15'
		//9010	CHART_STOPTIME	TIME	x	x	 	'14:00'
		//9004	DAYS_BACK	INT	 	    x	x	'5' (number of days back for INTRADAY, and daily bars. Number of weeks or months back for weekly or monthly bar data.)
		//1003	DISP_NAME	STRING	    x	x	x	'IBM' (Symbol formats are datafeed-specific. Check your Feed Handler help file for the correct format.)
		//1016	TRD_DATE	DATE	    x	x	x	'1996/01/15'
		//1018	TRDTIM_1	TIME	    x	x	x	'14:00'
		//1044	TRDXID_1	STRING	    x	 	 	'N'

		//TOPIC = LIVECHART
		//TABLE = TICKS, INTRADAY, DAILY 
		//FID = 9020 codes only are valid in the WHAT clause:
		//Description Code 
		//Trade 0 
		//Bid 1 
		//Ask 2 
		//Regional bid  3 
		//Regional ask 4 
		//Deleted 11 
		//Inserted 12 
		//Irregular Delete 44 
		//FormT Trade 32 




		//Helper function for 
		public  void LIVECHARTSynchronousFileSinkQuery(klTALApplication a,string lTQL, StreamWriter sw)
		{
			klTALStreamAttributes tsb_qqqq_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL,sw);	
			//Make a query and call request
			klAsyncQuery laqq=new klAsyncQuery(tsb_qqqq_sync);
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			Hashtable sp=new Hashtable();
			sp.Add(1006,"TRDPRC_1");
			sp.Add(1016,"TRD_DATE");
			sp.Add(1178,"TRDVOL_1");
			sp.Add(1018, "TRDTIM_1");
			laqq.setTQLPersist(sp);
			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_OnTerminate);

			//Connect and subscribe - this uses the data processor class
			//a.ConnectAndSubscribe(tsb_qqqq_sync);

			//instead of connecting and subscribing to the stream - install the transactional callback.
			//Circumvent async iocp callback pool and direct to ADO transaction 			
			
			//Since We did not connect - we have to manually make connection
			////Connection livechartConnection =a.FindMyConnection("", "TA_SRV", "LIVECHART");
			///

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;
			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked


			Thread.Sleep(3000);


		}

		//Gets option symbols for an underlying.
		

		
		public  void LIVECHART_Callback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			
			StreamWriter sw=laq.myStreamWriter();//bbcremove debug code 

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable myTQLPersist=laq.myTQLPersist();
			
			IEnumerator  ie=row.GetEnumerator();

			myTQLPersist.GetEnumerator();

     			
		
			int rc=row.Count;
			int pc=myTQLPersist.Count;// row.Count; typically has more FID's that we want to write to the transaction
			int op=0;
			for(op=0;op<pc;op++)
			{ 
				
				string fid=(string)myTQLPersist[op];
				//sp.Add(1006,"TRDPRC_1");
				//sp.Add(1016,"TRD_DATE");
				//sp.Add(1178,"TRDVOL_1");

					


				//IEnumerator  ie=row.GetEnumerator();
				//.m_arFields[i];
			}


			int elements = 0;

			if (row["TRDPRC_1"] != null)
				elements = row["TRDPRC_1"].PriceVectorValue.Length;
			
			int first_element=0;
			
			//here we take out our vectors

			Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
			int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
			TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
			DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;

			sw.WriteLine("Date,Time,Trade Price,Trade Vol");
						
			for (int i = first_element; i < elements; i++)
			{

				//System.Console.WriteLine(String.Format("{0},{1}:{2},{3},{4},{5},{6},{7}",
				sw.WriteLine(String.Format("{0},{1}:{2},{3},{4}",
					TRD_DATE[i].ToString("yyyy/MM/dd"),
					TRDTIM_1[i].Hours.ToString("D2"),
					TRDTIM_1[i].Minutes.ToString("D2"),
					TRDPRC_1[i].ToString(),
					TRDVOL_1[i].ToString()));
						
						
			}
				
			sw.Flush();
		
		}


		public  void LIVECHART_OnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		//Tic Data is cached in local file system - then transacted to db in one shot.
		public  void LIVECHART_PrimeDBTicCallback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	

			//lock(laq)//Do a wait one - or threadblock here - 
			//{
			//We created this string from the TABLE and What/Where FID's
			//String TQL=laq.myTQL();
			
			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			//Unused here - but we will use this in the realitme filters 
			//Hashtable myTQLPersist=laq.myTQLPersist();
			//IEnumerator  ie=row.GetEnumerator();
			//myTQLPersist.GetEnumerator();

			Hashtable whatFID=laq.myWhatFID();//
			Hashtable whereFID=laq.myWhereFID();//


			//int rc=row.Count;
			//int pc=myTQLPersist.Count;// row.Count; typically has more FID's that we want to write to the transaction
			//int op=0;
			//for(op=0;op<pc;op++)
			//{ 
				
			//string fid=(string)myTQLPersist[op];
			//sp.Add(1006,"TRDPRC_1");
			//sp.Add(1016,"TRD_DATE");
			//sp.Add(1178,"TRDVOL_1");
			//IEnumerator  ie=row.GetEnumerator();
			//.m_arFields[i];
			//}

	
			//Here we take out our vectors.
			//We only pull what we need - but there is a hard coupling to the FID's in the TICKS,INTRADAY,DAILY tbles.
		
			//This is the set we currently support.
			
			//From TICKS table
			//			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			//			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44

			if(	whatFID.ContainsValue("TICK_TYPE"))
			{ //Only tick data uses this fid
				Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
				int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
				int elements = 0;
				
				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				if (row["TRDPRC_1"] != null)
					elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
				int first_element=0;

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						TRDPRC_1[i].ToString(),
						TRDVOL_1[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);

			}

			//From INTRADAY TABLE
			//			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			//			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			//			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");
			if(	whatFID.ContainsValue("NUM_MOVES"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] NUM_MOVES = row["NUM_MOVES"] == null ? null : row["NUM_MOVES"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;
				int first_element=0;

				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4},{5},{6},{7}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						OPEN_PRC[i].ToString(),
						HIGH_1[i].ToString(),
						LOW_1[i].ToString(),
						SETTLE[i].ToString(),
						NUM_MOVES[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);
			}
			


			//From Daily Hashtable
			//			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			//			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			//			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");
			if(	whatFID.ContainsValue("MONEY_FLOW"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] MONEY_FLOW = row["MONEY_FLOW"] == null ? null : row["MONEY_FLOW"].IntVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;

				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);


				int first_element=0;	
				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1},{2},{3},{4},{5}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						OPEN_PRC[i].ToString(),
						HIGH_1[i].ToString(),
						LOW_1[i].ToString(),
						SETTLE[i].ToString(),
						MONEY_FLOW[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);
				//laq.m_ARE.Set();

			}


		

		}

						 	
		public  void LIVECHART_PrimeDBTicOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		public void LIVECHARTSynchronousFileSinkQuery(klTALApplication a, ArrayList symbols )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");

			//Here we create hash tables for What and Where FID's for the types of ADO transactions.
			//TIC, INTRA,DAILY
			Hashtable TICKS_WhatFID_Hash=new Hashtable();
			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			TICKS_WhatFID_Hash.Add(1006,"TRDPRC_1");
			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");

			//TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");//Toss out the irregular trades --val=44
			
			Hashtable INTRADAY_WhatFID_Hash=new Hashtable();
			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");

			Hashtable DAILY_WhatFID_Hash=new Hashtable();
			DAILY_WhatFID_Hash.Add(1003,"DISP_NAME");
			DAILY_WhatFID_Hash.Add(1016,"TRD_DATE");
			DAILY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			DAILY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			DAILY_WhatFID_Hash.Add(1012,"HIGH_1");
			DAILY_WhatFID_Hash.Add(1013,"LOW_1");
			DAILY_WhatFID_Hash.Add(1070,"SETTLE");
			DAILY_WhatFID_Hash.Add(6025,"MONEY_FLOW");

					

			//For each symbol we make queries for 5days tic, 15days 5min, 2000 daily
			//the tic queries are cached until all five days are available - then the tic table is created.

			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
			
			
				//Get todays date - then get 5days tic, 15days 5min, 2000 daily.

				//TQL Item String components (TABLE;WHAT;WHERE)
			
				Hashtable TICKS_WhereFID_Hash=new Hashtable();
				
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();

				TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash.Add(9008,tvp);//For tic data only the end date is used to get the data
		 
				tvp=new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'");
				TICKS_WhereFID_Hash.Add(1003,tvp);


				Hashtable INTRADAY_WhereFID_Hash=new Hashtable();
				INTRADAY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				INTRADAY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				//INTRADAY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//INTRADAY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//INTRADAY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");
				INTRADAY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","15"));
				INTRADAY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","5"));//5min bars




				Hashtable DAILY_WhereFID_Hash=new Hashtable();
				DAILY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				DAILY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				DAILY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","5000"));
				DAILY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","0"));//0 for daily
				//DAILY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//DAILY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//DAILY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");

				//We need to construct the TQL for each query - the persist, what and where FID Hashes have been
				//set up at this point.  The query can expand on the FID's, but it can not leave any out.
				string dailyTQL=klTALApplication.TQLfromWhatWhereFIDHashes("DAILY", DAILY_WhereFID_Hash ,DAILY_WhatFID_Hash);

				string intradayTQL=klTALApplication.TQLfromWhatWhereFIDHashes("INTRADAY", INTRADAY_WhereFID_Hash ,INTRADAY_WhatFID_Hash);

				string tics1TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
							
				
				//Construct the Queries.
				//These queries bypass the file stream and use ADO transactions.
				klAsyncQuery dailyQuery=new klAsyncQuery(DAILY_WhatFID_Hash,DAILY_WhereFID_Hash);
			
				klAsyncQuery intradayQuery=new klAsyncQuery(INTRADAY_WhatFID_Hash,INTRADAY_WhereFID_Hash);

				//The TicQueries use a stream writer we instantiate amd manage here. 
				
				//Use the symbol, date items in the WhereFID_Hash to make filename
				string fileName = (string)symbols[symbolCount];
				string fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				StreamWriter lsw =new StreamWriter(fileName);


				//For now one for DAYS and INTRADAY 
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+= "_DAILY_"+fileDate;
				fileName+=".txt";
				StreamWriter dsw =new StreamWriter(fileName);

				fileName = (string)symbols[symbolCount];
				fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+= "_INTRADAY_"+fileDate;
				fileName+=".txt";
				StreamWriter isw =new StreamWriter(fileName);



                
				klAsyncQuery day1TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);				
				//Set the stream writer
				day1TicsQuery.setMyStreamWriter(lsw);

				//Change the date in the Where tvp fid hash
				//Date Arithematic
				System.TimeSpan sts=new TimeSpan(7,0,0,0);//days, hours, minutes, and seconds
								
				DateTime d2=d1.Subtract( sts);

				//A DayOfWeek enumerated constant that indicates the day of the week. 
				//This property value ranges from zero, indicating Sunday, to six, indicating Saturday.
				if( d2.DayOfWeek == (DayOfWeek)6 )
					d2=d2.AddDays(2);
				if( d2.DayOfWeek==0)
					d2=d2.AddDays(1);
				date=d2.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics2TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);
				klAsyncQuery day2TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d2.Month+"_"+d2.Day+"_"+d2.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day2TicsQuery.setMyStreamWriter(lsw);


				DateTime d3=d2.AddDays(1);
				if( d3.DayOfWeek==(DayOfWeek)6)
					d3=d3.AddDays(2);
				if(  (d3.DayOfWeek==0 ))
					d3=d3.AddDays(1);
				date=d3.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics3TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day3TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d3.Month+"_"+d3.Day+"_"+d3.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day3TicsQuery.setMyStreamWriter(lsw);				
				
				DateTime d4=d3.AddDays(1);
				if( d4.DayOfWeek==(DayOfWeek)6)
					d4=d4.AddDays(2);
				if(  (d4.DayOfWeek==0 ))
					d4=d4.AddDays(1);
				date=d4.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics4TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day4TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d4.Month+"_"+d4.Day+"_"+d4.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day4TicsQuery.setMyStreamWriter(lsw);


				DateTime d5=d4.AddDays(1);
				if( d5.DayOfWeek==(DayOfWeek)6)
					d5=d5.AddDays(2);
				if(  (d5.DayOfWeek==0 ))
					d5=d5.AddDays(1);
				date=d5.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics5TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day5TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d5.Month+"_"+d5.Day+"_"+d5.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day5TicsQuery.setMyStreamWriter(lsw);
				
				//This is safe if we're running when the US equitiy markets are open.
				//bbcrevisit - protect it later
				DateTime d6=d5.AddDays(1);
				if( d6.DayOfWeek==(DayOfWeek)6)
					d6=d6.AddDays(2);
				if(  (d6.DayOfWeek==0 ))
					d6=d6.AddDays(1);
				date=d6.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics6TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day6TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = (string)symbols[symbolCount];
				fileDate="_"+d6.Month+"_"+d6.Day+"_"+d6.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day6TicsQuery.setMyStreamWriter(lsw);

				//Temporary
				dailyQuery.setMyStreamWriter(dsw);
				intradayQuery.setMyStreamWriter(isw);

		
				//There were originally two types of callback used by this method - one that caches tics
				//and one that does the ADO transaction for INTRADAY and DAILY
				//AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(LIVECHART_PrimeDBCallback);
				//AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBOnTerminate);

				//Uses the stream writer to cache tics.  Then loads them up for the transaction. 
				AsyncQuery.DataHandler kladodhtic=new AsyncQuery.DataHandler(LIVECHART_PrimeDBTicCallback);
				AsyncQuery.TerminateHandler kladothtic=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBTicOnTerminate);


				//Set the query handler for all of the INTRADAY and DAILY queries 
				//				dailyQuery.OnAdviseData += kladodh;
				//				dailyQuery.OnRequestData += kladodh;
				//				dailyQuery.OnTerminate += kladoth;
				//
				//				intradayQuery.OnAdviseData += kladodh;
				//				intradayQuery.OnRequestData += kladodh;
				//				intradayQuery.OnTerminate += kladoth;

				//temporary - 
				dailyQuery.OnAdviseData += kladodhtic;
				dailyQuery.OnRequestData += kladodhtic;
				dailyQuery.OnTerminate += kladothtic;
				
				intradayQuery.OnAdviseData += kladodhtic;
				intradayQuery.OnRequestData += kladodhtic;
				intradayQuery.OnTerminate += kladothtic;
	
				//Set the query handler for all of the tic queries.
				day1TicsQuery.OnAdviseData += kladodhtic;
				day1TicsQuery.OnRequestData += kladodhtic;
				day1TicsQuery.OnTerminate += kladothtic;

				day2TicsQuery.OnAdviseData += kladodhtic;
				day2TicsQuery.OnRequestData += kladodhtic;
				day2TicsQuery.OnTerminate += kladothtic;

				day3TicsQuery.OnAdviseData += kladodhtic;
				day3TicsQuery.OnRequestData += kladodhtic;
				day3TicsQuery.OnTerminate += kladothtic;

				day4TicsQuery.OnAdviseData += kladodhtic;
				day4TicsQuery.OnRequestData += kladodhtic;
				day4TicsQuery.OnTerminate += kladothtic;

				day5TicsQuery.OnAdviseData += kladodhtic;
				day5TicsQuery.OnRequestData += kladodhtic;
				day5TicsQuery.OnTerminate += kladothtic;

				day6TicsQuery.OnAdviseData += kladodhtic;
				day6TicsQuery.OnRequestData += kladodhtic;
				day6TicsQuery.OnTerminate += kladothtic;


				//Add the TICKS queries
				livechartConnection.AttachQuery(day1TicsQuery);
				livechartConnection.AttachQuery(day2TicsQuery);
				livechartConnection.AttachQuery(day3TicsQuery);
				livechartConnection.AttachQuery(day4TicsQuery);
				livechartConnection.AttachQuery(day5TicsQuery);
				livechartConnection.AttachQuery(day6TicsQuery);

				//add the DAILY and INTRADAY queries
				livechartConnection.AttachQuery(dailyQuery);
				livechartConnection.AttachQuery(intradayQuery);

				//day2TicsQuery.Request("TICKS;1003,1016,1006,1018,1178;9008='03/16/2007',1003='QQQQ'", "TAL4");//Callback gets invoked
				//Thread.Sleep(30000);
				//"TICKS;1003,1016,1006,1018,1178;9008='03/15/2007',1003='QQQQ'"
				//"TICKS;1003,1016,1006,1018,1044,1178;9008='2007/03/15',1003='QQQQ'"
				//badtql	"INTRADAY;1077, 1003, 1070, 1019, 1018, 1016, 1013, 1012;9008='03/16/2007' , 9004='15' , 1003='QQQQ' ;"	
				//badtql 	tics2TQL	"TICKS;1018, 1016, 1003, 1178;9008='3/17/2007' , 1003='QQQQ' ;"	string
				//good tql
				//tics1TQL=				"TICKS;1003,1016,1006,1018,1178;9008='03/15/2007',1003='QQQQ'";
				//tics2TQL=				"TICKS;1003,1016,1006,1018,1178;9008='03/14/2007',1003='QQQQ'";
				//tics3TQL=				"TICKS;1003,1016,1006,1018,1178;9008='03/13/2007',1003='QQQQ'";
				//tics4TQL=				"TICKS;1003,1016,1006,1018,1178;9008='03/12/2007',1003='QQQQ'";
				//tics5TQL=				"TICKS;1003,1016,1006,1018,1178;9008='03/09/2007',1003='QQQQ'";
	





				//This is a hateful way to do it. bbcrevisit - this needs to work - 
				//AutoResetEvent hData =new AutoResetEvent(true);
				//dailyQuery.m_ARE=hData;


				dailyQuery.Request(dailyTQL+" ","TAL4");
				//				//hData.Reset();
				Thread.Sleep(5000);
				dsw.Flush();
				dsw.Close();
						
				intradayQuery.Request(intradayTQL+" ","TAL4");
				Thread.Sleep(1000);
				isw.Flush();
				isw.Close();
	
			
				day6TicsQuery.Request(tics6TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day6TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
				
				day5TicsQuery.Request(tics5TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day5TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();			
				
				day4TicsQuery.Request(tics4TQL+" ", "TAL4");

				Thread.Sleep(5000);
				lsw=day4TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();
					
				day3TicsQuery.Request(tics3TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day3TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
					
				day2TicsQuery.Request(tics2TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day2TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
				
				day1TicsQuery.Request(tics1TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day1TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
				
				
				;
		
				


				
				

				//intradayQuery.Request(,"TAL4");

				//close the file cache for tic data 
				
				//lsw.Close();

				//send the filename and FID info to file to ADO method




				//Wait for all queries to end before returning





			}


		}


	}

		
	public class klLiveChartTicToNas :klFileSource
	{

		public klLiveChartTicToNas() :base("")
		{
		}

		
		public  void LIVECHART_TicToNASCallback(object sender, DataEventArgs e)
		{
			klAsyncQuery laq=(klAsyncQuery)sender;	

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable whatFID=laq.myWhatFID();
			Hashtable whereFID=laq.myWhereFID();

			if(	whatFID.ContainsValue("TICK_TYPE"))
			{
				Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
				int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
				int elements = 0;
				
				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				if (row["TRDPRC_1"] != null)
					elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
				int first_element=0;

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						TRDPRC_1[i].ToString(),
						TRDVOL_1[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);

			}

			



		

		}

						 	
		public  void LIVECHART_TicToNASOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		public void LIVECHARTSynchronousFileSinkQuery(klTALApplication a, ArrayList symbols,string baseFileName )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");

			Hashtable TICKS_WhatFID_Hash=new Hashtable();
			TICKS_WhatFID_Hash.Add(1003,"DISP_NAME");
			TICKS_WhatFID_Hash.Add(1016,"TRD_DATE");
			TICKS_WhatFID_Hash.Add(1018,"TRDTIM_1");
			TICKS_WhatFID_Hash.Add(1178,"TRDVOL_1");
			TICKS_WhatFID_Hash.Add(1006,"TRDPRC_1");
			TICKS_WhatFID_Hash.Add(9020,"TICK_TYPE");

			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
			
			
				Hashtable TICKS_WhereFID_Hash=new Hashtable();
				
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();

				TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash.Add(9008,tvp);//For tic data only the end date is used to get the data
		 
				tvp=new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'");
				TICKS_WhereFID_Hash.Add(1003,tvp);
		
				string tics1TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
	
				System.TimeSpan sts=new TimeSpan(7,0,0,0);//days, hours, minutes, and seconds
								
				DateTime d2=d1.Subtract( sts);

				//A DayOfWeek enumerated constant that indicates the day of the week. 
				//This property value ranges from zero, indicating Sunday, to six, indicating Saturday.
				if( d2.DayOfWeek == (DayOfWeek)6 )
					d2=d2.AddDays(2);
				if( d2.DayOfWeek==0)
					d2=d2.AddDays(1);
				date=d2.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics2TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);
				klAsyncQuery day2TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				string fileName = baseFileName;
				fileName +="/"+ (string)symbols[symbolCount];
				string fileDate="_"+d2.Month+"_"+d2.Day+"_"+d2.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				StreamWriter lsw =new StreamWriter(fileName);
				day2TicsQuery.setMyStreamWriter(lsw);


				DateTime d3=d2.AddDays(1);
				if( d3.DayOfWeek==(DayOfWeek)6)
					d3=d3.AddDays(2);
				if(  (d3.DayOfWeek==0 ))
					d3=d3.AddDays(1);
				date=d3.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics3TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day3TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = baseFileName;
				fileName +="/"+ (string)symbols[symbolCount];
				fileDate="_"+d3.Month+"_"+d3.Day+"_"+d3.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day3TicsQuery.setMyStreamWriter(lsw);				
				
				DateTime d4=d3.AddDays(1);
				if( d4.DayOfWeek==(DayOfWeek)6)
					d4=d4.AddDays(2);
				if(  (d4.DayOfWeek==0 ))
					d4=d4.AddDays(1);
				date=d4.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics4TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day4TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = baseFileName;
				fileName +="/"+ (string)symbols[symbolCount];
				fileDate="_"+d4.Month+"_"+d4.Day+"_"+d4.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day4TicsQuery.setMyStreamWriter(lsw);


				DateTime d5=d4.AddDays(1);
				if( d5.DayOfWeek==(DayOfWeek)6)
					d5=d5.AddDays(2);
				if(  (d5.DayOfWeek==0 ))
					d5=d5.AddDays(1);
				date=d5.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics5TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day5TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = baseFileName;
				fileName +="/"+(string)symbols[symbolCount];
				fileDate="_"+d5.Month+"_"+d5.Day+"_"+d5.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day5TicsQuery.setMyStreamWriter(lsw);
				
				//This is safe if we're running when the US equitiy markets are open.
				//bbcrevisit - protect it later
				DateTime d6=d5.AddDays(1);
				if( d6.DayOfWeek==(DayOfWeek)6)
					d6=d6.AddDays(2);
				if(  (d6.DayOfWeek==0 ))
					d6=d6.AddDays(1);
				date=d6.ToShortDateString();
				tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash[9008]=tvp;
				string tics6TQL=klTALApplication.TQLfromWhatWhereFIDHashes("TICKS", TICKS_WhereFID_Hash ,TICKS_WhatFID_Hash);			
				klAsyncQuery day6TicsQuery=new klAsyncQuery(TICKS_WhatFID_Hash,TICKS_WhereFID_Hash);
				fileName = baseFileName;
				fileName +="/"+ (string)symbols[symbolCount];
				fileDate="_"+d6.Month+"_"+d6.Day+"_"+d6.Year;;
				fileName+="_TICKS_"+fileDate;
				fileName+=".txt";
				lsw =new StreamWriter(fileName);
				day6TicsQuery.setMyStreamWriter(lsw);

				AsyncQuery.DataHandler kladodhtic=new AsyncQuery.DataHandler(LIVECHART_TicToNASCallback);
				AsyncQuery.TerminateHandler kladothtic=new AsyncQuery.TerminateHandler(LIVECHART_TicToNASOnTerminate);

				//day1TicsQuery.OnAdviseData += kladodhtic;
				//day1TicsQuery.OnRequestData += kladodhtic;
				//day1TicsQuery.OnTerminate += kladothtic;

				day2TicsQuery.OnAdviseData += kladodhtic;
				day2TicsQuery.OnRequestData += kladodhtic;
				day2TicsQuery.OnTerminate += kladothtic;

				day3TicsQuery.OnAdviseData += kladodhtic;
				day3TicsQuery.OnRequestData += kladodhtic;
				day3TicsQuery.OnTerminate += kladothtic;

				day4TicsQuery.OnAdviseData += kladodhtic;
				day4TicsQuery.OnRequestData += kladodhtic;
				day4TicsQuery.OnTerminate += kladothtic;

				day5TicsQuery.OnAdviseData += kladodhtic;
				day5TicsQuery.OnRequestData += kladodhtic;
				day5TicsQuery.OnTerminate += kladothtic;

				day6TicsQuery.OnAdviseData += kladodhtic;
				day6TicsQuery.OnRequestData += kladodhtic;
				day6TicsQuery.OnTerminate += kladothtic;

				//livechartConnection.AttachQuery(day1TicsQuery);
				livechartConnection.AttachQuery(day2TicsQuery);
				livechartConnection.AttachQuery(day3TicsQuery);
				livechartConnection.AttachQuery(day4TicsQuery);
				livechartConnection.AttachQuery(day5TicsQuery);
				livechartConnection.AttachQuery(day6TicsQuery);

				day6TicsQuery.Request(tics6TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day6TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
				
				day5TicsQuery.Request(tics5TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day5TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();			
				
				day4TicsQuery.Request(tics4TQL+" ", "TAL4");

				Thread.Sleep(5000);
				lsw=day4TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();
					
				day3TicsQuery.Request(tics3TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day3TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
					
				day2TicsQuery.Request(tics2TQL+" ", "TAL4");
				Thread.Sleep(5000);
				lsw=day2TicsQuery.myStreamWriter();
				lsw.Flush();lsw.Close();	
				
				//day1TicsQuery.Request(tics1TQL+" ", "TAL4");
				//Thread.Sleep(5000);
				//lsw=day1TicsQuery.myStreamWriter();
				//lsw.Flush();lsw.Close();	
	
			}
		}
	}


	public class klOptionsChainFileSource :klFileSource
	{
		//First query QUOTECHAIN for symbols, then do a request (not advise) on LIVEQUOTE table for the data.

		//Options FIDS
		//1067 EXPIR_DATE DATE The date on which the future or option expires 
		//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
		//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
		//1064 OPINT_1 LONG Open interest 
		//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
		//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
		//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 
		//We have to set the LIVEQUOTE WhereFID when doing the request for price data
		//FID Label Type Format 
		//2000 STYP INT '5' 
		//Here's the table of types
		//2000 (STYP)
		//TOPIC = LIVECHART or LIVEQUOTE 
		//TABLE = TICKS or LIVEQUOTE 
		//FID = 2000 codes are valid in both the WHAT and WHERE clauses:
		//Instrument Type Label Code 
		//Stock T_IT_STOCK 1 
		//Stock Option T_IT_STOCKOPT 2 
		//Future T_IT_FUTURE 3 
		//Future Option T_IT_FUTOPT 4 
		//Spot T_IT_SPOT 5 
		//Warrant T_IT_WARRANT 6 
		//Corp. Bond T_IT_CORPBOND 7 
		//Mutual Fund T_IT_MUTFUND 8 
		//Gov't Bond T_IT_GOVTBOND 9 
		//Index T_IT_INDEX 10 
		//Muni Bond T_IT_MUNIBOND 11 
		//News   12 
		//Unused   13 
		//Statistics T_IT_STATISTICS 14 
		//Money Mkt. Fund T_IT_MMKTFUND 15 
		//Unspecified Bond T_IT_UNSPECBOND 16 
		//Unspecified Fund T_IT_UNSPECFUND 17 
		//Miscellaneous T_IT_MISC 18 
		//SingleStock Future T_IT_SINGLE_STOCK_FUTURE 19 
		//Forex T_IT_FOREX 20 
		//Synthetic T_IT_SYNTHETIC 21 
		//Top 10 List T_IT_CHAIN 22 

		public klOptionsChainFileSource() :base("")
		{

			m_QueryHash= new Hashtable();
			m_ThreadHash= new Hashtable();
		}

		//This guy needs to be a hash table as well.
		//One thread per query.
		private Thread t;

		//Queries and threads are matched 1:1 by TQL.
		//Do not duplicate TQL queries in an instance of this class.
		private Hashtable m_QueryHash;
		private Hashtable m_ThreadHash;

		private long GetItemKey(string szItem)
		{
			return(szItem.GetHashCode());
				
		}

		private Connection livequoteConnection;

		//Options FIDS
		//1067 EXPIR_DATE DATE The date on which the future or option expires 
		//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
		//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
		//1064 OPINT_1 LONG Open interest 
		//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
		//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
		//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 
		//We have to set the LIVEQUOTE WhereFID when doing the request for price data
		//FID Label Type Format 
		//2000 STYP INT '5' 
		//Here's the table of types
		//2000 (STYP)
		//TOPIC = LIVECHART or LIVEQUOTE 
		//TABLE = TICKS or LIVEQUOTE 
		//FID = 2000 codes are valid in both the WHAT and WHERE clauses:
		//Instrument Type Label Code 
		//Stock T_IT_STOCK 1 
		//Stock Option T_IT_STOCKOPT 2 
		//Future T_IT_FUTURE 3 
		//Future Option T_IT_FUTOPT 4 
		//Spot T_IT_SPOT 5 
		//Warrant T_IT_WARRANT 6 
		//Corp. Bond T_IT_CORPBOND 7 
		//Mutual Fund T_IT_MUTFUND 8 
		//Gov't Bond T_IT_GOVTBOND 9 
		//Index T_IT_INDEX 10 
		//Muni Bond T_IT_MUNIBOND 11 
		//News   12 
		//Unused   13 
		//Statistics T_IT_STATISTICS 14 
		//Money Mkt. Fund T_IT_MMKTFUND 15 
		//Unspecified Bond T_IT_UNSPECBOND 16 
		//Unspecified Fund T_IT_UNSPECFUND 17 
		//Miscellaneous T_IT_MISC 18 
		//SingleStock Future T_IT_SINGLE_STOCK_FUTURE 19 
		//Forex T_IT_FOREX 20 
		//Synthetic T_IT_SYNTHETIC 21 
		//Top 10 List T_IT_CHAIN 22 

		//First get the options Symbols from QUOTECHAIN table and then do a LIVEQUOTE request -not an advise!
		public  void LIVEQUOTEOptionsSyncFileSinkQuery(klTALApplication a, StreamReader sr,Hashtable WhatFID,	Hashtable WhereFID,int howMany,StreamWriter sw)
		{
			string symbol;
			long filen= sr.BaseStream.Length;
			for(int i=0;i<howMany;i++)
			{
				symbol=sr.ReadLine();
				DateTime d1 = DateTime.Now;

				string fileName = symbol;
				string fileDate="_OPTION_STREAM"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+=fileDate;
				fileName+="_.txt";
					
				
				//Set the symbol element of the TVP 
				TQLTypeValuePair tvp=(TQLTypeValuePair)WhereFID[1003];
				tvp.m_TQLValue=symbol;

				string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);

				klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL,sw);
				
				klAsyncQuery laqq = new klAsyncQuery(tsb);
				
				if(i==0)//fix this
					livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
				AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);
				AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_OptionsOnTerminate);
			
				laqq.OnAdviseData += kladodh;
				laqq.OnRequestData += kladodh;
				laqq.OnTerminate += kladoth;

				//laqq.setMyStreamWriter(sw);
				
				addQuery(laqq,lTQL);
					
				//Nope - we have to 
				//a.ConnectAndSubscribe(tsb);
				Thread.Sleep(1000);
				//lsw=laqq.myStreamWriter();
				//lsw.Flush();
				//lsw.Close();

			}


		}


		public  void QUOTECHAIN_Callback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			int howMany=0;
			
			StreamWriter sw=laq.myStreamWriter();//bbcremove debug code 

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

		

			IEnumerator  blkEnumerator=blk.GetEnumerator();
		
			for(int i=0;i<blk.Count;i++)
			{
				blkEnumerator.MoveNext();



				Type mt= blkEnumerator.GetType();

				string moo=blkEnumerator.Current.ToString();


				//for(int j=0;j<blkEnumerator.Current.ToString()

				IRow rowl =(IRow)blkEnumerator.Current;

				IEnumerator  rowlEnumerator=rowl.GetEnumerator();	

				for(int j=0;j<rowl.Count;j++)

				{
					rowlEnumerator.MoveNext();

					Type mtr= rowlEnumerator.GetType();
					Field mlf=(Field)rowlEnumerator.Current;
				
					Type mtrr=mlf.GetType();

					string mlfn=mlf.Name;
				
				
					//string mlfs="'"+mlf.StringValue+"'";
					string mlfs=mlf.StringValue;

					//I thought the + before the symbol was a problem.  
					//Development when the markets are closed is difficult at times.
					//System.Diagnostics.Debug.WriteLine( mlfn );
					//int mo=mlfs.Length;mlfs=mlfs.Substring(1,mo-1);
					//System.Diagnostics.Debug.WriteLine( mlfs );
				
					howMany++;

					sw.WriteLine(mlfs);
					

					//Connect a query					 
			
				}
			
			}

			

			laq.m_howMany=howMany;

		
			sw.Flush();
		


		}


		public  void QUOTECHAIN_OnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}

		
		public  int QUOTECHAINSynchronousFileSinkQuery(klTALApplication a,string lTQL, StreamWriter sw)
		{
			//Options are only available through LIVEQUOTE.  
			//Use QUOTECHAIN;1003;6050='QQQQ' to get all symbols. 
			//Options FIDS
			//1067 EXPIR_DATE DATE The date on which the future or option expires 
			//1066 STRIKE_PRC LPACK Strike price (the price at which the option is exercisable 
			//6050 OPTION_ROOT STRING Option root encompassing all expiration months and instrument types 
			//1064 OPINT_1 LONG Open interest 
			//6202 OPT_CALC_TYPE STRING Option calculation type (eur/amer, rate-based/yield-based 
			//6203 OPT_EXPIR_INDEX INT 0 for regular, 1 for short-dated 
			//1109 PUTCALLIND STRING Indicates whether option is a put or a call or an underlier (P = put, C = call, U = underlier 

			int howMany=0;

			klTALStreamAttributes tsb_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL,sw);	
			//Make a query and call request
			klAsyncQuery laqq=new klAsyncQuery(tsb_sync);
			//Set the items we would like the query to persist.
			//Some kl queries that filter realtime will persis a subset of the data so
			//there is a container of the TQL what FIDs and where FID's this query will be persisting
			

			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(QUOTECHAIN_Callback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(QUOTECHAIN_OnTerminate);

			//Connect and subscribe - this uses the data processor class
			//a.ConnectAndSubscribe(tsb_qqqq_sync);

			//instead of connecting and subscribing to the stream - install the transactional callback.
			//Circumvent async iocp callback pool and direct to ADO transaction 			
			
			//Since We did not connect - we have to manually make connection
			////Connection livechartConnection =a.FindMyConnection("", "TA_SRV", "LIVECHART");
			///

			Connection livechartConnection = a.GetConnection("", "TA_SRV", "QUOTECHAIN");
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;
			livechartConnection.AttachQuery(laqq);
			laqq.Request(lTQL, "TAL4");//Callback gets invoked


			Thread.Sleep(3000);

			//This is a bad thing fix the threading!
			howMany=laqq.m_howMany;
			
			return howMany;

		}

		
			
		public  void LIVEDATA_OptionsCallback(object sender, DataEventArgs e)
		{
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
	
			StreamWriter lsw=laq.myStreamWriter();
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
					lsw.WriteLine(moo);
					lsw.Flush();
									
				}
			}


			

	
		}


		public  void LIVEDATA_OptionsOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}


		private void ThreadMain()
		{
			System.Console.WriteLine("Threadmain");


			while(m_bRunning)
			{
				Thread myThread=System.Threading.Thread.CurrentThread;
				string myName=myThread.Name;
				System.Console.WriteLine(myName);



				Thread.Sleep(1000);
			}
				 
		}

		private void SetupForData(object sender)
		{
			string TQL;//

			klAsyncQuery laq=(klAsyncQuery)sender;
					
			TQL=laq.myTQL();
				
			//
					
			StreamWriter lsw=laq.myStreamWriter();
					
			//
				
			ThreadStart ts = new ThreadStart(ThreadMain);
			t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			t.Start();  //DataProc gets called when there's a message after this gets fired.

			laq.m_bReadyForData = true;//This query is marked ready for data.
		}

		//bbcrevisit Verify dated comments
		public void addQuery(klAsyncQuery AQ,string szItem)
		{
			//The query was initialized and added to the connection by the application class klLiveDataApp
			//Now we add it to the hash table of queries associated with this data processor.
			//Again the benefit of having the app manage the connection-query relationship is the data stream
			//is now decoupled from the device.  We can re-direct data streams on the fly by the TQL - AsyncQuery relationship.
			//AQ.Advise(szItem, "TAL4");
				

			//TLS Solution - each klAsync***Query Gets it's own stream - we create the named TLS entry based on the TQL query - hence the Dig This comment - not to add the same Query twice to a klLiveData***Processor instance.
			//Threads use a local store memory mechanism to store thread-specific data. 
			//The common language runtime allocates a multi-slot data store array to each process when it is created. 
			//The thread can allocate a data slot in the data store, store and retrieve a data value in the slot, 
			//and free the slot for reuse after the thread expires. Data slots are unique per thread. No other thread 
			//(not even a child thread) can get that data.
			//AllocateDataSlot  Allocates an unnamed data slot on all the threads. 
			//AllocateNamedDataSlot  Allocates a named data slot on all threads. 
			//LocalDataStoreSlot
			//LocalDataStoreSlot ldts=System.Threading.Thread.AllocateNamedDataSlot(szItem);
			//Now there's a 1:1 relation between TQL and IOCP threads used.
			//StreamWriter lsw=AQ.myStreamWriter();
			//System.Threading.Thread.SetData(ldts,lsw );

			//The thread is named with the TQL string and we use the thread name to retrieve the TLS object.
			//This could be a little cleaner.
				
			//Sets the handles to Query callbacks.  Same callback used for all queries used by an instance of klLiveData***Processor. 
			//AQ.OnAdviseData +=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);

			//AQ.OnOtherAck +=new AsyncQuery.AckHandler(m_query_OnOtherAck);

			long lKey=GetItemKey(AQ.myTQL());

			m_QueryHash.Add(lKey,AQ);
				
			string TQL=AQ.myTQL();
				
			//
					
								
			livequoteConnection.AttachQuery(AQ);
	
			AQ.Request(TQL, "TAL4");//Callback gets invoked
			//Thread.Sleep(500);
				
			//ThreadStart ts = new ThreadStart(ThreadMain);
			//t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			//t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			//t.Start();  //DataProc gets called when there's a message after this gets fired.

			//AQ.m_bReadyForData = true;//This query is marked ready for data.

			m_bRunning = true;//This means there is at least one query attached a connection in this data processor.

		}

		bool m_bRunning;
            
		private Hashtable m_AutoResetEventHash;
			
		private void m_query_OnOtherAck(object sender, AckEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;

			if (1 == e.Status)
			{
				if (!(laq.m_bReadyForData))
				{
					//We have to set up the stream object and get it into the thread for DataProc?
					//This is bad.  It seems like there ultimately needs to be a binding between TQL C# function
					//and data processing streams.  
					//There needs to be a way to fill a hash table of thread mains written for each TQL query. 
					//
					//sender is always the query type associated with this klLiveData***Processor
					SetupForData(sender);
				}
			}
		}
			

		
	}


	public class klVolatilitySurface :  klFileSource
	{
		Hashtable m_symbolHash;

		public String m_baseFileName;

		//This method fills the underlying price into the klOptionsInstance Hashtable
		public  void LIVEDATA_EquityCallback(object sender, DataEventArgs e)
		{
			
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;

			string symbol="";
			
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					if(fld.FieldInfo.ID==1003)//symbol
					{
						symbol= fld.StringValue;
					}
				}
			}

			double price=0;		

			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
					if(fld.FieldInfo.ID==1006)//Price
					{
						Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
//						int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
//						TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
//						DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
						int elements = 0;
//				
						if (row["TRDPRC_1"] != null)
							elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
						int first_element=0;
						
						//decimal lp=TRDPRC_1[i].DecimalValue;
					
						//kloi.m_price=(double)lp;
//						
						price=fld.DoubleValue;
						
						//string sp=	TRDPRC_1.ToString();
						//price=(double)TypeDescriptor.GetConverter(price).ConvertFrom(sp);
						
						
						for (int i = first_element; i < elements; i++)
						{
//							//TRD_DATE[i].ToString("yyyy/MM/dd"),
//							//TRDTIM_1[i].Hours;
//							//TRDTIM_1[i].Minutes;
//							//TRDVOL_1[i].ToString();
							decimal lp=TRDPRC_1[i].DecimalValue;
							price=(double)lp;
						}
					}
				}
			}
			//First get the klOptionInstance from the hastable
			klOptionInstance kloi=null;
			//Run through the hashtable putting in underlying price and symbol 
				IDictionaryEnumerator symIdx=m_symbolHash.GetEnumerator();
				for(int i=0;i<m_symbolHash.Count;i++)
				{
					symIdx.MoveNext();
					System.Type tpo=symIdx.Current.GetType();
					//klOptionInstance ko=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
					kloi=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
					kloi.m_underlying=symbol;
					kloi.m_underlyingPrice=price;
					DateTime nki=DateTime.Now;
					kloi.m_now=nki;
												
				}				
			
				

		}


		public  void LIVEDATA_OptionsCallback(object sender, DataEventArgs e)
		{
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;

			string symbol="";
			//1003,"DISP_NAME"
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					if(fld.FieldInfo.ID==1003)//symbol
					{
						symbol= fld.StringValue;
					}
				}
			}

			//First get the klOptionInstance from the hastable
			klOptionInstance kloi=null;
			if(m_symbolHash.Contains(symbol))
			{

				IDictionaryEnumerator symIdx=m_symbolHash.GetEnumerator();
				for(int i=0;i<m_symbolHash.Count;i++)
				{
					symIdx.MoveNext();
					string lKey=(string)symIdx.Key;
					if (lKey==symbol)
					{
						System.Type tpo=symIdx.Current.GetType();
						//klOptionInstance ko=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
						kloi=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
						
					}				
				}				
			}

			if(kloi==null)
				throw new  System.Exception("klVolatilitySurface::LIVEDATA_OptionsCallback ERROR retrieving symbol.");


			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
//					WhatFID.Add(1016,"TRD_DATE");
//					WhatFID.Add(1018,"TRDTIM_1");
//					WhatFID.Add(1178,"TRDVOL_1");
//					WhatFID.Add(1006,"TRDPRC_1");
//					WhatFID.Add(1022,"BID");
//					WhatFID.Add(1025,"ASK");
//					//WhatFID.Add(9020,"TICK_TYPE");
//					WhatFID.Add(1067,"EXPIR_DATE");
//					WhatFID.Add(1066 ,"STRIKE_PRC");
//					WhatFID.Add(1064, "OPINT_1");
//					1109	PUTCALLIND	STRING
					//First some notes about type converters
					//DateTime dt=new DateTime(1990,5,6);
					//Console.WriteLine(TypeDescriptor.GetConverter(dt).ConvertTo(dt, typeof(string)));
					//string myStr="1991-10-10";
					//Console.WriteLine(TypeDescriptor.GetConverter(dt).ConvertFrom(myStr));

					//Fill in the kloi 
					if(fld.FieldInfo.ID==1109)//Inst Type
					{
						string inst="";
						inst=(string)TypeDescriptor.GetConverter(inst).ConvertFrom(fld.StringValue);
						kloi.m_instrumentType=inst;

					}

					if(fld.FieldInfo.ID==1067)//T
					{
        				//DateTime d=DateTime.Parse(fld.StringValue);
						DateTime d=new DateTime();
						d=(DateTime)TypeDescriptor.GetConverter(d).ConvertFrom(fld.StringValue);
						kloi.m_expiry=d;
					}
					if(fld.FieldInfo.ID==1066)//K
					{
						double strike=0;
						strike=(double)TypeDescriptor.GetConverter(strike).ConvertFrom(fld.StringValue);
						kloi.m_strike=strike;					
		
					}

					if(fld.FieldInfo.ID==1064)//OI
					{
						int OI=0;
						OI=(int)TypeDescriptor.GetConverter(OI).ConvertFrom(fld.StringValue);
						
						kloi.m_openInterest=OI;

					}
					if(fld.FieldInfo.ID==1022)//BID
					{
						double BID=0;
						BID=(double)TypeDescriptor.GetConverter(BID).ConvertFrom(fld.StringValue);
						kloi.m_bid=BID;

					}
					if(fld.FieldInfo.ID==1025)//ASK
					{
						double ASK=0;
						ASK=(double)TypeDescriptor.GetConverter(ASK).ConvertFrom(fld.StringValue);
						kloi.m_bid=ASK;

					}
					if(fld.FieldInfo.ID==1006)//Price
					{
						Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
						int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
						TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
						DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
						int elements = 0;
				
						if (row["TRDPRC_1"] != null)
							elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
						int first_element=0;

						for (int i = first_element; i < elements; i++)
						{
							//TRD_DATE[i].ToString("yyyy/MM/dd"),
							//TRDTIM_1[i].Hours;
							//TRDTIM_1[i].Minutes;
							//TRDVOL_1[i].ToString();
							decimal lp=TRDPRC_1[i].DecimalValue;
							kloi.m_price=(double)lp;
						}
					}


					
					
				
									
				}
			}


			

	
		}


		public  void LIVEDATA_OptionsOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}

		public  void LIVEDATA_EquityOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}

		public Hashtable makeSymbolHash(klTALApplication a,String fileName)
		{

			Hashtable OPTION_HASH=new Hashtable();
			m_symbolHash=OPTION_HASH;

			Hashtable WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1016,"TRD_DATE");
			WhatFID.Add(1018,"TRDTIM_1");
			WhatFID.Add(1178,"TRDVOL_1");
			WhatFID.Add(1006,"TRDPRC_1");
			WhatFID.Add(1022,"BID");
			WhatFID.Add(1025,"ASK");
								
			//WhatFID.Add(9020,"TICK_TYPE");
			WhatFID.Add(1067,"EXPIR_DATE");
			WhatFID.Add(1066 ,"STRIKE_PRC");
			WhatFID.Add(1064, "OPINT_1");
			WhatFID.Add(1109, "PUTCALLIND");						
			Hashtable WhereFID=new Hashtable();
			TQLTypeValuePair tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			WhereFID.Add(1003,tvp);
								
			//This sets the LIVEQUOTE instrument type FID.
			tvp=new TQLTypeValuePair("STYP","'2'");
			WhereFID.Add(2000,tvp);		
											
			DateTime d1 = DateTime.Now;
			string date=d1.Month+"_"+d1.Day+"_"+d1.Year;//d1.ToShortDateString();
			string time= d1.TimeOfDay.Hours.ToString()+ d1.TimeOfDay.Minutes.ToString();//d1.ToShortTimeString();
			
			StreamReader sr=new StreamReader(fileName);
			
			string symbol;
			
			long howMany =0;

			bool eof=false;
			string line="";
			while(!eof)
			{
				line=sr.ReadLine();
				//Remove ' at beginning and end of line
				if(line!=null)
				{
					howMany++;
				}
				else
				{
					eof=true;
								
				}
			}
			String underlyingSymbol=null;
			sr.Close();//bbcrevisit thi
			sr=new StreamReader(fileName);
			//sr.BaseStream.Seek(0,0);

			for(int i=0;i<howMany-1;i++)
			{
				symbol=sr.ReadLine();

				klOptionInstance kloi=new klOptionInstance();
				
				kloi.m_symbol=symbol;
							
				OPTION_HASH.Add(symbol,kloi);
			}
			
			underlyingSymbol=sr.ReadLine();


		
			sr.Close();//bbcrevisit thi
			sr=new StreamReader(fileName);
			//remove last symbol since it is the option root 
			howMany-=1 ;
			for(int i=0;i<howMany;i++)
			{
				symbol=sr.ReadLine();
					
				//DateTime d1 = DateTime.Now;
				 d1 = DateTime.Now;

				//Set the symbol element of the TVP 
				tvp=(TQLTypeValuePair)WhereFID[1003];
				tvp.m_TQLValue="'"+ symbol+ "'";
				string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);

				klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL);
				
				klAsyncQuery laqq = new klAsyncQuery(tsb);
				
				Connection livequoteConnection;

				livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
				AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);
				AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_OptionsOnTerminate);
			
				laqq.OnAdviseData += kladodh;
				laqq.OnRequestData += kladodh;
				laqq.OnTerminate += kladoth;

				//long lKey=GetItemKey(laqq.myTQL());

				//m_QueryHash.Add(lKey,laqq);
				
				string TQL=laqq.myTQL();
						
				livequoteConnection.AttachQuery(laqq);
	
				laqq.Request(TQL, "TAL4");
					
				Thread.Sleep(100);

				//Now ... is there in HashTable

			}

			//Now put underlyingSymbol in hash
			//And query for last sale price
			//Since it's a livequote query - caution is advised on runnning this at night
			//bbcrevisit
		{
		
			WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1006,"TRDPRC_1");
																	
			WhereFID=new Hashtable();
			tvp=new TQLTypeValuePair("DISP_NAME","");
			WhereFID.Add(1003,tvp);
			tvp=(TQLTypeValuePair)WhereFID[1003];
			tvp.m_TQLValue="'"+ underlyingSymbol+ "'";
			string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);
			klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL);
			klAsyncQuery laqq = new klAsyncQuery(tsb);
			Connection livequoteConnection;
			livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
			AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_EquityCallback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_EquityOnTerminate);
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;

			string TQL=laqq.myTQL();
						
			livequoteConnection.AttachQuery(laqq);
	
			laqq.Request(TQL, "TAL4");
					
			Thread.Sleep(5000);	
		}
		
			
//			IDictionaryEnumerator hashIdx=OPTION_HASH.GetEnumerator();
//			for(int i=0;i<OPTION_HASH.Count;i++)
//			{
//				hashIdx.MoveNext();
//				klOptionInstance kloi=(klOptionInstance)hashIdx.Value;
//				kloi.m_underlying=underlyingSymbol;
//
//                
//			}
			//XmlSerializer mySerializer = new XmlSerializer(typeof(Hashtable));
			

			XmlSerializer serializer = new 	XmlSerializer(typeof(Hashtable));
			// To write to a file, create a StreamWriter object.
			StreamWriter sw = new StreamWriter("OPTION_HASH.xml");
			serializer.Serialize(sw, OPTION_HASH);
			
			//FileStream myFileStream = 	new FileStream("myFileName.xml", FileMode.Open);
			// Calls the Deserialize method and casts to the object type.
			//myObject = (MySerializableClass)mySerializer.Deserialize(myFileStream);

			return OPTION_HASH;
		}

	}



	public class klLiveDataVolatilitySurface 
	{
		Hashtable m_symbolHash;

		public String m_baseFileName;

		//This method fills the underlying price into the klOptionsInstance Hashtable
		public  void LIVEDATA_EquityCallback(object sender, DataEventArgs e)
		{
			
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;

			string symbol="";
			
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					if(fld.FieldInfo.ID==1003)//symbol
					{
						symbol= fld.StringValue;
					}
				}
			}

			double price=0;		

			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
					if(fld.FieldInfo.ID==1006)//Price
					{
						//						Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
						//						int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
						//						TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
						//						DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
						//						int elements = 0;
						//				
						//						if (row["TRDPRC_1"] != null)
						//							elements = row["TRDPRC_1"].PriceVectorValue.Length;
						//				
						//						int first_element=0;
						//						
						price=fld.DoubleValue;
						
						//string sp=	TRDPRC_1.ToString();
						//price=(double)TypeDescriptor.GetConverter(price).ConvertFrom(sp);
						
						
						//						for (int i = first_element; i < elements; i++)
						//						{
						//							//TRD_DATE[i].ToString("yyyy/MM/dd"),
						//							//TRDTIM_1[i].Hours;
						//							//TRDTIM_1[i].Minutes;
						//							//TRDVOL_1[i].ToString();
						//							decimal lp=TRDPRC_1[i].DecimalValue;
						//							price=(double)lp;
						//						}
					}
				}
			}
			//First get the klOptionInstance from the hastable
			klOptionInstance kloi=null;
			//Run through the hashtable putting in underlying price and symbol 
			IDictionaryEnumerator symIdx=m_symbolHash.GetEnumerator();
			for(int i=0;i<m_symbolHash.Count;i++)
			{
				symIdx.MoveNext();
				System.Type tpo=symIdx.Current.GetType();
				//klOptionInstance ko=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
				kloi=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
				kloi.m_underlying=symbol;
				kloi.m_price=price;
												
			}				
			
				

		}


		public  void LIVEDATA_OptionsCallback(object sender, DataEventArgs e)
		{
			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
			if (!(laq.m_bReadyForData))
			{
				//SetupForData(sender);
			}
			
			RawData rd=e.Data;
			IDataBlock block= rd.GetDataAsBlock();
			int iRows = 1;
			int iFields = 0;

			string symbol="";
			//1003,"DISP_NAME"
			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					if(fld.FieldInfo.ID==1003)//symbol
					{
						symbol= fld.StringValue;
					}
				}
			}

			//First get the klOptionInstance from the hastable
			klOptionInstance kloi=null;
			if(m_symbolHash.Contains(symbol))
			{

				IDictionaryEnumerator symIdx=m_symbolHash.GetEnumerator();
				for(int i=0;i<m_symbolHash.Count;i++)
				{
					symIdx.MoveNext();
					string lKey=(string)symIdx.Key;
					if (lKey==symbol)
					{
						System.Type tpo=symIdx.Current.GetType();
						//klOptionInstance ko=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
						kloi=(klSymbolCalculus.klOptionInstance)(symIdx.Value);
						
					}				
				}				
			}

			if(kloi==null)
				throw new  System.Exception("klVolatilitySurface::LIVEDATA_OptionsCallback ERROR retrieving symbol.");


			foreach(IRow row in block)
			{
				System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
				foreach(Field fld in row)
				{
					string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
					//					WhatFID.Add(1016,"TRD_DATE");
					//					WhatFID.Add(1018,"TRDTIM_1");
					//					WhatFID.Add(1178,"TRDVOL_1");
					//					WhatFID.Add(1006,"TRDPRC_1");
					//					WhatFID.Add(1022,"BID");
					//					WhatFID.Add(1025,"ASK");
					//					//WhatFID.Add(9020,"TICK_TYPE");
					//					WhatFID.Add(1067,"EXPIR_DATE");
					//					WhatFID.Add(1066 ,"STRIKE_PRC");
					//					WhatFID.Add(1064, "OPINT_1");

					//First some notes about type converters
					//DateTime dt=new DateTime(1990,5,6);
					//Console.WriteLine(TypeDescriptor.GetConverter(dt).ConvertTo(dt, typeof(string)));
					//string myStr="1991-10-10";
					//Console.WriteLine(TypeDescriptor.GetConverter(dt).ConvertFrom(myStr));


					//Fill in the kloi 
					if(fld.FieldInfo.ID==1067)//T
					{
						//DateTime d=DateTime.Parse(fld.StringValue);
						DateTime d=new DateTime();
						d=(DateTime)TypeDescriptor.GetConverter(d).ConvertFrom(fld.StringValue);
						kloi.m_expiry=d;
					}
					if(fld.FieldInfo.ID==1066)//K
					{
						double strike=0;
						strike=(double)TypeDescriptor.GetConverter(strike).ConvertFrom(fld.StringValue);
						kloi.m_strike=strike;					
		
					}
					if(fld.FieldInfo.ID==1064)//OI
					{
						int OI=0;
						OI=(int)TypeDescriptor.GetConverter(OI).ConvertFrom(fld.StringValue);
						
						kloi.m_openInterest=OI;

					}
					if(fld.FieldInfo.ID==1022)//BID
					{
						double BID=0;
						BID=(double)TypeDescriptor.GetConverter(BID).ConvertFrom(fld.StringValue);
						kloi.m_bid=BID;

					}
					if(fld.FieldInfo.ID==1025)//ASK
					{
						double ASK=0;
						ASK=(double)TypeDescriptor.GetConverter(ASK).ConvertFrom(fld.StringValue);
						kloi.m_bid=ASK;

					}
					if(fld.FieldInfo.ID==1006)//Price
					{
						Price[] TRDPRC_1=row["TRDPRC_1"].PriceVectorValue;
						int[] TRDVOL_1 = row["TRDVOL_1"] == null ? null : row["TRDVOL_1"].IntVectorValue;
						TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
						DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;			
						int elements = 0;
				
						if (row["TRDPRC_1"] != null)
							elements = row["TRDPRC_1"].PriceVectorValue.Length;
				
						int first_element=0;

						for (int i = first_element; i < elements; i++)
						{
							//TRD_DATE[i].ToString("yyyy/MM/dd"),
							//TRDTIM_1[i].Hours;
							//TRDTIM_1[i].Minutes;
							//TRDVOL_1[i].ToString();
							decimal lp=TRDPRC_1[i].DecimalValue;
							kloi.m_price=(double)lp;
						}
					}


					
					
				
									
				}
			}


			

	
		}


		public  void LIVEDATA_OptionsOnTerminate(object sender, EventArgs e)
		{
			//tal.Cleanup();
			//tal.Init();
		}

		public Hashtable makeSymbolHash(klTALApplication a,String fileName)
		{

			Hashtable OPTION_HASH=new Hashtable();
			m_symbolHash=OPTION_HASH;

			Hashtable WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1016,"TRD_DATE");
			WhatFID.Add(1018,"TRDTIM_1");
			WhatFID.Add(1178,"TRDVOL_1");
			WhatFID.Add(1006,"TRDPRC_1");
			WhatFID.Add(1022,"BID");
			WhatFID.Add(1025,"ASK");
								
			//WhatFID.Add(9020,"TICK_TYPE");
			WhatFID.Add(1067,"EXPIR_DATE");
			WhatFID.Add(1066 ,"STRIKE_PRC");
			WhatFID.Add(1064, "OPINT_1");
									
			Hashtable WhereFID=new Hashtable();
			TQLTypeValuePair tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			WhereFID.Add(1003,tvp);
								
			//This sets the LIVEQUOTE instrument type FID.
			tvp=new TQLTypeValuePair("STYP","'2'");
			WhereFID.Add(2000,tvp);		
											
			DateTime d1 = DateTime.Now;
			string date=d1.Month+"_"+d1.Day+"_"+d1.Year;//d1.ToShortDateString();
			string time= d1.TimeOfDay.Hours.ToString()+ d1.TimeOfDay.Minutes.ToString();//d1.ToShortTimeString();
			
			StreamReader sr=new StreamReader(fileName);
			
			string symbol;
			
			long howMany =0;

			bool eof=false;
			string line="";
			while(!eof)
			{
				line=sr.ReadLine();
				//Remove ' at beginning and end of line
				if(line!=null)
				{
					howMany++;
				}
				else
				{
					eof=true;
								
				}
			}
			String underlyingSymbol=null;
			sr.Close();//bbcrevisit thi
			sr=new StreamReader(fileName);
			//sr.BaseStream.Seek(0,0);

			for(int i=0;i<howMany-1;i++)
			{
				symbol=sr.ReadLine();

				klOptionInstance kloi=new klOptionInstance();
				
				kloi.m_symbol=symbol;
							
				OPTION_HASH.Add(symbol,kloi);
			}
			
			underlyingSymbol=sr.ReadLine();
			
			sr.Close();//bbcrevisit thi
			sr=new StreamReader(fileName);
			//remove last symbol since it is the option root 
			howMany-=1 ;
			for(int i=0;i<howMany;i++)
			{
				symbol=sr.ReadLine();
					
				//DateTime d1 = DateTime.Now;
				d1 = DateTime.Now;

				//Set the symbol element of the TVP 
				tvp=(TQLTypeValuePair)WhereFID[1003];
				tvp.m_TQLValue="'"+ symbol+ "'";
				string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);

				klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL);
				
				klAsyncQuery laqq = new klAsyncQuery(tsb);
				
				Connection livequoteConnection;

				livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
				AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_OptionsCallback);
				AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_OptionsOnTerminate);
			
				laqq.OnAdviseData += kladodh;
				laqq.OnRequestData += kladodh;
				laqq.OnTerminate += kladoth;

				//long lKey=GetItemKey(laqq.myTQL());

				//m_QueryHash.Add(lKey,laqq);
				
				string TQL=laqq.myTQL();
						
				livequoteConnection.AttachQuery(laqq);
	
				laqq.Request(TQL, "TAL4");
					
				Thread.Sleep(100);

				//Now ... is there in HashTable

			}

			//Now put underlyingSymbol in hash
			//And query for last sale price
			//Since it's a livequote query - caution is advised on runnning this at night
			//bbcrevisit
			
		{
			WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1006,"TRDPRC_1");
																	
			WhereFID=new Hashtable();
			tvp=new TQLTypeValuePair("DISP_NAME","");
			WhereFID.Add(1003,tvp);
			tvp=(TQLTypeValuePair)WhereFID[1003];
			tvp.m_TQLValue="'"+ underlyingSymbol+ "'";
			string lTQL=klTALApplication.TQLfromWhatWhereFIDHashes("LIVEQUOTE",WhereFID ,WhatFID);
			klTALStreamAttributes tsb = new klTALStreamAttributes("","TA_SRV","LIVEQUOTE",lTQL);
			klAsyncQuery laqq = new klAsyncQuery(tsb);
			Connection livequoteConnection;
			livequoteConnection = a.GetConnection("", "TA_SRV", "LIVEQUOTE");
				
			AsyncQuery.DataHandler    kladodh=new AsyncQuery.DataHandler(LIVEDATA_EquityCallback);
			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(LIVEDATA_OptionsOnTerminate);
			laqq.OnAdviseData += kladodh;
			laqq.OnRequestData += kladodh;
			laqq.OnTerminate += kladoth;

			string TQL=laqq.myTQL();
						
			livequoteConnection.AttachQuery(laqq);
	
			laqq.Request(TQL, "TAL4");
					

			Thread.Sleep(300);	
		}


			
			IDictionaryEnumerator hashIdx=OPTION_HASH.GetEnumerator();
			for(int i=0;i<OPTION_HASH.Count;i++)
			{
				hashIdx.MoveNext();
				klOptionInstance kloi=(klOptionInstance)hashIdx.Value;
				kloi.m_underlying=underlyingSymbol;

                
			}

			return OPTION_HASH;
		}

	}


	public class klLiveChartOptionsBarFileSource :klFileSource
	{
		public String m_baseFileName;

		public klLiveChartOptionsBarFileSource() :base("")
		{
		}
		
		
		public  void LIVECHART_PrimeDBTicCallback(object sender, DataEventArgs e)
		{

			//if object is always of klAysncQuery type- we use TQL string to format
			klAsyncQuery laq=(klAsyncQuery)sender;	

			//lock(laq)//Do a wait one - or threadblock here - 

			IDataBlock blk = e.Data.GetDataAsBlock();
			
			IRow row = blk[0];

			Hashtable whatFID=laq.myWhatFID();//
			Hashtable whereFID=laq.myWhereFID();//

			//From INTRADAY TABLE
			//			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			//			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			//			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			//			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			//			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			//			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			//			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			//			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");
			if(	whatFID.ContainsValue("NUM_MOVES"))
			{
				Price[] OPEN_PRC=row["OPEN_PRC"].PriceVectorValue;
				Price[] HIGH_1=row["HIGH_1"].PriceVectorValue;
				Price[] LOW_1=row["LOW_1"].PriceVectorValue;
				Price[] SETTLE=row["SETTLE"].PriceVectorValue;
				int[] NUM_MOVES = row["NUM_MOVES"] == null ? null : row["NUM_MOVES"].IntVectorValue;
				TimeSpan[] TRDTIM_1 = row["TRDTIM_1"] == null ? null : row["TRDTIM_1"].TimeVectorValue;
				DateTime[] TRD_DATE = row["TRD_DATE"] == null ? null : row["TRD_DATE"].DateVectorValue;
				int elements = 0;
				if (row["OPEN_PRC"] != null)
					elements = row["OPEN_PRC"].PriceVectorValue.Length;
				int first_element=0;

				StreamWriter sw=laq.myStreamWriter();//This callback writes to an already open file stream
				Monitor.Enter(sw);

				for (int i = first_element; i < elements; i++)
				{

					sw.WriteLine(String.Format("{0},{1}:{2},{3},{4},{5},{6},{7}",
						TRD_DATE[i].ToString("yyyy/MM/dd"),
						TRDTIM_1[i].Hours.ToString("D2"),
						TRDTIM_1[i].Minutes.ToString("D2"),
						OPEN_PRC[i].ToString(),
						HIGH_1[i].ToString(),
						LOW_1[i].ToString(),
						SETTLE[i].ToString(),
						NUM_MOVES[i].ToString()));
						
						
				}				
				sw.Flush();
				Monitor.Exit(sw);
				//laq.m_ARE.Set();
			}
			


				
						

		}

			 	
		public  void LIVECHART_PrimeDBTicOnTerminate(object sender, EventArgs e)
		{

		}


		public void LIVECHARTSynchronousFileSinkQuery(klTALApplication a, ArrayList symbols,String rootSymbol )
		{
			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");

	
		
			Hashtable INTRADAY_WhatFID_Hash=new Hashtable();
			INTRADAY_WhatFID_Hash.Add(1003,"DISP_NAME");
			INTRADAY_WhatFID_Hash.Add(1016,"TRD_DATE");
			INTRADAY_WhatFID_Hash.Add(1018,"TRDTIM_1");
			INTRADAY_WhatFID_Hash.Add(1019,"OPEN_PRC");
			INTRADAY_WhatFID_Hash.Add(1012,"HIGH_1");
			INTRADAY_WhatFID_Hash.Add(1013,"LOW_1");
			INTRADAY_WhatFID_Hash.Add(1070,"SETTLE");
			INTRADAY_WhatFID_Hash.Add(1077,"NUM_MOVES");

			//For each symbol we make queries for 15days 5min

			int symbolsSize=symbols.Count;
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
				Hashtable TICKS_WhereFID_Hash=new Hashtable();
				
				DateTime d1 = DateTime.Now;
				string date=d1.ToShortDateString();

				TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
				TICKS_WhereFID_Hash.Add(9008,tvp);//For tic data only the end date is used to get the data
		 
				tvp=new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'");
				TICKS_WhereFID_Hash.Add(1003,tvp);


				Hashtable INTRADAY_WhereFID_Hash=new Hashtable();
				INTRADAY_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'"+(string)symbols[symbolCount]+"'"));
				INTRADAY_WhereFID_Hash.Add(9008,new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'"));//For tic data only the end date is used to get the data
				//INTRADAY_WhereFID_Hash.Add(9007,"CHART_STARTDATE");
				//INTRADAY_WhereFID_Hash.Add(9009,"CHART_STARTTIME");
				//INTRADAY_WhereFID_Hash.Add(9010,"CHART_STOPTIME");
				INTRADAY_WhereFID_Hash.Add(9004,new TQLTypeValuePair("DAYS_BACK","15"));
				INTRADAY_WhereFID_Hash.Add(9005,new TQLTypeValuePair("CHART_BAR_INTERVAL","5"));//5min bars

	
				string intradayTQL=klTALApplication.TQLfromWhatWhereFIDHashes("INTRADAY", INTRADAY_WhereFID_Hash ,INTRADAY_WhatFID_Hash);

			
				klAsyncQuery intradayQuery=new klAsyncQuery(INTRADAY_WhatFID_Hash,INTRADAY_WhereFID_Hash);


				String fileName =m_baseFileName+rootSymbol + "_" + (string)symbols[symbolCount];
				String fileDate="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;;
				fileName+= "_INTRADAY_"+fileDate;
				fileName+=".txt";
				StreamWriter isw =new StreamWriter(fileName);
		
				intradayQuery.setMyStreamWriter(isw);
		
				AsyncQuery.DataHandler kladodhtic=new AsyncQuery.DataHandler(LIVECHART_PrimeDBTicCallback);
				AsyncQuery.TerminateHandler kladothtic=new AsyncQuery.TerminateHandler(LIVECHART_PrimeDBTicOnTerminate);
				
				intradayQuery.OnAdviseData += kladodhtic;
				intradayQuery.OnRequestData += kladodhtic;
				intradayQuery.OnTerminate += kladothtic;

				livechartConnection.AttachQuery(intradayQuery);
			
				//This is a hateful way to do it. bbcrevisit - this needs to work - 
				//AutoResetEvent hData =new AutoResetEvent(true);
				//dailyQuery.m_ARE=hData;
				//				//hData.Reset();
				
				intradayQuery.Request(intradayTQL+" ","TAL4");
				Thread.Sleep(300);
				
				if(isw.BaseStream.Length==0)
				{
					isw.Flush();
					isw.Close();
					File.Delete(fileName);
				}
				else
				{
					isw.Flush();
					isw.Close();
				}
			
				//Wait for all queries to end before returning

			}


		}


	}


	public class klLiveQuoteEquityTickFileSource : klFileSource
	{		
			
		//private klLiveDataDelegate m_delegate;
				
		//one per - make thread safe as the threads will use th TQL string to get the stream object from the table
		static private Hashtable m_QueryHash;
				
		//private Dequeue m_buf;

		//one buffer Dequeue per stream 
		//Deque is sometimes written dequeue, but this is generally not preferred because 
		//dequeue is also a verb meaning "to remove from a queue". Nevertheless, several libraries 
		//and some writers, such as Aho, Hopcroft, and Ullman use dequeue.
		static private Hashtable m_DequeHash;
	
		public void Add(long lKey,Dequeue AQDataContainer)
		{
			m_DequeHash.Add(lKey,AQDataContainer);
		}


		//This guy needs to be a hash table as well.
		//One thread per query.
		private Thread t;

		static private Hashtable m_ThreadHash;

		//private AutoResetEvent hData;
		//add accessors
		static private Hashtable m_AutoResetEventHash;

		public void Add(long lKey,AutoResetEvent hData)
		{
			m_AutoResetEventHash.Add(lKey,hData);
		}

		private bool m_bReadyForData;
		private bool m_bRunning;
			
		private void m_query_OnAdviseData(object sender, DataEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;	
			String TQL=laq.myTQL();
					
			long lKey=GetItemKey(TQL);
					
				   
			Dequeue AQ_DataContainer =(Dequeue) m_DequeHash[lKey];
				
			lock(AQ_DataContainer)
			{
				AutoResetEvent hData= (AutoResetEvent)m_AutoResetEventHash[lKey];
				lock(hData)
				{


					if (!(laq.m_bReadyForData))
					{
						SetupForData(sender);
					}
					lock(AQ_DataContainer.SyncRoot)
					{
						StreamWriter lsw=laq.myStreamWriter();
						RawData rd=e.Data;
						IDataBlock block= rd.GetDataAsBlock();
						int iRows = 1;
						int iFields = 0;
						foreach(IRow row in block)
						{
							System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
							foreach(Field fld in row)
							{
								string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
								lsw.WriteLine(moo);
								lsw.Flush();
									
							}
						}


						//AQ_DataContainer.EnqueueTail(e.Data);


					}
				
					hData.Set();
				}
			}
			
		}
				
		//((klLiveData.klLiveDataMain.klAsync***Query)(sender))
		private void m_query_OnOtherAck(object sender, AckEventArgs e)
		{

			klAsyncQuery laq=(klAsyncQuery)sender;

			if (1 == e.Status)
			{
				if (!(laq.m_bReadyForData))
				{
					//We have to set up the stream object and get it into the thread for DataProc?
					//This is bad.  It seems like there ultimately needs to be a binding between TQL C# function
					//and data processing streams.  
					//There needs to be a way to fill a hash table of thread mains written for each TQL query. 
					//
					//sender is always the query type associated with this klLiveData***Processor
					SetupForData(sender);
				}
			}
		}
			

			
		private void DataProc()
		{
			//				try
			//				{
			//
			//
			//
			//					Object o;
			//
			//					while (m_bRunning)
			//					{
			//						//Alelu - get the NAME of this thread - it's the TQL string
			//						//Get the named TLS object - of type klDataStreamWriter (StreamWriter for V0.1)
			//						string TQL= System.Threading.Thread.CurrentThread.Name;
			//						////
			//						LocalDataStoreSlot ldts=System.Threading.Thread.GetNamedDataSlot(TQL);
			//						//Now there's a 1:1 relation between TQL and IOCP threads used.
			//					
			//						//bbcrevisit this is not working for now - 
			//						//as a workaround - poosibly a better solution
			//						// we are using the static hash table member to get the stream
			//						StreamWriter lsw=(StreamWriter)System.Threading.Thread.GetData(ldts);
			//
			//						long lKey=GetItemKey(TQL);
			//
			//						AutoResetEvent hData= (AutoResetEvent)m_AutoResetEventHash[lKey];
			//						hData.WaitOne();
			//						
			//
			//						if (m_QueryHash.Contains(lKey))
			//						{
			//							klAsyncQuery las = (klAsyncQuery) m_QueryHash[lKey];
			//							lsw=las.myStreamWriter();
			//						}		
			//						
			//						do
			//						{
			//							Dequeue m_buf=(Dequeue)m_DequeHash[lKey];
			//							
			//							//debug
			//							System.Console.WriteLine(TQL);
			//							System.Diagnostics.Debug.WriteLine(TQL);
			//							
			//							
			//							lock(m_buf.SyncRoot)
			//							{
			//									//o = m_buf.DequeueHead();
			//								o=null;
			//							}
			//							//RawData rd=(RawData)o;
			//							//IDataBlock block= rd.GetDataAsBlock();
			//
			//							if (!(null == o))
			//							{
			//								RawData r=(RawData) o;
			//								IDataBlock block= r.GetDataAsBlock();
			//								int iRows = 1;
			//								int iFields = 0;
			//								foreach(IRow row in block)
			//								{
			//									System.Diagnostics.Debug.WriteLine( String.Format("Row {0}", iRows++) );
			//									foreach(Field fld in row)
			//									{
			//										string moo=String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue);
			//										lsw.WriteLine(moo);
			//										lsw.Flush();
			//										System.Console.WriteLine(String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue));
			//										iFields = 1;
			//										System.Diagnostics.Debug.WriteLine( String.Format("Field {0}: {1}: {2}", iFields++, fld.FieldInfo.ID, fld.StringValue) );
			//									}
			//								}
			//
			//
			//							}
			//						}
			//						while (!(null == o));
			//				
			//					}
			//				}
			//				catch
			//				{
			//					m_bReadyForData = false;
			//				}

		}
		//sender is always the query type associated with this klLiveData***Processor
		private void SetupForData(object sender)
		{
			string TQL;//

			klAsyncQuery laq=(klAsyncQuery)sender;
					
			TQL=laq.myTQL();
					
			StreamWriter lsw=laq.myStreamWriter();
					
			ThreadStart ts = new ThreadStart(DataProc );
			t = new Thread(ts);

			//Each thread is associated with a TQL data stream query.
			t.Name=TQL;

			//Have to put info or StreamWriter in thread local storage or address Hash of function pointers with compiled impls design issue discussed above

			t.Start();  //DataProc gets called when there's a message after this gets fired.

			laq.m_bReadyForData = true;//This query is marked ready for data.
		}
		//Dig this - do not duplicate TQL stream objects in the klLiveQuoteEquityTickFileSource Hash!  
		private long GetItemKey(string szItem)
		{
			return(szItem.GetHashCode());
				
		}
		public klAsyncQuery findQuery( string szItem)
		{
			long lKey=GetItemKey(szItem);
			 
			if (m_QueryHash.Contains(lKey))
			{
				klAsyncQuery las = (klAsyncQuery) m_QueryHash[lKey];
				return las;
				//lsw=las.myStreamWriter();
			}
			else 
				return null;
		}
			
		//Verify dated comments
		public void addQuery(klAsyncQuery AQ,string szItem)
		{
			//The query was initialized and added to the connection by the application class klLiveDataApp
			//Now we add it to the hash table of queries associated with this data processor.
			//Again the benefit of having the app manage the connection-query relationship is the data stream
			//is now decoupled from the device.  We can re-direct data streams on the fly by the TQL - AsyncQuery relationship.
			AQ.Advise(szItem, "TAL4");
				

			//TLS Solution - each klAsync***Query Gets it's own stream - we create the named TLS entry based on the TQL query - hence the Dig This comment - not to add the same Query twice to a klLiveData***Processor instance.
			//Threads use a local store memory mechanism to store thread-specific data. 
			//The common language runtime allocates a multi-slot data store array to each process when it is created. 
			//The thread can allocate a data slot in the data store, store and retrieve a data value in the slot, 
			//and free the slot for reuse after the thread expires. Data slots are unique per thread. No other thread 
			//(not even a child thread) can get that data.
			//AllocateDataSlot  Allocates an unnamed data slot on all the threads. 
			//AllocateNamedDataSlot  Allocates a named data slot on all threads. 
			//LocalDataStoreSlot
			LocalDataStoreSlot ldts=System.Threading.Thread.AllocateNamedDataSlot(szItem);
			//Now there's a 1:1 relation between TQL and IOCP threads used.
			StreamWriter lsw=AQ.myStreamWriter();
			System.Threading.Thread.SetData(ldts,lsw );

			//The thread is named with the TQL string and we use the thread name to retrieve the TLS object.
			//This could be a little cleaner.
				
			//Sets the handles to Query callbacks.  Same callback used for all queries used by an instance of klLiveData***Processor. 
			AQ.OnAdviseData +=new AsyncQuery.DataHandler(m_query_OnAdviseData);

			AQ.OnOtherAck +=new AsyncQuery.AckHandler(m_query_OnOtherAck);

			long lKey=GetItemKey(AQ.myTQL());

			m_QueryHash.Add(lKey,AQ);

			m_bRunning = true;//This means there is at least one query attached a connection in this data processor.

		}
            
		
		public klLiveQuoteEquityTickFileSource() : base ("")
		{
			m_bReadyForData = false;
			m_QueryHash= new Hashtable();
			m_DequeHash= new Hashtable();
			m_AutoResetEventHash = new Hashtable();
				
		}
	}
		
		
	public class klLiveData
	{
		//This method builds TQL string from the What Where HashTables 
		
		//XML TQL workflow is parsed and factored out as a series of queries directed to klDataStream derivatives.

		//The delegate is used by the LiveDataProcessor to handle messages from the queries attached to it.
		public delegate void klLiveDataDelegate(RawData r, StreamWriter fileStream);
	
                 
	
		public static void livedataMain()

		{
			klTALApplication a = new klTALApplication();

		

		{
			//"30185","MR_BVPERSHAREINTANG"
			//String TQL="FUNDAMENTALS;30159;1003='AAPL'  ";
			klLiveQuoteEquityFundamentalsADOSource efas=new klLiveQuoteEquityFundamentalsADOSource();
			string factTableName="RUT_FACT" ;

			Hashtable FUNDAMENTALS_WhatFID_Hash=new Hashtable();
			for(int i=0;i<TALFIDS.nFundamentalIDs;i++)//for(int i=0;i<3;i++)
			{
				string id=TALFIDS.fundamentalFIDs[i,0];
				int idi =int.Parse(id);
				string name=TALFIDS.fundamentalFIDs[i,1];
				FUNDAMENTALS_WhatFID_Hash.Add(idi,name);
			}
			
			//Hashtable FUNDAMENTALS_WhereFID_Hash=new Hashtable();
        
			//FUNDAMENTALS_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'QQQQ'"));
		
			//string lTQL =klTALApplication.TQLfromWhatWhereFIDHashes("FUNDAMENTAL",FUNDAMENTALS_WhereFID_Hash,FUNDAMENTALS_WhatFID_Hash);
		
			//	efas.LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(a,FUNDAMENTALS_WhatFID_Hash,FUNDAMENTALS_WhereFID_Hash,factTableName);
		

			//ArrayList symbols=klSymbolCalculus.SymbolSet.RUTSymbols();
			
			//efas.LIVEQUOTE_FUNDAMENTAL_MakeFactTable(factTableName,FUNDAMENTALS_WhatFID_Hash);
		
			//efas.LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(a,FUNDAMENTALS_WhatFID_Hash,symbols,factTableName);


		}

		{

			//klLiveChartEquityADOSource edbs=new klLiveChartEquityADOSource();
			//edbs.LIVECHARTSynchronousADOSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/26/2007',1003='QQQQ'");
			

			//Thread.Sleep(3000);


		}


		


		{  ///////////////////klOptionsChainFileSource Example
		
			//			klOptionsChainFileSource	ocfs=new klOptionsChainFileSource();
			//
			//			int howMany;//Get the options symbols for an underlying.
			//			StreamWriter qqqq_sync_lswmoo=new StreamWriter("QQQQ_QUOTECHAIN_.txt");
			//			howMany=ocfs.QUOTECHAINSynchronousFileSinkQuery(a,"QUOTECHAIN;1003;6050='AU' ",qqqq_sync_lswmoo);
			//			Thread.Sleep(4000);
			//			qqqq_sync_lswmoo.Flush();
			//			qqqq_sync_lswmoo.Close();
			//			//Connect an AsyncQuery to LIVEQUOTE for the option symbols
			//			StreamReader sr= new StreamReader("QQQQ_QUOTECHAIN_.txt");
			//
			//			Hashtable WhatFID=new Hashtable();
			//			WhatFID.Add(1003,"DISP_NAME");
			//			WhatFID.Add(1016,"TRD_DATE");
			//			WhatFID.Add(1018,"TRDTIM_1");
			//			WhatFID.Add(1178,"TRDVOL_1");
			//			WhatFID.Add(1006,"TRDPRC_1");
			//			WhatFID.Add(1022,"BID");
			//			WhatFID.Add(1025,"ASK");
			//
			//			//WhatFID.Add(9020,"TICK_TYPE");
			//			WhatFID.Add(1067,"EXPIR_DATE");
			//			WhatFID.Add(1066 ,"STRIKE_PRC");
			//			WhatFID.Add(1064, "OPINT_1");
			//	
			//			Hashtable WhereFID=new Hashtable();
			//			TQLTypeValuePair tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			//			WhereFID.Add(1003,tvp);
			//
			//			//This sets the LIVEQUOTE instrument type FID.
			//			tvp=new TQLTypeValuePair("STYP","'2'");
			//			WhereFID.Add(2000,tvp);
			//			
			//			
			//			DateTime d1 = DateTime.Now;
			//			string date="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;//d1.ToShortDateString();
			//			string time= d1.TimeOfDay.Hours.ToString()+ d1.TimeOfDay.Minutes.ToString();//d1.ToShortTimeString();
			//			string filename ="QQQQ_"+date+"_"+time+".txt";
			//			StreamWriter qqqq_optionschain=new StreamWriter(filename);
			//
			//			ocfs.LIVEQUOTEOptionsSyncFileSinkQuery(a,sr,WhatFID,WhereFID,howMany,qqqq_optionschain);
			//			Thread.Sleep(3000);
			//			qqqq_optionschain.Flush();
			//			qqqq_optionschain.Close();

			
		}






		
		
		{

			//			StreamWriter qqqq_sync_lswmoo=new StreamWriter("QQQQ_synchronous_tic_request.txt");
			//			klLiveChartEquityFileSource efs=new klLiveChartEquityFileSource();
			//			efs.LIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/15/2007',1003='QQQQ'",qqqq_sync_lswmoo);
			//			qqqq_sync_lswmoo.Flush();
			//			qqqq_sync_lswmoo.Close();


		}




		

			//Array List  
			ArrayList symbols=klSymbolCalculus.SymbolSet.SandPSymbols();//RUTSymbols()    ;//new ArrayList();
			//		NASDAQ MOAST ACTIVE ETF's 031807-	
			//			symbols.Add("SPY");
			//			symbols.Add("QQQQ");
			//			symbols.Add("IWM");
			//			symbols.Add("XLE");
			//			symbols.Add("XLF");
			//			symbols.Add("EWJ");
			//			symbols.Add("QID");
			//			symbols.Add("OIH");
			//			symbols.Add("DIA");
			//			symbols.Add("SMH");
			//			symbols.Add("EEM");
			//			symbols.Add("XLB");
			//			symbols.Add("GLD");
			//			symbols.Add("EWZ");
			//			symbols.Add("IYR");
			//			symbols.Add("EFA");
			//			symbols.Add("RTH");
			//			symbols.Add("MDY");
			//			symbols.Add("XLK");
			//			symbols.Add("XHB");
			//			symbols.Add("EWT");
			//			symbols.Add("EWW");
			//			symbols.Add("EWM");
			//			symbols.Add("XLU");
			//			symbols.Add("USO");
			//			symbols.Add("EWS");
			//			symbols.Add("XLP");
			//			symbols.Add("SDS");
			//			symbols.Add("IWO");
			//			symbols.Add("XLI");
			//			symbols.Add("FXI");
			//			symbols.Add("EWH");
			//			symbols.Add("GDX");
			//			symbols.Add("RKH");
			//			symbols.Add("IYT");
			//			symbols.Add("QLD");
			//			symbols.Add("IWD");
			//			symbols.Add("IBB");
			//			symbols.Add("IVV");
			//			symbols.Add("IJR");
			//			symbols.Add("XLY");
			//			symbols.Add("EWG");
			//			symbols.Add("XLV");
			//			symbols.Add("IWN");
			//			symbols.Add("SHY");
			//			symbols.Add("IWF");
			//			symbols.Add("KBE");
			//			symbols.Add("IAI");
			//			symbols.Add("IWC");
			//			symbols.Add("TLT");
			//			symbols.Add("EWY");
			//			symbols.Add("EWC");
			//			symbols.Add("XRT");


		{
			//2000daily, 15day 5min, 5day tic.
			//klLiveChartEquityFileSource efs=new klLiveChartEquityFileSource();
			//efs.LIVECHARTSynchronousFileSinkQuery( a,  symbols);



		}
			
			

		{		
			//			Hashtable WhatFID=new Hashtable();
			//			WhatFID.Add(1003,"DISP_NAME");
			//			WhatFID.Add(1016,"TRD_DATE");
			//			WhatFID.Add(1018,"TRDTIM_1");
			//			WhatFID.Add(1178,"TRDVOL_1");
			//			WhatFID.Add(1006,"TRDPRC_1");
			//			WhatFID.Add(9020,"TICK_TYPE");
			//
			//			Hashtable WhereFID=new Hashtable();
			//			DateTime d1 = DateTime.Now;
			//			string date=d1.ToShortDateString();
			//			TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
			//			WhereFID.Add(9008,tvp);//For tic data only the end date is used to get the data
			//			tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			//			WhereFID.Add(1003,tvp);
			//			a.LIVEQUOTEFileSourceQuery(symbols, WhatFID,	 WhereFID);

			
		}



		{
			symbols=klSymbolCalculus.SymbolSet.SandPSymbols();//RUTSymbols()    ;//new ArrayList();
			//5000daily, 15day 5min, 5day tic.
			klLiveChartEquityADOSource edbs=new klLiveChartEquityADOSource();
			//Set the data source - 
			edbs.setDBInfo("SPY","SPY","sa","rexcat1");
			edbs.LIVECHARTSynchronousADOSinkQuery(a,symbols);
			

			//Thread.Sleep(3000);
		}





		
		
		
		{
			//Use AsyncQuery.Request for Synchronous requests. 
			//StreamWriter qqqq_sync_lsw=new StreamWriter("QQQQ_synchronous_tic_request.txt");
			//Each Stream attribute we create is an AsyncQuery that gets processed by the delegate.  
			//Use a different DataProceedure derivative for directing flow to file, SQLSRV via ADO,  VTK, or the kl Libraries.
			//This design coupled with TQL creates a scripting language for tickerplant data flow.
			//klLIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/15/2007',1003='QQQQ'",qqqq_sync_lsw);
			//klLIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/14/2007',1003='QQQQ'",qqqq_sync_lsw);
			//klLIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/13/2007',1003='QQQQ'",qqqq_sync_lsw);
			//klLIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/12/2007',1003='QQQQ'",qqqq_sync_lsw);
			//klLIVECHARTSynchronousFileSinkQuery(a,"TICKS;1003,1016,1006,1018,1178;9008='03/09/2007',1003='QQQQ'",qqqq_sync_lsw);
		}
			
		
		{//Connect by hand.
			//			StreamWriter qqqq_sync_lsw2=new StreamWriter("QQQQ_synchronous_tic_request2.txt");		
			//			klTALStreamAttributes tsb_qqqq_sync = new klTALStreamAttributes("","TA_SRV","LIVECHART",lTQL,qqqq_sync_lsw2);	
			//			//Make a query and call request
			//			klAsyncQuery laqq=new klAsyncQuery(tsb_qqqq_sync);
			//			//Set the items we would like the query to persist.
			//			//Some kl queries that filter realtime will persis a subset of the data so
			//		    //there is a container of the TQL what FIDs and where FID's this query will be persisting
			//			Hashtable sp=new Hashtable();
			//			sp.Add(1006,"TRDPRC_1");
			//			sp.Add(1016,"TRD_DATE");
			//			sp.Add(1178,"TRDVOL_1");
			//			sp.Add(1018, "TRDTIM_1");
			//			laqq.setTQLPersist(sp);
			//			AsyncQuery.DataHandler kladodh=new AsyncQuery.DataHandler(klADO_LIVEDATA_Callback);
			//			AsyncQuery.TerminateHandler kladoth=new AsyncQuery.TerminateHandler(klADO_LIVEDATA_OnTerminate);
			//		    //If we did not connect through Data Processor - we have to manually make connection
			//			Connection livechartConnection = a.GetConnection("", "TA_SRV", "LIVECHART");
			//			laqq.OnAdviseData += kladodh;
			//			laqq.OnRequestData += kladodh;
			//			laqq.OnTerminate += kladoth;
			//			livechartConnection.AttachQuery(laqq);
			//			laqq.Request(lTQL, "TAL4");//Callback gets invoked
			//			Thread.Sleep(3000);
		}

			while(a.IsRunning())
			{

				Thread.Sleep(1000);
			}
		}

	}//End klLiveData



	public class klTownsendDriver
	{
		private klTALApplication a ;
		public klTownsendDriver()
		{

			a = new klTALApplication();
		}
		private void IWM_PRIME_Click()
		{
			klTALApplication a = new klTALApplication();
			ArrayList symbols=klSymbolCalculus.SymbolSet.RUTSymbols();
			//5000daily, 15day 5min, 5day tic.
			klLiveChartEquityADOSource edbs=new klLiveChartEquityADOSource();
			//Set the data source - 
			edbs.setDBInfo("IWM","IWM","sa","rexcat1");
			edbs.LIVECHARTSynchronousADOSinkQuery(a,symbols);
			

		
		}

		private void SPY_PRIME_Click()
		{
			//klTALApplication a = new klTALApplication();
			ArrayList symbols=klSymbolCalculus.SymbolSet.SandPSymbols();
			//5000daily, 15day 5min, 5day tic.
			klLiveChartEquityADOSource edbs=new klLiveChartEquityADOSource();
			//Set the data source - 
			edbs.setDBInfo("i","i","sa","rexcat1");
			edbs.LIVECHARTSynchronousADOSinkQuery(a,symbols);
			

		}

		private void SPY_LIVEDATA_Click()
		{
			
			ArrayList symbols=klSymbolCalculus.SymbolSet.SandPSymbols();
			Hashtable WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1016,"TRD_DATE");
			WhatFID.Add(1018,"TRDTIM_1");
			WhatFID.Add(1178,"TRDVOL_1");
			WhatFID.Add(1006,"TRDPRC_1");
			WhatFID.Add(9020,"TICK_TYPE");

			Hashtable WhereFID=new Hashtable();
			DateTime d1 = DateTime.Now;
			string date=d1.ToShortDateString();
			TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
			WhereFID.Add(9008,tvp);//For tic data only the end date is used to get the data
			tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			WhereFID.Add(1003,tvp);
			a.LIVEQUOTEFileSourceQuery(symbols, WhatFID,	 WhereFID);
		}

		private void IWM_LIVEDATA_Click()
		{
			
			ArrayList symbols=klSymbolCalculus.SymbolSet.RUTSymbols();
			Hashtable WhatFID=new Hashtable();
			WhatFID.Add(1003,"DISP_NAME");
			WhatFID.Add(1016,"TRD_DATE");
			WhatFID.Add(1018,"TRDTIM_1");
			WhatFID.Add(1178,"TRDVOL_1");
			WhatFID.Add(1006,"TRDPRC_1");
			WhatFID.Add(9020,"TICK_TYPE");

			Hashtable WhereFID=new Hashtable();
			DateTime d1 = DateTime.Now;
			string date=d1.ToShortDateString();
			TQLTypeValuePair tvp=new TQLTypeValuePair("CHART_STOPDATE","'"+date+"'");
			WhereFID.Add(9008,tvp);//For tic data only the end date is used to get the data
			tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			WhereFID.Add(1003,tvp);
			a.LIVEQUOTEFileSourceQuery(symbols, WhatFID,	 WhereFID);
		
		}

		private void WeeklyTicToNAS_Click()
		{
			ArrayList IWMsymbols=klSymbolCalculus.SymbolSet.RUTSymbols();
			ArrayList SPYsymbols=klSymbolCalculus.SymbolSet.SandPSymbols();
			klLiveChartTicToNas edbs=new klLiveChartTicToNas();

			edbs.LIVECHARTSynchronousFileSinkQuery(a,SPYsymbols,"c:/klTicData2007/10_2007");

			edbs.LIVECHARTSynchronousFileSinkQuery(a,IWMsymbols,"c:/klTicData2007/10_2007");


			
			
		
		}

		private void ProcessNAS_Click()
		{
			
			// System.Windows.Forms.DateTimePicker dateTimePicker1;
			// System.Windows.Forms.MonthCalendar monthCalendar1;
			// System.Windows.Forms.DateTimePicker dateTimePicker2;
			// System.Windows.Forms.MonthCalendar monthCalendar2;
			ArrayList IWMsymbols=klSymbolCalculus.SymbolSet.RUTSymbols();
			ArrayList SPYsymbols=klSymbolCalculus.SymbolSet.SandPSymbols();
			int symbolsSize=SPYsymbols.Count;
			
			//long[] timeTic=null;
			//long[] priceTic=null;
			//long[] volTic = null;


			long[] timeTic=new long[16000000];
			double[] priceTic=new double[16000000];
			long[] volTic = new long[16000000];
			long ticsSpaceLeft=16000000;
			long TIC_MAX =16000000;
			long ticsRecorded=0;
			
			for(int symbolCount=0;symbolCount<symbolsSize;symbolCount++)
			{
				DateTime d2=DateTime.Now;

				//////bbcrevisit fix 
				DateTime beginDate=d2;//= monthCalendar1.SelectionStart;
				DateTime endDate=d2;//= monthCalendar2.SelectionStart;

				System.TimeSpan ts=endDate-beginDate;
				//Find the number of days in the range

				DateTime d=beginDate;
				double ndays=ts.TotalDays;
				//Call a function to see if the next date is a trading day.
				for (int i=0;i<ndays;i++)
				{
					
					System.DateTime di=d;
					System.TimeSpan tsi=new System.TimeSpan(i,0,0,0);	
					di=di.Add(tsi);
					string baseFileName="//wx-47/klTicData2007/";
					string month=di.Month+"_2007";
					baseFileName+=month;
					string fileName = baseFileName;					
					fileName +="/"+ (string)SPYsymbols[symbolCount];
				
					
					string fileDate="_"+di.Month+"_"+di.Day+"_"+di.Year;;
					fileName+="_TICKS_"+fileDate;
					fileName+=".txt";
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

						StreamReader lsw =new StreamReader(fileName);
						bool eof=false;
						while(!eof)
						{
							string line=lsw.ReadLine();
							long ticb=beginDate.Ticks;
							string delimStr = ",";
							char [] delimiter = delimStr.ToCharArray();
							string [] split = null;
							string[] vals= line.Split(delimiter);

							//Get the dt for this trade
							//long ticc=;
							//DateTime ticn=
							string dto= vals[0]+" "+vals[1];
							DateTime pdtt=DateTime.Parse(dto);
							long pdtt_t=pdtt.Ticks;

							if(line==null)
							{
								eof=true;
								
							}
							ticsSpaceLeft--;
							if(ticsSpaceLeft<=0)
								throw new System.Exception("klProcessNAS: out of space for tics");
							timeTic.SetValue(pdtt_t,ticsRecorded);
							volTic.SetValue(long.Parse(vals[3]),ticsRecorded);
							double dd=double.Parse(vals[2]);
							priceTic.SetValue(dd,ticsRecorded);
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
				string baseFileName="c:/klTSDB/SPY/";
				string fileName = baseFileName;					
				fileName +="/"+ (string)SPYsymbols[symbolCount];
				fileName+="_"+beginDate.Month+"_"+beginDate.Day+"_"+beginDate.Year;
				fileName+="_"+endDate.Month+"_"+endDate.Day+"_"+endDate.Year;
				fileName+=".txt";
				StreamWriter sw= new StreamWriter(fileName);
				for(int l=0;l<ticsRecorded;l++)
				{
					string line=timeTic[l]+", "+	volTic[l] + ", " + priceTic[l];
					sw.WriteLine(line);
				}
				sw.Close();
				ticsRecorded=0;
				ticsSpaceLeft=TIC_MAX;

				


			}


			}

		}

		private void SPY_FACT_Click()
		{
				
		
			//"30185","MR_BVPERSHAREINTANG"
			//String TQL="FUNDAMENTALS;30159;1003='AAPL'  ";
			klLiveQuoteEquityFundamentalsADOSource efas=new klLiveQuoteEquityFundamentalsADOSource();
			DateTime d1=DateTime.Now;
			
			string factTableName="SPY_FACT"+"_"+d1.Month+"_"+d1.Day+"_"+d1.Year;

			Hashtable FUNDAMENTALS_WhatFID_Hash=new Hashtable();
			for(int i=0;i<TALFIDS.nFundamentalIDs;i++)//for(int i=0;i<3;i++)
			{
				string id=TALFIDS.fundamentalFIDs[i,0];
				int idi =int.Parse(id);
				string name=TALFIDS.fundamentalFIDs[i,1];
				FUNDAMENTALS_WhatFID_Hash.Add(idi,name);
			}
			
			Hashtable FUNDAMENTALS_WhereFID_Hash=new Hashtable();
        
			FUNDAMENTALS_WhereFID_Hash.Add(1003,new TQLTypeValuePair("DISP_NAME","'QQQQ'"));
		
			string lTQL =klTALApplication.TQLfromWhatWhereFIDHashes("FUNDAMENTAL",FUNDAMENTALS_WhereFID_Hash,FUNDAMENTALS_WhatFID_Hash);
		
			efas.LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(a,FUNDAMENTALS_WhatFID_Hash,FUNDAMENTALS_WhereFID_Hash,factTableName);
		
			ArrayList symbols=klSymbolCalculus.SymbolSet.SandPSymbols();

			efas.m_sqlConnection=new SqlConnection("workstation id=i;packet size=8192;user id=sa;data source=i;persist security info=False;initial catalog=kl");
			
			new SqlConnection("Data Source=localhost;Integrated Security=SSPI;Initial Catalog=kl");
			//new SqlConnection("workstation id=DUALCOREP4;packet size=8192;user id=sa;data source=g;persist security info=False;initial catalog=kl");
			
			efas.LIVEQUOTE_FUNDAMENTAL_MakeFactTable(factTableName,FUNDAMENTALS_WhatFID_Hash);
		
			efas.LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(a,FUNDAMENTALS_WhatFID_Hash,symbols,factTableName);
			

			//symbols=klSymbolCalculus.SymbolSet.SandPSymbols();

			//factTableName="SPY_FACT"+"_"+d1.Month+"_"+d1.Day+"_"+d1.Year;

			//efas.LIVEQUOTE_FUNDAMENTAL_MakeFactTable(factTableName,FUNDAMENTALS_WhatFID_Hash);

			//efas.LIVEQUOTE_FUNDAMENTAL_SynchronousADOSinkQuery(a,FUNDAMENTALS_WhatFID_Hash,symbols,factTableName);
			

		
		}

		private void SPY_OPTION()
		{
			
			/////////////////klOptionsChainFileSource Example
		
			klOptionsChainFileSource	ocfs=new klOptionsChainFileSource();

			ArrayList symbols=klSymbolCalculus.SymbolSet.SandPSymbols();

			symbols.Insert(0,"A");//First do the basket //bbcrevisit set back to root SPY

			int numSymbols=symbols.Count;
			
			int symbolCount=0;

			for(symbolCount=0;symbolCount<numSymbols;symbolCount++)
			{
				String symbol = (string)symbols[symbolCount];

				string baseFileName="c:/klTSDB/SPY/OPTIONS/";
			
				string fileName = baseFileName+symbol+"_QUOTECHAIN";
			
				DateTime d1 = DateTime.Now;
				string date="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;//d1.ToShortDateString();
				string time= d1.TimeOfDay.Hours.ToString()+ d1.TimeOfDay.Minutes.ToString();//d1.ToShortTimeString();
			
				fileName+=date+"_"+".txt";
								
				//fileName +="/"+ (string)SPYsymbols[symbolCount];
				//fileName+="_"+beginDate.Month+"_"+beginDate.Day+"_"+beginDate.Year;
				//fileName+=".txt";
			
				StreamWriter sw= new StreamWriter(fileName);

				int howMany;//Get the options symbols for an underlying.
			
				howMany=ocfs.QUOTECHAINSynchronousFileSinkQuery(a,"QUOTECHAIN;1003;6050="+symbol,sw);
				Thread.Sleep(300);
				sw.Flush();
				sw.Close();

				StreamReader sr= new StreamReader(fileName);
		
				ArrayList optionSymbols=new ArrayList();

				bool eof=false;
				while(!eof)
				{
					string line=sr.ReadLine();


					
					if(line!=null)
					{
						//bbcrevisit - the TWL requires ' ' and we only store the symbol now
						//the old way was to store '+SYMTK'
						//Remove ' at beginning and end of line
						//line=line.Remove(0,1);
						//line=line.Remove(line.Length-1,1);
						optionSymbols.Add(line);
					}
					else
					{
						eof=true;
								
					}
					if(optionSymbols.Count>2000)
						break;
				}
				
				if(optionSymbols.Count>0)
				{
					//Remove the last one since it contains the root symbol per TAL instructions
	
					optionSymbols.RemoveAt(optionSymbols.Count-1);

					//Onew Way to get last five days of tics
					//klLiveChartTicToNas edbs=new klLiveChartTicToNas();
					//edbs.LIVECHARTSynchronousFileSinkQuery(a,optionSymbols,baseFileName);

					//Now make a volatility surface 
					klVolatilitySurface bvs=new klVolatilitySurface();
					Hashtable surface=bvs.makeSymbolHash(a,fileName);

					
					
					
					//Get 15 days of trades for each symbol in the quotechain
					klLiveChartOptionsBarFileSource efs=new klLiveChartOptionsBarFileSource();

					efs.m_baseFileName="c:/klTSDB/SPY/OPTIONS/";
										
					efs.LIVECHARTSynchronousFileSinkQuery( a,  optionSymbols,symbol);

					//Now make price statistics from Tic data using kl algorithm factory.

					//Use workflow example and wrap above in a data source.  The persistable items
					//should be the symbol hash with the options instance data, and a filename hash
					//that keeps track of the non-zero files written by the above query.


				}


				
				

			}

			///Use this to connect to livequote for OI - this is different than livechart query below
			///I do not believe we can get the OI from livechart
			//			//
			//			Hashtable WhatFID=new Hashtable();
			//			WhatFID.Add(1003,"DISP_NAME");
			//			WhatFID.Add(1016,"TRD_DATE");
			//			WhatFID.Add(1018,"TRDTIM_1");
			//			WhatFID.Add(1178,"TRDVOL_1");
			//			WhatFID.Add(1006,"TRDPRC_1");
			//			WhatFID.Add(1022,"BID");
			//			WhatFID.Add(1025,"ASK");
			//			
			//			//WhatFID.Add(9020,"TICK_TYPE");
			//			WhatFID.Add(1067,"EXPIR_DATE");
			//			WhatFID.Add(1066 ,"STRIKE_PRC");
			//			WhatFID.Add(1064, "OPINT_1");
			//				
			//			Hashtable WhereFID=new Hashtable();
			//			TQLTypeValuePair tvp=new TQLTypeValuePair("DISP_NAME","");//Leave Empty - we fill from array in helper function.
			//			WhereFID.Add(1003,tvp);
			//			
			//			//This sets the LIVEQUOTE instrument type FID.
			//			tvp=new TQLTypeValuePair("STYP","'2'");
			//			WhereFID.Add(2000,tvp);		
			//						
			//			d1 = DateTime.Now;
			//			date="_"+d1.Month+"_"+d1.Day+"_"+d1.Year;//d1.ToShortDateString();
			//			time= d1.TimeOfDay.Hours.ToString()+ d1.TimeOfDay.Minutes.ToString();//d1.ToShortTimeString();
			//			string filename =symbol+date+"_"+time+".txt";
			//			StreamWriter qqqq_optionschain=new StreamWriter(filename);
			//			
			//			ocfs.LIVEQUOTEOptionsSyncFileSinkQuery(a,sr,WhatFID,WhereFID,howMany,qqqq_optionschain);
			//			Thread.Sleep(3000);
			//			qqqq_optionschain.Flush();
			//			qqqq_optionschain.Close();



		{
			//ArrayList Optionsymbols=klSymbolCalculus.SymbolSet.OptionsSymbols();
			//klLiveChartTicToNas edbs=new klLiveChartTicToNas();
			//edbs.LIVECHARTSynchronousFileSinkQuery(a,Optionsymbols,"c:/klTicData2007/12_2007_OPT");

		}




		
		}
			
	}



}
