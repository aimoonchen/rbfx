#pragma once
#include "Effekseer/Effekseer.Base.Pre.h"
#include "Effekseer/Effekseer.File.h"
#include "EASTL/unique_ptr.h"

namespace Urho3D {
    class Context;
    class File;
}

namespace EffekseerUrho3D {
class FileReader : public Effekseer::FileReader
{
private:
    ea::unique_ptr<Urho3D::File> m_filePtr;

public:
    FileReader(Urho3D::File* file);

    ~FileReader() override;

    size_t Read(void* buffer, size_t size) override;

    void Seek(int position) override;

    int GetPosition() const override;

    size_t GetLength() const override;
};

class FileWriter : public Effekseer::FileWriter
{
private:
    ea::unique_ptr<Urho3D::File> m_filePtr;

public:
    FileWriter(Urho3D::File* file);

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
