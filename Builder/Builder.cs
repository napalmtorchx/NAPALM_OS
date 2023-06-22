using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
using DiscUtils.Iso9660;

public static class Builder
{
    public static string Path { get { return Environment.CurrentDirectory.Replace("\\", "/") + "/"; } } 

    public const string AssemblerPath       = "../Bin/NASM/nasm.exe";
    public const string CompilerPath        = "../Bin/GCC/bin/i686-elf-gcc.exe";
    public const string LinkerPath          = "../Bin/GCC/bin/i686-elf-ld.exe";
    public const string LiminePath          = "../Bootloader/";
    public const string LimineExePath       = "../Bin/limine-deploy.exe";
    public const string ObjsPath            = "../Bin/OBJS/";
    public const string OutputPath          = "../Images/";
    public const string KernelPath          = "../Kernel/";
    public const string LinkerScriptPath    = "../Kernel/Build/Linker.ld";
    public const string VMWarePath          = "C:/Program Files (x86)/VMware/VMware Player/vmplayer.exe";

    public const string CompilerArgs = "-fno-use-cxa-atexit -fno-exceptions -fno-rtti -nostdlib -ffreestanding -O2 -Wextra -fshort-enums -DKERNEL";

    public static bool BuildAll()
    {
        PreClean();
        if (!AssembleKernel()) { return false; }
        if (!CompileKernel()) { return false; }
        if (!LinkKernel()) { return false; }
        if (!CreateRAMDisk()) { return false; }
        if (!CreateISO()) { return false; }
        if (!StartVM()) { return false; }
        PostClean();

        return true;
    }

    public static void PreClean()
    {
        if (Directory.Exists(ObjsPath)) { Directory.Delete(ObjsPath, true); }
        Directory.CreateDirectory(ObjsPath);
    }

    public static void PostClean()
    {

    }

    public static bool AssembleKernel()
    {
        List<string> files_in = new List<string>();
        List<string> files_out = new List<string>();
        foreach (string file in Directory.EnumerateFiles(Path + KernelPath + "Source/", "*.asm", SearchOption.AllDirectories))
        {
            string fname = file.Replace("\\", "/");
            fname = fname.Substring(fname.LastIndexOf('/') + 1);
            files_in.Add(file);
            files_out.Add(Path + ObjsPath + fname.Replace(".asm", ".o"));
        }

        for (int i = 0; i < files_in.Count; i++)
        {
            try
            {
                Process proc = Process.Start(AssemblerPath, "-felf32 \"" + files_in[i] + "\" -o \"" + files_out[i] + "\"");
                proc.WaitForExit();
                if (proc.ExitCode == 0) { Debug.Info("Successfully assembled file: '" + files_in[i] + "' > '" + files_out[i].Substring(files_out[i].LastIndexOf('/') + 1) + "'"); }
                else { return false; }
            }
            catch (Exception ex)
            {
                Debug.Error("Build failure at file '" + files_in[i] + "'\n" + ex.Message);
                return false;
            }
        }

        Debug.OK("Finished compiling kernel");
        return true;
    }

    public static bool CompileKernel()
    {
        List<string> files_in = new List<string>();
        List<string> files_out = new List<string>();
        foreach (string file in Directory.EnumerateFiles(Path + KernelPath + "Source/", "*.c", SearchOption.AllDirectories))
        {
            string fname = file.Replace("\\", "/");
            fname = fname.Substring(fname.LastIndexOf('/') + 1);
            files_in.Add(file);
            files_out.Add(Path + ObjsPath + fname.Replace(".c", ".o"));
        }

        for (int i = 0; i < files_in.Count; i++)
        {
            try
            {
                Process proc = Process.Start(CompilerPath, "-Wall -w -I../Kernel/Include -c \"" + files_in[i] + "\" -o \"" + files_out[i] + "\" " + CompilerArgs);
                proc.WaitForExit();
                if (proc.ExitCode == 0) { Debug.Info("Successfully compiled file: '" + files_in[i] + "' > '" + files_out[i].Substring(files_out[i].LastIndexOf('/') + 1) + "'"); }
                else { return false; }
            }
            catch (Exception ex)
            {
                Debug.Error("Build failure at file '" + files_in[i] + "'\n" + ex.Message);
                return false;
            }
        }

        Debug.OK("Finished compiling kernel");
        return true;
    }

    public static bool LinkKernel()
    {
        string[] files = Directory.GetFiles(Path + ObjsPath);
        string files_str = string.Empty;
        
        for (int i = 0; i < files.Length; i++) { files_str += "\"" + files[i] + "\" "; }

        string args = "-T " + LinkerScriptPath + " -o " + Path + ObjsPath + "Kernel.elf " + files_str;

        try
        {
            Process proc = Process.Start(LinkerPath, args);
            proc.WaitForExit();
            if (proc.ExitCode == 0) { Debug.Info("Successfully Linked kernel"); }
            else { return false; }
        }
        catch (Exception ex)
        {
            Debug.Error("Failed to link kernel - " + ex.Message);
            return false;
        }

        return true;
    }

    public static bool CreateRAMDisk()
    {
        try
        {
            Process proc = Process.Start(Path + "../RAMFS/bin/Debug/net7.0/RAMFS.exe");
            proc.WaitForExit();
            if (proc.ExitCode != 0) { Debug.Error("Failed to create ramdisk image" + proc.ExitCode); return false; }
        }
        catch (Exception ex)
        {
            Debug.Error("Error starting VM - " + ex.Message);
            return false;
        }
        return true;
    }

    public static bool CreateISO()
    {
        CDBuilder iso = new CDBuilder()
        {
            UseJoliet = true,
            VolumeIdentifier = "NAPALM_OS",
            UpdateIsolinuxBootTable = true,
        };

        iso.AddFile("kernel.elf", Path + ObjsPath   + "Kernel.elf");
        iso.AddFile("limine.sys", Path + LiminePath + "limine.sys");
        iso.AddFile("limine.cfg", Path + LiminePath + "limine.cfg");
        iso.AddFile("ramdisk.img", Path + OutputPath + "ramdisk.img");
        iso.SetBootImage(File.OpenRead(Path + LiminePath + "limine-cd.bin"), BootDeviceEmulation.NoEmulation, 0);
        iso.Build(Path + OutputPath + "NapalmOS.iso");
        Debug.OK("Finished creating ISO image");

        return true;
    }

    public static bool StartVM()
    {
        try
        {
            Thread thread = new Thread(() => PipeMain("NapalmOS"));
            thread.Start();
            Process proc = Process.Start(VMWarePath, Path + OutputPath + "VMware/vmware_mach.vmx");
            proc.WaitForExit();
            if (proc.ExitCode != 0) { Debug.Error("Failed to run VM - Exited with code " + proc.ExitCode); return false; }
        }
        catch (Exception ex)
        {
            Debug.Error("Error starting VM - " + ex.Message);
            return false;
        }
        return true;
    }

    public static void PipeMain(string name)
    {
        DebugPipe.Start("NapalmOS");
    }
}