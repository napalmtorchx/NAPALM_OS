using System;
using System.IO;
using System.Text;

public static class Program
{
    public static RAMFileSystem RAMFS = new RAMFileSystem();

    private static void Main(string[] args)
    {
        Debug.Info("NapalmOS RAMFS Utility");

        RAMFS = new RAMFileSystem(1024, 33554432);

        ImportFile("../Images/RAMDISK/areallyreallylongfilenameformyramfs.txt", "areallyreallylongfilenameformyramfs.txt");
        ImportFile("../Images/RAMDISK/test.txt", "test.txt");
        ImportFile("../Images/RAMDISK/test_stkovrflw.app", "test_stkovrflw.app");
        ImportFile("../Images/RAMDISK/test_add.app", "test_add.app");
        ImportFile("../Images/RAMDISK/test_idle.app", "test_idle.app");
        SaveImage("../Images/ramdisk.img");   
    }

    public static void SaveImage(string fname)
    {
        File.WriteAllBytes(fname, RAMFS.Data);
        Debug.Info("Saved image file to '" + fname + "'");
    }

    public static void ImportFile(string src, string dest, bool hidden = false)
    {
        if (!File.Exists(src)) { Debug.Error("Unable to locate file '" + src + "'"); return; }
        byte[] data = File.ReadAllBytes(src);
        RAMFS.AddFile(dest, data, hidden);
    }
}
