using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Lumen.Core
{
    internal struct COM_NATIVE
    {
        [DllImport("Lumen.Windows.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr Native_CreateCom();
        [DllImport("Lumen.Windows.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint Native_AddRef(IntPtr com);
        [DllImport("Lumen.Windows.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Native_ReleaseCom(IntPtr com);
        [DllImport("Lumen.Windows.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Native_ResetCom(IntPtr com);
    }

    // Windows ComPtr handler
    public abstract class COM<T> : IDisposable where T : class
    {
        private IntPtr ptr;
        public COM() {
            ptr = COM_NATIVE.Native_CreateCom();
        }
        public COM(IntPtr ptr, bool addRef = true) { 
            this.ptr = ptr;
            if (addRef && ptr != IntPtr.Zero)
            {
                COM_NATIVE.Native_AddRef(ptr);
            }
        }
        ~COM() {
            Destroy();
        }

        public IntPtr Get() => ptr;

        // Cleans the COM value
        public void Reset()
        {
            COM_NATIVE.Native_ResetCom(ptr);
        }

        public void Dispose()
        {
            Destroy();
            GC.SuppressFinalize(this);
        }

        private void Destroy()
        {
            if (ptr != IntPtr.Zero)
            {
                COM_NATIVE.Native_ReleaseCom(ptr);
                ptr = IntPtr.Zero;
            }
        }
    }
}
