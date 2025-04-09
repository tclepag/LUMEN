using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using Lumen.Graphics.Backends;


namespace Lumen.Graphics.Backends.D3D11
{
    internal struct D3D11_NATIVE
    {
        [DllImport("Lumen.DirectX", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Native_InitializeD3D11(out IntPtr device, out IntPtr context, out IntPtr swapChain);
    }

    public class D3D11Renderer : IRenderer, IDisposable
    {
        private IntPtr device;
        private IntPtr context;
        private IntPtr swapChain;

        public void Initialize()
        {
            int hr = D3D11_NATIVE.Native_InitializeD3D11(out device, out context, out swapChain);
            if (hr != 0) 
            {
                Marshal.ThrowExceptionForHR(hr);
            }
            Console.WriteLine("Successfully initialized D3D11!");
        }

        public void Render()
        {
            throw new NotImplementedException();
        }

        public void Cleanup()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }
    }
}
