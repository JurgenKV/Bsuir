using System.Collections.Generic;

namespace spolks.HostAddressValidator
{
    public interface IAddressValidator
    {
        IList<string> Validate(IReadOnlyCollection<string> source);
    }
}