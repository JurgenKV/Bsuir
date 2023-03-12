namespace spolks.HostAddressValidator
{
    public static class AddressValidatorProvider
    {
        public static IAddressValidator GetValidator()
        {
            var firstValidator = new IpAddressValidator(null);
            
            return new UriAddressValidator(firstValidator);
        }
    }
}