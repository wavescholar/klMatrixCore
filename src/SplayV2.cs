using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SplayTree
{
    public class Tree
    {

        public Tree Left;
        public Tree Right;
        public Tree Parent;
        public int Key;
		public string Value;
    }

    public class SplayTree
    {
        public void SetParentLeft(Tree t)
        {
            if (t.Left != null) t.Left.Parent = t;
        }
        public void SetParentRight(Tree t)
        {
            if (t.Right != null) t.Right.Parent = t;
        }

        Tree Splay(int i, Tree t)
        {
            Tree N, l, r, y;
            N = new Tree();
            if (t == null) return t;
            N.Left = N.Right = null;
            l = r = N;

            for (; ; )
            {
                if (i < t.Key)
                {
                    if (t.Left == null) break;
                    if (i < t.Left.Key)
                    {
                        y = t.Left;
                        t.Left = y.Right;
                        if (t.Left != null) t.Left.Parent = t;
                        y.Right = t;
                        t.Parent = y;
                        t = y;
                        if (t.Left == null) break;
                    }
                    r.Left = t;
                    t.Parent = r;
                    r = t;
                    t = t.Left;
                }
                else if (i > t.Key)
                {
                    if (t.Right == null) break;
                    if (i > t.Right.Key)
                    {
                        y = t.Right;
                        t.Right = y.Left;
                        if (t.Right != null) t.Right.Parent = t;
                        y.Left = t;
                        t.Parent = y;
                        t = y;
                        if (t.Right == null) break;
                    }
                    l.Right = t;
                    t.Parent = l;
                    l = t;
                    t = t.Right;
                }
                else
                {
                    break;
                }
            }
            l.Right = t.Left;
            if (l.Right != null) l.Right.Parent = l;
            r.Left = t.Right;
            if (r.Left != null) r.Left.Parent = r;
            t.Left = N.Right;
            if (t.Left != null) t.Left.Parent = t;
            t.Right = N.Left;
            if (t.Right != null) t.Right.Parent = t;
            t.Parent = null;
            return t;
        }

        Tree MoveToRoot(int i, Tree t)
        {
            Tree N = new Tree();
            Tree l, r, y;
            if (t == null) return t;
            N.Left = N.Right = null;
            l = r = N;

            for (; ; )
            {
                if (i < t.Key)
                {
                    if (t.Left == null) break;
                    r.Left = t;
                    t.Parent = r;
                    r = t;
                    t = t.Left;
                }
                else if (i > t.Key)
                {
                    if (t.Right == null) break;
                    l.Right = t;
                    t.Parent = l;
                    l = t;
                    t = t.Right;
                }
                else
                {
                    break;
                }
            }
            l.Right = t.Left;
            if (l.Right != null) l.Right.Parent = l;
            r.Left = t.Right;
            if (r.Left != null) r.Left.Parent = r;
            t.Left = N.Right;
            if (t.Left != null) t.Left.Parent = t;
            t.Right = N.Left;
            if (t.Right != null) t.Right.Parent = t;
            t.Parent = null;
            return t;
        }

        bool CheckTree(Tree t)
        {
            if (t == null) return true;
            if (t.Left != null && (t.Left.Parent != t || !CheckTree(t.Left)))
            {
                return false;
            }
            if (t.Right != null && (t.Right.Parent != t || !CheckTree(t.Right)))
            {
                return false;
            }
            return true;
        }

        static double INFINITY = (1 << 20);
        static int LABLEN = 20;

        public class Pnode
        {
            public Pnode left;
            public Pnode right;
            public int edge_length;

            public int height;
            public int lablen;
            public int parent_dir;       // -1=I  left, 0=I  root, 1=right       
            public String label;
			public String value;
        };

        Pnode BuildTreeRecurse(Tree t)
        {
            Pnode pn;
            if (t == null) return null;
            pn = new Pnode();
            pn.left = BuildTreeRecurse(t.Left);
            pn.right = BuildTreeRecurse(t.Right);

            if (pn.left != null) pn.left.parent_dir = -1;
            if (pn.right != null) pn.right.parent_dir = 1;
            pn.label = Convert.ToString(t.Key);
            pn.lablen = pn.label.Length;
			
			pn.value = t.Value;

            return pn;
        }

        /*
         * Copy the tree from the original structure into the Pnode structure
         * fill in the parent_dir, label, and labelen fields, but not the    
         * edge_length or height fields.                                     
         */
        Pnode BuildTree(Tree t)
        {
            Pnode pn;
            if (t == null) return null;
            pn = BuildTreeRecurse(t);
            pn.parent_dir = 0;
            return pn;
        }

        /* 
         * The lprofile array is description of the left profile of a tree.
         * Assuming the root is located at (0,0), lprofile[i] is the leftmost
         * point used on row i of the tree.  rprofile is similarly defined.
         */
        static int MAX_HEIGHT = 1000;
        static int[] lprofile = new int[MAX_HEIGHT];
        static int[] rprofile = new int[MAX_HEIGHT];

        /*
         * The following function fills in the lprofile array for the given tree.
         * It assumes that the center of the label of the root of this tree
         * is located at a position (x,y).  It assumes that the edge_length
         * fields have been computed for this tree.
         */
        void ComputeLeftProfile(Pnode pn, int x, int y)
        {
            int i;
            int isleft;
            if (pn == null) return;
            if (pn.parent_dir == -1)
                isleft = 1;
            else
                isleft = 0;
            lprofile[y] = Math.Min(lprofile[y], x - ((pn.lablen - isleft) / 2));
            if (pn.left != null)
            {
                for (i = 1; i <= pn.edge_length && y + i < MAX_HEIGHT; i++)
                {
                    lprofile[y + i] = Math.Min(lprofile[y + i], x - i);
                }
            }
            ComputeLeftProfile(pn.left, x - pn.edge_length - 1, y + pn.edge_length + 1);
            ComputeLeftProfile(pn.right, x + pn.edge_length + 1, y + pn.edge_length + 1);
        }
        void ComputeRightProfile(Pnode pn, int x, int y)
        {
            int i, notleft;
            if (pn == null) return;
            if (pn.parent_dir != -1)
                notleft = 1;
            else
                notleft = 0;
            rprofile[y] = Math.Max(rprofile[y], x + ((pn.lablen - notleft) / 2));
            if (pn.right != null)
            {
                for (i = 1; i <= pn.edge_length && y + i < MAX_HEIGHT; i++)
                {
                    rprofile[y + i] = Math.Max(rprofile[y + i], x + i);
                }
            }
            ComputeRightProfile(pn.left, x - pn.edge_length - 1, y + pn.edge_length + 1);
            ComputeRightProfile(pn.right, x + pn.edge_length + 1, y + pn.edge_length + 1);
        }

        /* 
         * This function fills in the edge_length and height fields of the
         * specified tree.
         */
        void ComputeEdgeLengths(Pnode pn)
        {
            int h, hmin, i, delta;
            if (pn == null) return;
            ComputeEdgeLengths(pn.left);
            ComputeEdgeLengths(pn.right);

            /* first fill in the edge_length of pn */
            if (pn.right == null && pn.left == null)
            {
                pn.edge_length = 0;
            }
            else
            {
                if (pn.left != null)
                {
                    for (i = 0; i < pn.left.height && i < MAX_HEIGHT; i++)
                    {
                        rprofile[i] = -(int)INFINITY;
                    }
                    ComputeRightProfile(pn.left, 0, 0);
                    hmin = pn.left.height;
                }
                else
                {
                    hmin = 0;
                }
                if (pn.right != null)
                {
                    for (i = 0; i < pn.right.height && i < MAX_HEIGHT; i++)
                    {
                        lprofile[i] = (int)INFINITY;
                    }
                    ComputeLeftProfile(pn.right, 0, 0);
                    hmin = Math.Min(pn.right.height, hmin);
                }
                else
                {
                    hmin = 0;
                }
                delta = 4;
                for (i = 0; i < hmin; i++)
                {
                    delta = Math.Max(delta, 2 + 1 + rprofile[i] - lprofile[i]);
                    /* the "2" guarantees a gap of 2 between different parts of the tree */
                }
                /* If the node has two children of height 1, then we allow the
                       two leaves to be within 1, instead of 2 */
                if (((pn.left != null && pn.left.height == 1) ||
                    (pn.right != null && pn.right.height == 1)) && delta > 4) delta--;
                pn.edge_length = ((delta + 1) / 2) - 1;
            }

            /* now fill in the height of pn */
            h = 1;
            if (pn.left != null)
            {
                h = Math.Max(pn.left.height + pn.edge_length + 1, h);
            }
            if (pn.right != null)
            {
                h = Math.Max(pn.right.height + pn.edge_length + 1, h);
            }
            pn.height = h;
        }

        int print_next;  /* used by print_level.  If you call "printf()" at   */
        /* any point, this is the x coordinate of the next   */
        /* char printed.                                     */

        int size;

        /*
         * This function prints the given level of the given tree, assuming
         * that the node pn has the given x cordinate.
         */
        void print_level(Pnode pn, int x, int level)
        {
            int i, isleft;
            if (pn == null) return;
            if (pn.parent_dir == -1)
                isleft = 1;
            else
                isleft = 0;
            if (level == 0)
            {
                for (i = 0; i < (x - print_next - ((pn.lablen - isleft) / 2)); i++)
                {
                    System.Console.Write(" ");
                }
                print_next += i;
                System.Console.Write(pn.label +  " " +  pn.value);
                print_next += pn.lablen;
            }
            else if (pn.edge_length >= level)
            {
                if (pn.left != null)
                {
                    for (i = 0; i < (x - print_next - (level)); i++)
                    {
                        System.Console.Write(" ");
                    }
                    print_next += i;
                    System.Console.Write("/");
                    print_next++;
                }
                if (pn.right != null)
                {
                    for (i = 0; i < (x - print_next + (level)); i++)
                    {
                        System.Console.Write(" ");
                    }
                    print_next += i;
                    System.Console.Write("\\");
                    print_next++;
                }
            }
            else
            {
                print_level(pn.left, x - pn.edge_length - 1, level - pn.edge_length - 1);
                print_level(pn.right, x + pn.edge_length + 1, level - pn.edge_length - 1);
            }
        }

        /* 
         * This pretty-prints the given tree, left-justified.
         * The tree is drawn in such a way that both of the edges down from
         * a node are the same length.  This length is the minimum such that
         * the two subtrees are separated by at least two blanks.
         */
        public void PrintTreeASCII(Tree t)
        {
            Pnode proot;
            int xmin, i;
            if (t == null) return;
            proot = BuildTree(t);
            ComputeEdgeLengths(proot);
            for (i = 0; i < proot.height && i < MAX_HEIGHT; i++)
            {
                lprofile[i] = (int)INFINITY;
            }
            ComputeLeftProfile(proot, 0, 0);
            xmin = 0;
            for (i = 0; i < proot.height && i < MAX_HEIGHT; i++)
            {
                xmin = Math.Min(xmin, lprofile[i]);
            }
            for (i = 0; i < proot.height; i++)
            {
                print_next = 0;
                print_level(proot, -xmin, i);
                System.Console.Write("\n");
            }
            if (proot.height >= MAX_HEIGHT)
            {
                System.Console.Write("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
            }

        }

        public Tree insert(int i, string val, Tree t)
        {
            /* Insert i into the tree t, unless it's already there.    */
            /* Return a pointer to the resulting tree.                 */
            Tree newTree;

            newTree = new Tree();
            newTree.Key = i;
			newTree.Value = val;
            if (t == null)
            {
                newTree.Left = newTree.Right = newTree.Parent = null;
                size = 1;
                return newTree;
            }
            t = Splay(i, t);
            if (i < t.Key)
            {
                newTree.Left = t.Left;
                newTree.Right = t;
                t.Left = null;
                SetParentLeft(newTree);
                SetParentRight(newTree);
                size++;
                return newTree;
            }
            else if (i > t.Key)
            {
                newTree.Right = t.Right;
                newTree.Left = t;
                t.Right = null;
                SetParentLeft(newTree);
                SetParentRight(newTree);
                size++;
                return newTree;
            }
            else
            {
                return t;
            }
        }

        public Tree delete(int i, Tree t)
        {
            Tree x;
            if (t == null) return null;
            t = Splay(i, t);
            if (i == t.Key)
            {
                if (t.Left == null)
                {
                    x = t.Right;
                }
                else
                {
                    x = Splay(i, t.Left);
                    x.Right = t.Right;
                    SetParentRight(x);
                }
                size--;
                return x;
            }
            return t;
        }

        public void RunTest()
        {
			string path = "C:\\KL\\Data\\Books\\KeatsShelley.txt";
			Dictionary<string, int> words = new Dictionary<string, int>();
			List<string> flatWordList = new List<string>();
			string[] lines = File.ReadAllLines(path);
			string[] splitters = { " ", ".", ",", ";", ":", "?", "!" };
			foreach (string line in lines)
			{
				string[] split = line.Split(splitters, StringSplitOptions.RemoveEmptyEntries); ;
				foreach (string word in split)
				{
					flatWordList.Add(word);
					if (words.ContainsKey(word))
						words[word]++;
					else
						words.Add(word, 1);
				}
			}

            Tree root;
            //char[] line = new char[100];
            int i, N;
            root = null;
            size = 0;
            while (true)
            {
                System.Console.WriteLine("Enter the number of nodes in the tree: ");
                N = -1;
                String numberS = System.Console.ReadLine();
                N = Convert.ToInt32(numberS);

                if ((N < 1) || (N > 200))
                {
                    System.Console.WriteLine("Choose a number between 1 and 200.\n");
                    continue;
                }
                break;
            }
            for (i = 0; i < N; i++)
            {
				
                root = insert(i,flatWordList[i], root);
                if (!CheckTree(root)) System.Console.WriteLine("error\n"); ;
            }
            PrintTreeASCII(root);
            do
            {
                System.Console.WriteLine("Select a node to splay: ");

                String nodeToSplayS = System.Console.ReadLine();
                int nodeToSplay = Convert.ToInt32(nodeToSplayS);

                if (nodeToSplay <= 0)
                    break;
                root = Splay(nodeToSplay, root);
                PrintTreeASCII(root);
            } while (true);
        }

    }



	public class StringSplayTree
	{
		public void SetParentLeft(Tree t)
		{
			if (t.Left != null) t.Left.Parent = t;
		}
		public void SetParentRight(Tree t)
		{
			if (t.Right != null) t.Right.Parent = t;
		}

		Tree Splay(string i, Tree t)
		{
			Tree N, l, r, y;
			N = new Tree();
			if (t == null) return t;
			N.Left = N.Right = null;
			l = r = N;

			for (; ; )
			{
				if (String.Compare(i, t.Value) < 0)
				{
					if (t.Left == null) break;
					if (String.Compare(i, t.Left.Value) <0)
					{
						y = t.Left;
						t.Left = y.Right;
						if (t.Left != null) t.Left.Parent = t;
						y.Right = t;
						t.Parent = y;
						t = y;
						if (t.Left == null) break;
					}
					r.Left = t;
					t.Parent = r;
					r = t;
					t = t.Left;
				}
				else if (String.Compare(i,t.Value) >0)
				{
					if (t.Right == null) break;
					if (String.Compare(i, t.Right.Value)>0)
					{
						y = t.Right;
						t.Right = y.Left;
						if (t.Right != null) t.Right.Parent = t;
						y.Left = t;
						t.Parent = y;
						t = y;
						if (t.Right == null) break;
					}
					l.Right = t;
					t.Parent = l;
					l = t;
					t = t.Right;
				}
				else
				{
					break;
				}
			}
			l.Right = t.Left;
			if (l.Right != null) l.Right.Parent = l;
			r.Left = t.Right;
			if (r.Left != null) r.Left.Parent = r;
			t.Left = N.Right;
			if (t.Left != null) t.Left.Parent = t;
			t.Right = N.Left;
			if (t.Right != null) t.Right.Parent = t;
			t.Parent = null;
			return t;
		}

		Tree MoveToRoot(String i, Tree t)
		{
			Tree N = new Tree();
			Tree l, r, y;
			if (t == null) return t;
			N.Left = N.Right = null;
			l = r = N;

			for (; ; )
			{
				if (String.Compare(i,t.Value)<0 )
				{
					if (t.Left == null) break;
					r.Left = t;
					t.Parent = r;
					r = t;
					t = t.Left;
				}
				else if (String.Compare(i,t.Value) >0)
				{
					if (t.Right == null) break;
					l.Right = t;
					t.Parent = l;
					l = t;
					t = t.Right;
				}
				else
				{
					break;
				}
			}
			l.Right = t.Left;
			if (l.Right != null) l.Right.Parent = l;
			r.Left = t.Right;
			if (r.Left != null) r.Left.Parent = r;
			t.Left = N.Right;
			if (t.Left != null) t.Left.Parent = t;
			t.Right = N.Left;
			if (t.Right != null) t.Right.Parent = t;
			t.Parent = null;
			return t;
		}

		bool CheckTree(Tree t)
		{
			if (t == null) return true;
			if (t.Left != null && (t.Left.Parent != t || !CheckTree(t.Left)))
			{
				return false;
			}
			if (t.Right != null && (t.Right.Parent != t || !CheckTree(t.Right)))
			{
				return false;
			}
			return true;
		}

		static double INFINITY = (1 << 20);
		static int LABLEN = 20;

		public class Pnode
		{
			public Pnode left;
			public Pnode right;
			public int edge_length;

			public int height;
			public int lablen;
			public int parent_dir;       // -1=I  left, 0=I  root, 1=right       
			public String label;
			public String value;
		};

		Pnode BuildTreeRecurse(Tree t)
		{
			Pnode pn;
			if (t == null) return null;
			pn = new Pnode();
			pn.left = BuildTreeRecurse(t.Left);
			pn.right = BuildTreeRecurse(t.Right);
			if (pn.left != null) pn.left.parent_dir = -1;
			if (pn.right != null) pn.right.parent_dir = 1;
			pn.label = Convert.ToString(t.Key);
			pn.value = t.Value;
			pn.lablen = pn.label.Length;

			return pn;
		}

		/*
		 * Copy the tree from the original structure into the Pnode structure
		 * fill in the parent_dir, label, and labelen fields, but not the    
		 * edge_length or height fields.                                     
		 */
		Pnode BuildTree(Tree t)
		{
			Pnode pn;
			if (t == null) return null;
			pn = BuildTreeRecurse(t);
			pn.parent_dir = 0;
			return pn;
		}

		/* 
		 * The lprofile array is description of the left profile of a tree.
		 * Assuming the root is located at (0,0), lprofile[i] is the leftmost
		 * point used on row i of the tree.  rprofile is similarly defined.
		 */
		static int MAX_HEIGHT = 1000;
		static int[] lprofile = new int[MAX_HEIGHT];
		static int[] rprofile = new int[MAX_HEIGHT];

		/*
		 * The following function fills in the lprofile array for the given tree.
		 * It assumes that the center of the label of the root of this tree
		 * is located at a position (x,y).  It assumes that the edge_length
		 * fields have been computed for this tree.
		 */
		void ComputeLeftProfile(Pnode pn, int x, int y)
		{
			int i;
			int isleft;
			if (pn == null) return;
			if (pn.parent_dir == -1)
				isleft = 1;
			else
				isleft = 0;
			lprofile[y] = Math.Min(lprofile[y], x - ((pn.lablen - isleft) / 2));
			if (pn.left != null)
			{
				for (i = 1; i <= pn.edge_length && y + i < MAX_HEIGHT; i++)
				{
					lprofile[y + i] = Math.Min(lprofile[y + i], x - i);
				}
			}
			ComputeLeftProfile(pn.left, x - pn.edge_length - 1, y + pn.edge_length + 1);
			ComputeLeftProfile(pn.right, x + pn.edge_length + 1, y + pn.edge_length + 1);
		}
		void ComputeRightProfile(Pnode pn, int x, int y)
		{
			int i, notleft;
			if (pn == null) return;
			if (pn.parent_dir != -1)
				notleft = 1;
			else
				notleft = 0;
			rprofile[y] = Math.Max(rprofile[y], x + ((pn.lablen - notleft) / 2));
			if (pn.right != null)
			{
				for (i = 1; i <= pn.edge_length && y + i < MAX_HEIGHT; i++)
				{
					rprofile[y + i] = Math.Max(rprofile[y + i], x + i);
				}
			}
			ComputeRightProfile(pn.left, x - pn.edge_length - 1, y + pn.edge_length + 1);
			ComputeRightProfile(pn.right, x + pn.edge_length + 1, y + pn.edge_length + 1);
		}

		/* 
		 * This function fills in the edge_length and height fields of the
		 * specified tree.
		 */
		void ComputeEdgeLengths(Pnode pn)
		{
			int h, hmin, i, delta;
			if (pn == null) return;
			ComputeEdgeLengths(pn.left);
			ComputeEdgeLengths(pn.right);

			/* first fill in the edge_length of pn */
			if (pn.right == null && pn.left == null)
			{
				pn.edge_length = 0;
			}
			else
			{
				if (pn.left != null)
				{
					for (i = 0; i < pn.left.height && i < MAX_HEIGHT; i++)
					{
						rprofile[i] = -(int)INFINITY;
					}
					ComputeRightProfile(pn.left, 0, 0);
					hmin = pn.left.height;
				}
				else
				{
					hmin = 0;
				}
				if (pn.right != null)
				{
					for (i = 0; i < pn.right.height && i < MAX_HEIGHT; i++)
					{
						lprofile[i] = (int)INFINITY;
					}
					ComputeLeftProfile(pn.right, 0, 0);
					hmin = Math.Min(pn.right.height, hmin);
				}
				else
				{
					hmin = 0;
				}
				delta = 4;
				for (i = 0; i < hmin; i++)
				{
					delta = Math.Max(delta, 2 + 1 + rprofile[i] - lprofile[i]);
					/* the "2" guarantees a gap of 2 between different parts of the tree */
				}
				/* If the node has two children of height 1, then we allow the
					   two leaves to be within 1, instead of 2 */
				if (((pn.left != null && pn.left.height == 1) ||
					(pn.right != null && pn.right.height == 1)) && delta > 4) delta--;
				pn.edge_length = ((delta + 1) / 2) - 1;
			}

			/* now fill in the height of pn */
			h = 1;
			if (pn.left != null)
			{
				h = Math.Max(pn.left.height + pn.edge_length + 1, h);
			}
			if (pn.right != null)
			{
				h = Math.Max(pn.right.height + pn.edge_length + 1, h);
			}
			pn.height = h;
		}

		int print_next;  /* used by print_level.  If you call "printf()" at   */
		/* any point, this is the x coordinate of the next   */
		/* char printed.                                     */

		int size;

		/*
		 * This function prints the given level of the given tree, assuming
		 * that the node pn has the given x cordinate.
		 */
		void print_level(Pnode pn, int x, int level)
		{
			int i, isleft;
			if (pn == null) return;
			if (pn.parent_dir == -1)
				isleft = 1;
			else
				isleft = 0;
			if (level == 0)
			{
				for (i = 0; i < (x - print_next - ((pn.lablen - isleft) / 2)); i++)
				{
					System.Console.Write(" ");
				}
				print_next += i;
				System.Console.Write(pn.value);
				print_next += pn.lablen;
			}
			else if (pn.edge_length >= level)
			{
				if (pn.left != null)
				{
					for (i = 0; i < (x - print_next - (level)); i++)
					{
						System.Console.Write(" ");
					}
					print_next += i;
					System.Console.Write("/");
					print_next++;
				}
				if (pn.right != null)
				{
					for (i = 0; i < (x - print_next + (level)); i++)
					{
						System.Console.Write(" ");
					}
					print_next += i;
					System.Console.Write("\\");
					print_next++;
				}
			}
			else
			{
				print_level(pn.left, x - pn.edge_length - 1, level - pn.edge_length - 1);
				print_level(pn.right, x + pn.edge_length + 1, level - pn.edge_length - 1);
			}
		}

		/* 
		 * This pretty-prints the given tree, left-justified.
		 * The tree is drawn in such a way that both of the edges down from
		 * a node are the same length.  This length is the minimum such that
		 * the two subtrees are separated by at least two blanks.
		 */
		public void PrintTreeASCII(Tree t)
		{
			Pnode proot;
			int xmin, i;
			if (t == null) return;
			proot = BuildTree(t);
			ComputeEdgeLengths(proot);
			for (i = 0; i < proot.height && i < MAX_HEIGHT; i++)
			{
				lprofile[i] = (int)INFINITY;
			}
			ComputeLeftProfile(proot, 0, 0);
			xmin = 0;
			for (i = 0; i < proot.height && i < MAX_HEIGHT; i++)
			{
				xmin = Math.Min(xmin, lprofile[i]);
			}
			for (i = 0; i < proot.height; i++)
			{
				print_next = 0;
				print_level(proot, -xmin, i);
				System.Console.Write("\n");
			}
			if (proot.height >= MAX_HEIGHT)
			{
				System.Console.Write("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
			}

		}

		public Tree insert(String i, Tree t)
		{
			/* Insert i into the tree t, unless it's already there.    */
			/* Return a pointer to the resulting tree.                 */
			Tree newTree;

			newTree = new Tree();
			newTree.Value = i;
			if (t == null)
			{
				newTree.Left = newTree.Right = newTree.Parent = null;
				size = 1;
				return newTree;
			}
			t = Splay(i, t);
			if (String.Compare(i ,t.Value)<0)
			{
				newTree.Left = t.Left;
				newTree.Right = t;
				t.Left = null;
				SetParentLeft(newTree);
				SetParentRight(newTree);
				size++;
				return newTree;
			}
			else if (String.Compare (i,t.Value) >0)
			{
				newTree.Right = t.Right;
				newTree.Left = t;
				t.Right = null;
				SetParentLeft(newTree);
				SetParentRight(newTree);
				size++;
				return newTree;
			}
			else
			{
				return t;
			}
		}

		public Tree delete(String i, Tree t)
		{
			Tree x;
			if (t == null) return null;
			t = Splay(i, t);
			if (String.Compare(i,t.Value) ==0)
			{
				if (t.Left == null)
				{
					x = t.Right;
				}
				else
				{
					x = Splay(i, t.Left);
					x.Right = t.Right;
					SetParentRight(x);
				}
				size--;
				return x;
			}
			return t;
		}

		public void RunTest()
		{
			string path = "C:\\KL\\Data\\Books\\KeatsShelley.txt";
			Dictionary<string, int> words = new Dictionary<string, int>();
			List<string> flatWordList = new List<string>();
			string[] lines = File.ReadAllLines(path);
			string[] splitters = { " ", ".", ",", ";", ":", "?", "!" };
			foreach (string line in lines)
			{
				string[] split = line.Split(splitters, StringSplitOptions.RemoveEmptyEntries); ;
				foreach (string word in split)
				{					
					if (words.ContainsKey(word))
						words[word]++;
					else
					{
						words.Add(word, 1);
						if (word.Length != 0)
						{
							flatWordList.Add(word);
						}

					}
				}
			}
			flatWordList.Sort();
			flatWordList.Reverse();

			Tree root;
			int i, N;
			root = null;
			size = 0;
			while (true)
			{
				System.Console.WriteLine("Enter the number of nodes in the tree: ");
				N = -1;
				String numberS = System.Console.ReadLine();
				N = Convert.ToInt32(numberS);

				if ((N < 1) || (N > 200))
				{
					System.Console.WriteLine("Choose a number between 1 and 200.\n");
					continue;
				}
				break;
			}
			for (i = 0; i < N; i++)
			{
				root = insert(flatWordList[i +(int) (flatWordList.Count /2.0) ], root);
				if (!CheckTree(root)) System.Console.WriteLine("error\n"); ;
			}
			PrintTreeASCII(root);
			do
			{
				System.Console.WriteLine("Select a node to splay: ");

				String nodeToSplayS = System.Console.ReadLine();
				root = Splay(nodeToSplayS, root);
				PrintTreeASCII(root);
			} while (true);
		}

	}

    class Program
    {
        static void RunSplayTest(string[] args)
        {
            StringSplayTree sp = new StringSplayTree();
            sp.RunTest();

        }
    }
}
