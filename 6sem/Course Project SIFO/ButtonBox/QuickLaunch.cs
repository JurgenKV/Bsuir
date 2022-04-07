using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ButtonBox
{
    class QuickLaunch
    {
        private string signal;
        private string filepath;
        

        public QuickLaunch( string signal, string filepath)
        {
            this.filepath = filepath;
            this.signal = signal;
        }

        public string Filepath { get => filepath; set => filepath = value; }
        public string Signal { get => signal; set => signal = value; }
    }
}
