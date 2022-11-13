using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;


namespace Lemer
{

    public class Lehmer
    {
        private const int a = 11817;
        private const int m = 36471;
        private int seed;
        public static int e = 100;

        public Lehmer(int seed)
        {
            if (seed <= 0 || seed == int.MaxValue)
                throw new Exception("Bad seed");
            this.seed = seed;
        }

        public int Next2()
        {
            int z1 = ((a * Program.ri) % m);

            return z1;
        }



    }

    class Program
    {

        public static int ri = 7;
        static void Main(string[] args)
        {

            SubValue();

            Console.WriteLine("");
            int[] counts = new int[100];

            Lehmer lehmer = new Lehmer(1);
            for (int i = 0; i < 10000; ++i)
            {
                ri = lehmer.Next2();
                int z1 = ri % Lehmer.e;
                ++counts[z1];
            }
            for (int i = 0; i < counts.Length; ++i)
            {
                Console.WriteLine("" + counts[i]);
            }
            Console.ReadLine();
        }

        static private void SubValue()
        {
            int r0 = 11817;
            int r1 = 36471;
            int r2 = 0;

            for (int i = 0; i < 20; i++)
            {
                r2 = r0 * r1;
                r1 = r0;
                r0 = int.Parse(r2.ToString().Substring(2));
                Console.WriteLine(r0);
            }
        }

    }
}
