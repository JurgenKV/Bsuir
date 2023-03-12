using System.Collections.Generic;
using System.Linq;
using System.Net;
using spolks.Extensions;

namespace spolks.HostAddressValidator
{
    public class IpAddressValidator : AddressValidator
    {
        public IpAddressValidator(IAddressValidator validator) : base(validator)
        {
        }
        
        public override IList<string> Validate(IReadOnlyCollection<string> source)
        {
            var result = WrappedValidator?.Validate(source) ?? new List<string>();
            result.AddRange(source.Where(x => IPAddress.TryParse(x, out var temp)).ToList());

            return result;
        }
    }
}