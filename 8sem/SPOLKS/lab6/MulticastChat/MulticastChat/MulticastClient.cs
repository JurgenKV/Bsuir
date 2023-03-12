using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace MulticastChat
{
    public class MulticastClient
    {
        private UdpClient udpClient;
        public IPEndPoint MulticastIpEndPoint { get; private set; }
        public IPEndPoint HostIpEndPoint { get; private set; }
        public int Port { get; private set; }

        public MulticastClient(string multicastGroup, int port)
        {
            udpClient = new UdpClient(port);
            udpClient.SubscribeToGroup(IPAddress.Parse(multicastGroup));
            MulticastIpEndPoint = new IPEndPoint(IPAddress.Parse(multicastGroup), port);


            foreach (var networkInterface in NetworkInterface.GetAllNetworkInterfaces())
            {
                if (networkInterface.OperationalStatus == OperationalStatus.Up &&
                    networkInterface.NetworkInterfaceType != NetworkInterfaceType.Loopback)
                {
                    var properties = networkInterface.GetIPProperties();
                    var addresses = properties.UnicastAddresses;
                    foreach (var address in addresses)
                    {
                        if (address.Address.AddressFamily == AddressFamily.InterNetwork)
                        {
                            HostIpEndPoint = new IPEndPoint(address.Address, port);
                        }
                    }
                }
            }

            Port = port;
        }

        public void Subscribe(IPAddress ipAddress)
        {
            udpClient.SubscribeToGroup(ipAddress);
        }

        public void UnSubscribe(IPAddress ipAddress)
        {
            udpClient.UnSubscribeToGroup(ipAddress);
        }

        public void OffLoopbackMessage()
        {
            udpClient.OffLoopbackMessage();
        }

        public void OnLoopbackMessage()
        {
            udpClient.OnLoopbackMessage();
        }


        public void Send(string message)
        {
            udpClient.Send(message, MulticastIpEndPoint);
        }

        public void Listen()
        {
            string message = "";
            var senderIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            while (true)
            {
                if (udpClient.Receive(ref message, ref senderIpEndPoint))
                {
                    if (message == "--ping")
                    {
                        udpClient.Send("--echo", senderIpEndPoint);
                        continue;
                    }
                    if (message == "--echo")
                    {
                        Console.WriteLine("User online: " + senderIpEndPoint.Address);
                        continue;
                    }
                    Console.WriteLine(senderIpEndPoint.Address + ":" + message);
                }
            }
        }
    }
}
