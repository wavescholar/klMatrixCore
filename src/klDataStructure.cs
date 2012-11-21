using System;
using System.Collections;

namespace klDataStructure
{
	/// <summary>
	/// System.Collections conform class for a ring-queue.
	/// </summary>
	/// <remarks>
	/// The collection support adding and removing at both ends and
	/// automatic expansion.
	/// The left end of the ring is referred to as head, the right end as tail.
	/// add / remove is O(1)
	/// expansion is O(n)
	/// indexed access and enumeration is O(1)
	/// </remarks>
	public class Dequeue : ICollection, IEnumerable, ICloneable
	{
		protected object[] InnerList;
		protected double growthFactor;
		protected int Head,Tail,count;
		protected ulong version;

		#region contructors
		/// <summary>
		/// Create an empty Dequeu with capacity 32 and growth 2
		/// </summary>
		public Dequeue()
			: this(32,2.0)
		{}
		/// <summary>
		/// Create an empty Dequeu with given capacity and growth 2
		/// </summary>
		/// <param name="Capacity">the initial capacity of the collection</param>
		public Dequeue(int Capacity)
			: this(Capacity, 2.0)
		{}
		/// <summary>
		/// Create an empty Dequeu with given capacity and given growth
		/// </summary>
		/// <param name="Capacity">the initial capacity of the collection</param>
		/// <param name="GrowthFactor">the factor by which to grow the collection when the capacity is reached</param>
		public Dequeue(int Capacity, double GrowthFactor)
		{
			InnerList = new object[Capacity];
			this.GrowthFactor = GrowthFactor;
			Head = Tail = count = 0;
			version = 0;
		}
		/// <summary>
		/// Create a new Dequeu as a copy of the given collection
		/// </summary>
		/// <param name="C">The source collection</param>
		public Dequeue(ICollection C)
			: this(C,C.Count)
		{}
		/// <summary>
		/// Create a new Dequeu as a copy of the given collection and the given capacity
		/// </summary>
		/// <param name="C">The source collection</param>
		/// <param name="Capacity">The capacity of the new Dequeue (must be >= C.Count)</param>
		public Dequeue(ICollection C, int Capacity)
			: this(Capacity,2.0)
		{
            EnqueueTailRange(C);				
		}

		#endregion
		#region public methods
		/// <summary>
		/// Add the given object to the collections head
		/// </summary>
		/// <param name="value">The object to enqueue</param>
		public void EnqueueHead(object value)
		{
			if(Count==Capacity)
				SetSize((int)(Capacity*GrowthFactor));
			Head--;
			if(Head<0)
				Head+=Capacity;
			InnerList[Head] = value;
			count++;
			version++;
		}

		/// <summary>
		/// Add the given object to the collections tail
		/// </summary>
		/// <param name="value">The object to enqueue</param>
		public void EnqueueTail(object value)
		{
			if(Count==Capacity)
				SetSize((int)(Capacity*GrowthFactor));
			InnerList[Tail] = value;
			Tail++;
			Tail%=Capacity;
			count++;
			version++;
		}

		/// <summary>
		/// Retrieve and remove the current head
		/// </summary>
		/// <returns>The removed object</returns>
		public object DequeueHead()
		{
			if(Count==0)
				throw new Exception("Dequeue was empty!");
			object r = InnerList[Head];
			Head ++;
			Head %= Capacity;
			count--;
			version++;
			return r;
		}

		/// <summary>
		/// Retrieve and remove the current tail
		/// </summary>
		/// <returns>The removed object</returns>
//		public object DequeueTail()
//		{
//			if(Count==0)
//				throw new Exception("Dequeue was empty!");
//			object r = InnerList[Tail];
//			Tail --;
//			if(Tail<0)
//				Tail+=Capacity;
//			count--;
//			version++;
//			return r;
//		}

		public object DequeueTail() 
		{
			if(Count==0)
				throw new Exception("Dequeue was empty!");
			object r = InnerList[--Tail];
			if(Tail<0)
				Tail+=Capacity;
			count--;
			version++;
			return r;
		}



		/// <summary>
		/// Add the given collection to the dequeues tail
		/// </summary>
		/// <param name="C">The source collection</param>
		public void EnqueueTailRange(ICollection C)
		{
			int Cap = Capacity;
			while(Cap<C.Count)
				Cap = (int)(Cap*GrowthFactor);
			if(Cap>Capacity)
				SetSize(Cap);
			foreach(object O in C)
				EnqueueTail(O);
		}

		/// <summary>
		/// Add the given collection to the dequeues head.
		/// To preserve the order in the collection, the entries are
		/// added in revers order.
		/// </summary>
		/// <param name="C">The source collection</param>
		public void EnqueueHeadRange(ICollection C)
		{
			int Cap = Capacity;
			while(Cap<C.Count)
				Cap = (int)(Cap*GrowthFactor);
			if(Cap>Capacity)
				SetSize(Cap);
			ArrayList Temp = new ArrayList(C);
			Temp.Reverse();
			foreach(object O in Temp)
				EnqueueHead(O);
		}

		/// <summary>
		/// Deletes all entries from the collection
		/// </summary>
		public void Clear()
		{
			Head = Tail = count = 0;
			version++;
		}
		/// <summary>
		/// Sets the capacity to Count.
		/// </summary>
		public void TrimToSize()
		{
			SetSize(Count);
		}

		/// <summary>
		/// Indexed access to all elements currently in the collection.
		/// Indexing starts at 0 (head) and ends at Count-1 (tail).
		/// </summary>
		public object this [int index]
		{
			get
			{
				if(index<0 || index>=this.Count)
					throw new ArgumentOutOfRangeException("index");
				return InnerList[(Head+index)%Capacity];
			}
			set
			{
				if(index<0 || index>=this.Count)
					throw new ArgumentOutOfRangeException("index");
				InnerList[(Head+index)%Capacity] = value;
				version++;
			}
		}
		/// <summary>
		/// The current factor by which to grow the collection in case of expansion
		/// </summary>
		public double GrowthFactor
		{
			get {return growthFactor;}
			set{growthFactor = value;}
		}

		/// <summary>
		/// The current amount of cells available to the dequeue
		/// </summary>
		public int Capacity
		{
			get {return InnerList.Length;}
			set
			{
				if(Capacity>=Count)
					SetSize(Capacity);
				else
					throw new ArgumentException("Capacity was smaller than Count!");
			}
		}

		/// <summary>
		/// The current version of the dequeue. The version is increased with every changing operation.
		/// The main use is to invalidate all IEnumerators.
		/// </summary>
		public ulong Version
		{
			get {return version;}
		}
        
		/// <summary>
		/// Returns true.
		/// </summary>
		public bool IsSynchronized
		{
			get
			{
				return true;
			}
		}

		/// <summary>
		/// The current number of elements in the queue
		/// </summary>
		public int Count
		{
			get
			{
				return count;
			}
		}

		/// <summary>
		/// Implementation of the ICollection.CopyTo function.
		/// </summary>
		/// <param name="array">Target array</param>
		/// <param name="index">Start-Index in target array</param>
		public void CopyTo(Array array, int index)
		{
			if(array==null)
				throw new ArgumentNullException("array");
			if(index<0)
				throw new ArgumentOutOfRangeException("index",index,"Must be at least zero!");
			if(array.Length-index<Count)
				throw new ArgumentException("Array was to small!");
			if(array.Rank>1)
				throw new ArgumentException("Array was multidimensional!");
			int i;
			for(i=0;i<Count;i++)
			{
				array.SetValue(this[i],i+index);
			}
		}

		/// <summary>
		/// Returns this.
		/// </summary>
		public object SyncRoot
		{
			get
			{
				return this;
			}
		}

		/// <summary>
		/// Standard implementation.
		/// </summary>
		/// <returns>A DequeueEnumerator on the current dequeue</returns>
		public IEnumerator GetEnumerator()
		{
			return new DequeueEnumerator(this,0,Count-1);
		}

		/// <summary>
		/// Standard implementation.
		/// </summary>
		/// <returns>A Dequeue with a shallow copy of this one.</returns>
		public object Clone()
		{
			Dequeue E = new Dequeue(this,Capacity);
			E.GrowthFactor = this.growthFactor;
			E.version = this.Version;
			return E;
		}
		#endregion
		#region protected
		/// <summary>
		/// Sets the collections capacity to newSize
		/// </summary>
		/// <param name="newSize">the new collection size (must be >= Count)</param>
		protected void SetSize(int newSize)
		{
			if(newSize<Count)
				throw new ArgumentException("New Size was smaller than Count!");
			object[] O2 = new object[newSize];
			int i;
			for(i=0;i<Count;i++)
			{
				O2[i] = this[i];
			}
			Head = 0;
			Tail = Count;
			InnerList = O2;
			version++;
		}

		#endregion
		#region internal
		internal class DequeueEnumerator : IEnumerator
		{
			Dequeue Deq;
			int Start,End,Pos;
			ulong Version;
			object current;
			public DequeueEnumerator(Dequeue D, int Start, int End)
			{
				Deq = D;
				this.Version = Deq.Version;
                this.Start = Start;
				this.End = End;
				this.Pos = Start-1;
			}
			public void Reset()
			{
				Pos = Start-1;
				if(Version != Deq.Version)
					throw new InvalidOperationException("Collection was changed!");
			}

			public object Current
			{
				get
				{
					if(Pos<Start || Pos>End)
						throw new InvalidOperationException("Enumerator not valid!");
					return current;
				}
			}

			public bool MoveNext()
			{
				if(Version != Deq.Version)
					throw new InvalidOperationException("Collection was changed!");
				Pos++;
				if(Pos>End)
					return false;
				current = Deq[Pos];
				return true;
			}
		}
		#endregion
	}
}
