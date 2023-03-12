using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using spolks.Extensions;

namespace spolks.ParallelPing
{
    public class PingManager
    {
        private readonly List<string> hosts;
        
        public PingManager()
        {
            hosts = new List<string>();
        }

        public PingManager AddRange(IEnumerable<string> hosts)
        {
            this.hosts.AddRange(hosts);

            return this;
        }

        public PingManager Add(string host)
        {
            hosts.Add(host);

            return this;
        }

        public PingManager Clear()
        {
            hosts.Clear();

            return this;
        }

        public PingManager Remove(string host)
        {
            hosts.Remove(host);

            return this;
        }

        public string DisplayHosts()
        {
            var builder = new StringBuilder();
            builder.AppendJoin(Environment.NewLine, hosts);

            return builder.ToString();
        }

        public string Ping()
        {
            var tasks = hosts.Select(Ping).ToList();
            Task.WhenAll(tasks);

            return GetResult(tasks);
        }

        private string GetResult(IEnumerable<Task<PingReply>> tasks)
        {
            var builder = new StringBuilder();
            var results = tasks.Select(x => x.Result.Print());
            builder.AppendJoin(Environment.NewLine, results);

            return builder.ToString();
        }
    
        private static Task<PingReply> Ping(string ipAddress)
        {
            var ping = new Ping();
            return ping.SendPingAsync(ipAddress);
        }
    }
}