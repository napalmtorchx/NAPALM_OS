using System;
using System.IO.Pipes;
using System.Runtime.InteropServices;

public static class DebugPipe
{
    private const int STD_OUTPUT_HANDLE = -11;
    private const uint ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004;
    private const uint DISABLE_NEWLINE_AUTO_RETURN = 0x0008;

    [DllImport("kernel32.dll")]
    private static extern bool GetConsoleMode(IntPtr hConsoleHandle, out uint lpMode);

    [DllImport("kernel32.dll")]
    private static extern bool SetConsoleMode(IntPtr hConsoleHandle, uint dwMode);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr GetStdHandle(int nStdHandle);

    [DllImport("kernel32.dll")]
    public static extern uint GetLastError();

    public static NamedPipeServerStream? Pipe = null;
    public static Thread? Thread = null;

    private static bool _connected = false;

    public static void Start(string name)
    {
        Pipe = new NamedPipeServerStream(name);
        Thread = new Thread(() => { WaitForConnection(30); });
        Thread.Start();
        Debug.Info("Started pipe - Name:" + name);

        Pipe.WaitForConnection();
        _connected = true;

        //Console.OutputEncoding = System.Text.Encoding.ASCII;
        while (Pipe.IsConnected)
        {
            while (!Pipe.CanRead);
            Console.Write((char)Pipe.ReadByte());
        }
    }

    public static void WaitForConnection(int timeout)
    {
        int time = 0, last = 0;
        while (true)
        {
            if (_connected) { return; }
            if (last != DateTime.Now.Second)
            {
                last = DateTime.Now.Second;
                time++;
            }
            if (time >= timeout) { Debug.Error("Pipe connection timed out."); return; }
        }
    }
}