using System.Net.NetworkInformation;

namespace spolks.Extensions
{
    public static class PingReplyX
    {
        public static string Print(this PingReply reply)
        {
            return $"address: {reply.Address} | time: {reply.RoundtripTime} | status: {reply.Status}";
        }
    }
}