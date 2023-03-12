using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace BroadcastChat
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
            socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
            _port = port;
            socket.Bind(new IPEndPoint(IPAddress.Any, _port));
        }

        public int Send(string message, IPEndPoint destinationIp )
        {
            lock (sync)
            {
                return socket.SendTo(System.Text.Encoding.ASCII.GetBytes(message), destinationIp);
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
                var n = socket.ReceiveFrom(buffer,ref tempEndPoint);

                if (n <= 0) return false;
                
                endPoint = (IPEndPoint) tempEndPoint;
                message = Encoding.ASCII.GetString(buffer.Take(n).ToArray(), 0, n);
            }
            return true;
        }
    }
}
