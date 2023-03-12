using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace BroadcastChat
{
    public class BroadcastClient
    {
        private UdpClient udpClient;
        public IPEndPoint BroadcastIpEndPoint { get; private set; }
        public IPEndPoint HostIpEndPoint { get; private set; }
   
        public BroadcastClient(int port)
        {
            udpClient = new UdpClient(port);

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
                            GetBroadcastIp(address, port);
                            HostIpEndPoint = new IPEndPoint(address.Address, port);
                        }
                    }
                }
            }
            Console.WriteLine("Broadcast ip: " + BroadcastIpEndPoint.Address);
        }

        private void GetBroadcastIp(UnicastIPAddressInformation ipAddress, int port)
        {
            byte[] ipAdressBytes = ipAddress.Address.GetAddressBytes();
            byte[] subnetMaskBytes = ipAddress.IPv4Mask.GetAddressBytes();
            byte[] broadcastAddress = new byte[ipAdressBytes.Length];
            for (int i = 0; i < broadcastAddress.Length; i++)
            {
                broadcastAddress[i] = (byte)(ipAdressBytes[i] | (subnetMaskBytes[i] ^ 255));
            }
            BroadcastIpEndPoint = new IPEndPoint(new IPAddress(broadcastAddress), port);
        }

        public void Send(string message)
        {
            udpClient.Send(message, BroadcastIpEndPoint);
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
