using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using spolks.Extensions;

namespace spolks.Tracert
{
    public class TracertOrchestrator
    {
        public int Hops { get; set; } = 30;
        public int Timeout { get; set; } = 3000;
        
        public string Trace(string address)
        {
            var pings = CreatePings();
            var tasks = pings.Select(x => x.Run(address, Timeout)).ToList();
            Task.WhenAll(tasks);

            return Display(tasks);
        }

        private IList<PingTracert> CreatePings()
        {
            var pings = new List<PingTracert>();
            for (var index = 1; index < Hops + 1; index++)
            {
                pings.Add(new PingTracert(index));
            }

            return pings;
        }

        private string Display(IEnumerable<Task<PingReply>> tasks)
        {
            var builder = new StringBuilder("Tracert:" + Environment.NewLine);
            foreach (var task in tasks)
            {
                builder.AppendLine(task.Result.Print());
                if (task.Result.Status == IPStatus.Success)
                {
                    break;
                }
            }

            return builder.ToString();
        }
    }
}