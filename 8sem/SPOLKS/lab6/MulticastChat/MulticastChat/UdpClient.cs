using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace MulticastChat
{
    public class UdpClient
    {
        private Socket socket;
        protected object sync = new object();
        private readonly int _port;

        public UdpClient(int port)
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);

            _port = port;
            socket.Bind(new IPEndPoint(IPAddress.Any, _port));
        }

        public void SubscribeToGroup(IPAddress groupIpAddress)
        {

            var mcastOption = new MulticastOption(groupIpAddress);
            socket.SetSocketOption(SocketOptionLevel.IP,
                            SocketOptionName.AddMembership,
                            mcastOption);
            socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastTimeToLive, 4);
        }

        public void UnSubscribeToGroup(IPAddress groupIpAddress)
        {
            var mcastOption = new MulticastOption(groupIpAddress);

            socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.DropMembership, mcastOption);
        }

        public void OffLoopbackMessage()
        {
            socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastLoopback, 0);
        }

        public void OnLoopbackMessage()
        {
            socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastLoopback, 1);
        }

        public int Send(string message, IPEndPoint destinationIp)
        {
            lock (sync)
            {
                return socket.SendTo(Encoding.ASCII.GetBytes(message), destinationIp);
            }
        }

        public bool Receive(ref string message, ref IPEndPoint endPoint)
        {
            lock (sync)
            {
                if (!socket.Poll(200, SelectMode.SelectRead))
                {
                    return false;
                }
                var buffer = new byte[2048];
                EndPoint tempEndPoint = new IPEndPoint(IPAddress.Any, _port);
                var n = socket.ReceiveFrom(buffer, ref tempEndPoint);

                if (n <= 0) return false;

                endPoint = (IPEndPoint)tempEndPoint;
                message = Encoding.ASCII.GetString(buffer.Take(n).ToArray(), 0, n);
            }
            return true;
        }
    }
}
