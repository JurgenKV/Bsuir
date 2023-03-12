using System;
using System.Text;

namespace spolks.Extensions
{
    public static class StringX
    {
        public static string RemoveProtocol(this string uri)
        {
            return new StringBuilder(uri)
                .Replace("http://", string.Empty)
                .Replace("https://", string.Empty)
                .ToString();
        }
    }
}