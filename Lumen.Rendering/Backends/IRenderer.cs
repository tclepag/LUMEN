using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lumen.Graphics.Backends
{
    internal interface IRenderer
    {
        public void Initialize(); // Builds the renderer
        public void Render(); // Draws with the renderer
        public void Cleanup(); // Cleans up renderer objects
    }
}
