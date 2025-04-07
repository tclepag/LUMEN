namespace LSHARP
{
    public class Game
    {
        // Static factory method for instance creation
        public static Game CreateInstance()
        {
            return new Game();
        }

        // Static method for initial testing
        public static int Hello()
        {
            return 42; // Return a value to confirm it works
        }

        // Optional diagnostic method
        public static string GetTypeInfo()
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            var assembly = typeof(Game).Assembly;
            sb.AppendLine($"Assembly: {assembly.FullName}");

            foreach (var type in assembly.GetTypes())
            {
                sb.AppendLine($"Type: {type.FullName}");
                foreach (var method in type.GetMethods())
                {
                    sb.AppendLine($"  Method: {method.Name} - Is Static: {method.IsStatic}");
                }
            }

            return sb.ToString();
        }

        // Instance methods
        public int Initialize()
        {
            return 0;
        }

        public int Start()
        {
            return 0;
        }

        public int Update()
        {
            return 0;
        }

        public int Render()
        {
            return 0;
        }

        public int Shutdown()
        {
            return 0;
        }
    }

    // Optional diagnostics class
    public class Diagnostics
    {
        public static string GetAssemblyInfo()
        {
            return Game.GetTypeInfo();
        }
    }
}
