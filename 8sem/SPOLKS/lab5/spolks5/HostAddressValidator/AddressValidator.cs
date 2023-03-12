using System.Collections.Generic;

namespace spolks.HostAddressValidator
{
    public abstract class AddressValidator : IAddressValidator
    {
        protected IAddressValidator WrappedValidator { get; }

        protected AddressValidator(IAddressValidator validator)
        {
            WrappedValidator = validator;
        }

        public abstract IList<string> Validate(IReadOnlyCollection<string> source);
    }
}