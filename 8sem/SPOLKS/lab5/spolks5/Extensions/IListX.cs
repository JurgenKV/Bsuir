using System.Collections.Generic;

namespace spolks.Extensions
{
    public static class IListX
    {
        public static IList<T> AddRange<T>(this IList<T> collection, IList<T> source)
        {
            foreach (var item in source)
            {
                collection.Add(item);
            }

            return collection;
        }
    }
}