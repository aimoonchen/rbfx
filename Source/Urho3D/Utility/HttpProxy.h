#pragma once
#include "Urho3D/Urho3D.h"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>
#ifndef __EMSCRIPTEN__
#include "curl/curl.h"
#endif
namespace Urho3D
{
class CurlTask
{
public:
    CurlTask(uint32_t i)
        : id{i}
    {
    }
    ~CurlTask() = default;
    uint32_t id;
    int curl_code{0};
    int response_code{0};
    int download_size{0};
    int last_download_size{0};
    int ref_cound{2};
    int size{0};
    bool finish{ false };
    std::string url;
    std::string path;
    std::ofstream* file{nullptr};
#ifndef __EMSCRIPTEN__
    CURL* curl{nullptr};
#endif
    std::vector<char> result;
};

template <class T> class Singleton
{
protected:
    Singleton() = default;
    ~Singleton() = default;

public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }
};

class HttpProxy : public Singleton<HttpProxy>
{
public:
    HttpProxy();
    ~HttpProxy();
    void Init();
    void Clear();
    int Download(const char* url, const char* path);
    int Fetch(const char* url, std::vector<char>& body);
    uint32_t FetchAsync(const char* url);
    int Post(const char* url, const char* params, char** body);
    const char* GetError(int c);

    int DownloadSync(const char* url, const char* file_path);
    uint32_t DownloadAsync(const char* url, const char* file_path);
    void ReleaseTask(uint32_t tid);
    bool IsTaskFinish(uint32_t tid);
    void GetState(uint32_t tid, int& curl_code, int& response_code, int& download_size, int& size);
    std::vector<char> GetResult(uint32_t tid);
    int FileLength(const char* url);
    int post(const char* url, const char* params, char** body);
    static int post_func(void* arg);
    int PostAsync(const char* url, const char* params);
    int request_get(const char* url, std::vector<char>& body);
    int SaveFile(const char* url, const char* file_path);
    static void* save_file_func(void* arg);
    uint32_t SaveFileAsync(const char* url, const char* file_path);
    uint32_t SaveFileSync(const char* url, const char* file_path);
    int GetDownloadFileLength(const char* url);

private:
    int counter_{1};
    std::unordered_map<uint32_t, std::unique_ptr<CurlTask>> tasks_;
    std::mutex tasks_mtx_;
    CurlTask* create_task();
    void release_task(uint32_t tid);
    CurlTask* get_task(uint32_t tid);
#ifndef __EMSCRIPTEN__
    static void init_ssl(CURL* curl);
#endif
    static int enable_ssl;
    static int update_pen;
    struct chunk
    {
        std::unique_ptr<char[]> data;
        int size{0};
    };
    struct chunk_contain
    {
        std::vector<std::unique_ptr<chunk>> blocks;
        int total_size{0};
    };

    static size_t write_data(char* buffer, size_t size, size_t nmemb, chunk_contain* c);
    static size_t write_data_to_file(char* buffer, size_t size, size_t nmemb, std::ofstream* file);
    static size_t write_data_to_file_handle(char* buffer, size_t size, size_t nmemb, CurlTask* handle);
#ifndef __EMSCRIPTEN__
    static int progress_func_new(CurlTask* task, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
#endif
    static int progress_func(CurlTask* task, double dltotal, double dlnow, double ultotal, double ulnow);
};

#ifndef __EMSCRIPTEN__
URHO3D_API void CurlInit();
URHO3D_API void CurlCleanup();
URHO3D_API void DownloadDirectory(const std::string& from_url, const std::string& to_path, std::function<void(const std::string&)>* callback = nullptr);
URHO3D_API void UploadDirectory(const std::string& from_path, const std::string& to_url, const std::vector<std::filesystem::path>& pathlist, const std::vector<std::string>& exclude, std::function<void(const std::string&)>* callback = nullptr);
URHO3D_API size_t Post(const std::string& url, const std::string& params, char** body);
#endif

} // namespace Urho3D
