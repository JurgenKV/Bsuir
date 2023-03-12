using System;
using System.Collections.Generic;
using System.Linq;
using spolks.Extensions;

namespace spolks.HostAddressValidator
{
    public class UriAddressValidator : AddressValidator
    {       
        public UriAddressValidator(IAddressValidator validator) : base(validator)
        {
        }
        
        public override IList<string> Validate(IReadOnlyCollection<string> source)
        {
            var result = WrappedValidator?.Validate(source) ?? new List<string>();
            var curResult = source.Where(x => Uri.IsWellFormedUriString(x, UriKind.Absolute))
                .Select(x => x.RemoveProtocol()).ToList();
            result.AddRange(curResult);

            return result;
        }
    }
}