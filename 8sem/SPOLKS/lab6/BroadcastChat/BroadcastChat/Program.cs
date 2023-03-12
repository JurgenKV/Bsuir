using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BroadcastChat
{
    class Program
    {
        static void Main(string[] args)
        {

            var broadcastClient = new BroadcastClient(33333);
            var listner = new Thread(broadcastClient.Listen);
            listner.Start();
            Console.WriteLine("My ip: " + broadcastClient.HostIpEndPoint.Address);
            Console.WriteLine("\"--exit\" for exit; \"--ping\" for check online user");
            while (true)
            {
                var message = Console.ReadLine();
                if (message == "--exit")
                {
                    break;
                }

                broadcastClient.Send(message);
            }
            listner.Abort();
        }
    }
}
