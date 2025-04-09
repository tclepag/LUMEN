using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Lumen.Graphics.Backends.D3D11.Handles
{
    public class D3D11Device : SafeHandle
    {
        public D3D11Device(): base(IntPtr.Zero, true) { }
        public override bool IsInvalid => handle == IntPtr.Zero;
        protected override bool ReleaseHandle()
        {
            throw new NotImplementedException();
        }
    }
}
