#include "EffekseerUrho3D.File.h"
#include "EffekseerUrho3D.Utils.h"
#include "../../Core/Context.h"
#include "../../Resource/ResourceCache.h"
#include "../../IO/File.h"

namespace EffekseerUrho3D {
FileReader::FileReader(Urho3D::File* filePtr)
{
    m_filePtr.reset(filePtr);
}

FileReader::~FileReader()
{
    m_filePtr->Close();
}

size_t FileReader::Read(void* buffer, size_t size)
{
    return m_filePtr->Read(buffer, size);
}

void FileReader::Seek(int position)
{
    m_filePtr->Seek(position);
}

int FileReader::GetPosition() const
{
    return m_filePtr->GetPosition();
}

size_t FileReader::GetLength() const
{
    return m_filePtr->GetSize();
}

FileWriter::FileWriter(Urho3D::File* filePtr)
{
    m_filePtr.reset(filePtr);
}

FileWriter::~FileWriter()
{
    m_filePtr->Close();
}

size_t FileWriter::Write(const void* buffer, size_t size)
{
    return m_filePtr->Write(buffer, size);
}

void FileWriter::Flush()
{
    m_filePtr->Flush();
}

void FileWriter::Seek(int position)
{
    m_filePtr->Seek(position);
}

int FileWriter::GetPosition() const
{
    return m_filePtr->GetPosition();
}

size_t FileWriter::GetLength() const
{
    return m_filePtr->GetSize();
}

FileInterface::FileInterface(Urho3D::Context* context)
    : context_{ context }
{
}

Effekseer::FileReaderRef FileInterface::OpenRead(const char16_t* path)
{
    return Effekseer::MakeRefPtr<FileReader>(new Urho3D::File(context_, ToGdString(path), Urho3D::FILE_READ));
}

Effekseer::FileWriterRef FileInterface::OpenWrite(const char16_t* path)
{
    return Effekseer::MakeRefPtr<FileWriter>(new Urho3D::File(context_, ToGdString(path), Urho3D::FILE_WRITE));
}
}
