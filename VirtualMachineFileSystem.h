#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

class FatFileSystem
{
  private:
    int mountFD;

  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();

};
#endif
