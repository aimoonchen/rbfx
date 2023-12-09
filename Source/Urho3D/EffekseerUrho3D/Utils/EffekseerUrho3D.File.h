#pragma once
#include "Effekseer/Effekseer.Base.Pre.h"
#include "Effekseer/Effekseer.File.h"
#include "../../IO/AbstractFile.h"
//#include "EASTL/unique_ptr.h"

namespace Urho3D {
    class Context;
    class File;
}

namespace EffekseerUrho3D {
class FileReader : public Effekseer::FileReader
{
private:
    Urho3D::AbstractFilePtr m_filePtr;

public:
    FileReader(Urho3D::AbstractFilePtr file);

    ~FileReader() override;

    size_t Read(void* buffer, size_t size) override;

    void Seek(int position) override;

    int GetPosition() const override;

    size_t GetLength() const override;
};

class FileWriter : public Effekseer::FileWriter
{
private:
    Urho3D::AbstractFilePtr m_filePtr;

public:
    FileWriter(Urho3D::AbstractFilePtr file);

    ~FileWriter() override;

    size_t Write(const void* buffer, size_t size) override;

    void Flush() override;

    void Seek(int position) override;

    int GetPosition() const override;

    size_t GetLength() const override;
};

class FileInterface : public Effekseer::FileInterface
{
public:
    FileInterface(Urho3D::Context* context);
    Effekseer::FileReaderRef OpenRead(const char16_t* path) override;

    Effekseer::FileWriterRef OpenWrite(const char16_t* path) override;
private:
    Urho3D::Context* context_{ nullptr };
};
}
