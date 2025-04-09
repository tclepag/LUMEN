using System;
using System.Runtime.InteropServices;

namespace Lumen.Core
{

    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct CWNINFO
    {
        [MarshalAs(UnmanagedType.LPWStr)]
        public string className;    // LPCWSTR -> IntPtr for string pointer
        [MarshalAs(UnmanagedType.LPWStr)]
        public string windowName;   // LPCWSTR -> IntPtr for string pointer
        public uint winStyle;       // DWORD -> uint
        public uint exStyle;        // DWORD -> uint
        public int width;           // int
        public int height;          // int
        public int posX;            // int
        public int posY;            // int
        public IntPtr parent;       // HWND -> IntPtr
        public IntPtr menu;         // HMENU -> IntPtr
        public int show;            // int
    }

    public class Window : IDisposable
    {
        [DllImport("Lumen.Native.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Native_CreateWindow();
        [DllImport("Lumen.Native.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Native_DestroyWindow(IntPtr window);
        [DllImport("Lumen.Native.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern bool Native_BuildWindow(IntPtr window, CWNINFO windowInfo);
        [DllImport("Lumen.Native.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Native_ProcessWindowMessages(IntPtr window);


        // Fields

        private IntPtr window;

        public Window(CWNINFO windowInfo)
        {
            window = Native_CreateWindow();
            if (window == IntPtr.Zero)
            {
                throw new Exception();
            }

            if (!Native_BuildWindow(window, windowInfo))
            {
                throw new Exception();
            }
        }

        ~Window()
        {
            if (window != IntPtr.Zero)
            {
                Native_DestroyWindow(window);
                window = IntPtr.Zero;
            }
        }

        public int Update()
        {
            if (window != IntPtr.Zero)
            {
                int result = Native_ProcessWindowMessages(window);
                return result;
            }
            return 0;
        }

        public void Dispose()
        {
            GC.SuppressFinalize(this);
        }
    }
}
