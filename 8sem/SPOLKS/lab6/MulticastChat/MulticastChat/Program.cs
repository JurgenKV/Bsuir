using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MulticastChat
{
    class Program
    {
        static void Main(string[] args)
        {
            var multicastClient = new MulticastClient("225.4.5.6", 33333);
            var ipAdress = IPAddress.Parse("225.4.5.6"); 
            var listner = new Thread(multicastClient.Listen);
            listner.Start();
            Console.WriteLine("My ip: " + multicastClient.HostIpEndPoint.Address);
            Console.WriteLine("Multicast Address: " + multicastClient.MulticastIpEndPoint.Address);
            Console.WriteLine("Port: " + multicastClient.Port);
            Console.WriteLine("\"--exit\" for exit; \"--ping\" for check online user; \n\"--unsbscrb\" to unsubscribe of chat; \"--sbscrb\" to subscribe to chat;\n \"--offlb\" to off loopback message; \"--onlb\" to on loopback message;");
            while (true)
            {
                var message = Console.ReadLine();
                if (message == "--exit")
                {
                    break;
                }
                if (message == "--unsbscrb")
                {
                    multicastClient.UnSubscribe(ipAdress);
                    continue;
                }
                if (message == "--sbscrb")
                {
                    multicastClient.Subscribe(ipAdress);
                    continue;
                }
                if (message == "--offlb")
                {
                    multicastClient.OffLoopbackMessage();
                    continue;
                }
                if (message == "--onlb")
                {
                    multicastClient.OnLoopbackMessage();
                    continue;
                }

                multicastClient.Send(message);
            }
            listner.Abort();
        }
    }
}
