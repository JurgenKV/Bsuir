using System.Net.NetworkInformation;
using System.Threading.Tasks;

namespace spolks.Tracert
{
    public class PingTracert
    {
        private readonly Ping ping;
        private readonly PingOptions pingOptions;
        private readonly byte[] data = new byte[0];

        public PingTracert(int n)
        {
            pingOptions = new PingOptions(n, true);
            ping = new Ping();
        }

        public Task<PingReply> Run(string address, int timeout)
        {
            return ping.SendPingAsync(address, timeout, data, pingOptions);
        }
    }
}