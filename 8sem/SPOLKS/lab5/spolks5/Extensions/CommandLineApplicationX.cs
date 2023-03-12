using Microsoft.Extensions.CommandLineUtils;

namespace spolks.Extensions
{
    public static class CommandLineApplicationX
    {
        public static void SetInitialConfiguration(this CommandLineApplication app, string description)
        {
            app.Description = description;
            app.HelpOption("-?|-h|--help");
        }
    }
}