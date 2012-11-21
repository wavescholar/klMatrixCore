using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;
using System.IO;
using System.Xml.Serialization;
namespace C_Sharp
{	
	//This class maintains a weak reference to data that is intended to be accessed in a bursty fashion.
	public class BurstyData : IDisposable
	{
		//This is a private nested class.  It's used internally to manage a large data set that is accessed
		//in a busrty fashion.  A weak reference to a Data object is kept by the main class and the data is
  		//regenerated when needed.  This should be a very short lived object, it's purpose is to maintain a file cache.
		[Serializable]
		[XmlRootAttribute("Data", Namespace = "http://www.wscmp.com",IsNullable = false)]
		public class Data
		{
			//Members must be public for serialization?  
			public List<double> data = new List<double>();

			public List<double> DataList
			{
				get { return data; }
				set { data = value; }
			}
			
			public String dataDesc;

			//The XmlAttribute instructs the XmlSerializer to serialize this field as 
			//an XML attribute instead of an XML element (the default behavior).
			[XmlAttribute]
			public String DataDesc
			{
				get { return dataDesc; }
				set { dataDesc = value; }
			}
			//Tolerance of Missing Data: Fields can be marked as optional by applying the 
			//OptionalFieldAttribute attribute to them. During deserialization, if the 
			//ptional data is missing, the serialization engine ignores the absence and does not throw an exception. 
			[OptionalField]
			//Setting the IsNullable property to false instructs the XmlSerializer that the XML 
			//attribute will not appear if this field is set to a null reference
			[XmlElementAttribute(IsNullable = false)]
			public DateTime when;
			public DateTime When
			{
				get { return when; }
				set { when = value; }
			}
			//This is immutable, we don't want our code to change it, so it declaired readonly
			public static readonly string FilePath = @"C:\temp\Data.xml";

			public void Serialize()
			{
				XmlSerializer serializer = new XmlSerializer(typeof(Data));
				TextWriter writer = new StreamWriter(FilePath);
				serializer.Serialize(writer, data);
			}
			public void Deserialize()
			{
				XmlSerializer serializer = new XmlSerializer(typeof(Data));
				//If the XML document has been altered with unknown  nodes or attributes, handle them with the 
				//UnknownNode and UnknownAttribute events.
				//TODO
				//serializer.UnknownNode+= new XmlNodeEventHandler(serializer_UnknownNode);
				//serializer.UnknownAttribute+= new  XmlAttributeEventHandler(serializer_UnknownAttribute);
				
				FileStream fs = new FileStream(FilePath, FileMode.Open);
				Data data = (Data)serializer.Deserialize(fs);
			}
			public static bool ClearDataCache()
			{
				try
				{
					File.Delete(FilePath);
					return true;
				}
				catch (Exception e)
				{
					Console.Write("Caught exception trying to clear cache" + e.Message);
					return false;
				}
			}

		};

		//A possible design flaw.  We have a hard reference to the data here, so it obviates the purpose of 
		//the weak reference.
		//If code in this class tries to set this directly, there will be a 
		//mismatch with the burstyObject and the cache.
		//TODO - design flaw?
		//private Data cache;

		//private Data Cache
		//{
		//    get { return cache; }
		//    //Anytime a cache object is set, we serialize it, and set the target of the WeakReference to
		//    //the data in the cache
		//    set 
		//    { 
		//        cache = value;
		//        cache.Serialize();
		//        _FileWeakRef.Target = cache.DataList;
		//    }
		//}

		public void SetData(List<double> list, String dataDesc)
		{
			//If the user wishes to replace the busrty data object we need to serialize the
			//data in case it needs to be regenerated, and set the burty object to the new one.
			Data data = new Data { DataList = list, DataDesc = dataDesc, When = DateTime.Now };

			data.Serialize();

			_FileWeakRef.Target = list;
			
			//The setter in the
			//Cache = data;
		}

		public void CreateData()
		{
			List<double> l = new List<double>();
			//Create 10,000 points sin(x) x \in [0,1]
			double delta = 1.0f/10000f;
			for (int i = 0; i < 10000; i++)
			{
				l.Add(Math.Sin(i * delta));
			}
			//Recall strings are interned
			String dataDesc = @"sin(x) x \in [0,1]";

			DateTime now = DateTime.Now;

			//Using object initializers to initialize type objects in a declarative manner 
			//without explicitly invoking a constructor for the type
			Data data = new Data{DataList = l,DataDesc = dataDesc,When = now};

			////Serialize it and set the BurstyObject to this one. -------->Don't need this since we implemented serialize and deserialize methods on the Data class.
			//XmlSerializer serializer = new XmlSerializer(typeof(Data));
			//TextWriter writer = new StreamWriter(data.FilePath);
			//serializer.Serialize(writer, data);

			data.Serialize();
			_FileWeakRef.Target = data.DataList;
		}
		
		public WeakReference _FileWeakRef = new WeakReference(null);

		//Takes a data object and returns the list - TODO optionally process data
		private List<double> ParseData(Data data)
		{
			return data.DataList;
		}

		private List<double> burstyObject;

		public List<double> BurstyObject
		{
			get {
				//This local ref object is imperative since the GC may run while the code below is being executed.
				List<double> fileStrongRef = _FileWeakRef.Target as List<double>;

				if (fileStrongRef == null) 
				{
					//Then we need to regenerate from out internal cache kept on disk.
					//If the data was never created or set, then there will certainly be a problem.
					Data data = new Data();

					data.Deserialize();

					fileStrongRef = data.DataList;

					fileStrongRef = ParseData(data);
				
					_FileWeakRef.Target = fileStrongRef;
				}
				//Do not try this: return _FileWeakRef.Target ,as the GC may have reclainmed our object already.  
				return fileStrongRef; 
				}
			set 
			{
				//We need to place the data in a Data object and set the cache.
				Data data = new Data { //Use object initialzer for Data class
					DataList =value,
					DataDesc = "No Data Desc, User Set Object Through Property",
					When = DateTime.Now};
				
				data.Serialize();
				_FileWeakRef.Target = value;

				//This is bad! It sets the cache object but does not serialize the data. 
				//If the WeakReference is reclaimed by the GC and we go to regenreate
				//cache = data; 
				//Cache = data;  //This setter serializes the cache to a file.
  
				burstyObject = value; 
			}
		}

		#region IDisposable And Finalization

		private bool disposed = false;

		//Implement IDisposable. This method is not virtual because an override 
		//could potentially leak the resource we are trying to clean up.
		public void Dispose()
		{
			Dispose(true);
			// This object will be cleaned up by the Dispose method.
			// Therefore we call GC.SupressFinalize to take this object off the finalization queue.
			// We do not want to run finalization code for this object twice
			GC.SuppressFinalize(this);
		}
		
		// If disposing equals true, the method has been called directly
		// or indirectly by a user's code. Managed and unmanaged resources can be disposed.
		// If disposing equals false, the method has been called by the
		// runtime from inside the finalizer and you should not reference
		// other objects. Only unmanaged resources can be disposed.
		protected virtual void Dispose(bool disposing)
		{
			// Check to see if Dispose has already been called.
			if (!this.disposed)
			{
				// If disposing equals true, dispose all managed
				// and unmanaged resources.
				if (disposing)
				{
					// Dispose managed resources.
					
				}
				// Calling the appropriate methods to clean up unmanaged resources.
				// If disposing is false, only the following code is executed.
				bool didClearFileCache = Data.ClearDataCache();
				Console.WriteLine("Attempt to celar cache resulted in {0}", didClearFileCache ? "Success" : "Failure");
				// Note disposing has been done.
				disposed = true;
			}
		}
		#endregion
	}
}
