#include "HttpProxy.h"
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <queue>

#include "../IO/Log.h"
#if defined(__EMSCRIPTEN__)
#include <emscripten/fetch.h>
#endif

#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

namespace Urho3D
{
    int HttpProxy::enable_ssl{ 1 };
    int HttpProxy::update_pen{ 0 };

    void* HttpProxy::save_file_func(void* arg)
    {
#ifndef __EMSCRIPTEN__
        auto task = (CurlTask*)arg;
        auto url = task->url.c_str();
        auto file_path = task->path.c_str();

        std::ofstream file;
        file.open(file_path, std::ios::out | std::ios::binary);
        if (!file.good()) {
            return 0;
        }
        task->file = &file;
        auto curl = curl_easy_init();
        task->curl = curl;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            init_ssl(curl);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_file_handle);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, task);
            //curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);

            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, task);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            //curl_easy_setopt(curl, CURLOPT_RANGE,"100-"); 

#if LIBCURL_VERSION_NUM >= 0x072000
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_func_new);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, task);
#endif
            auto curl_code = curl_easy_perform(curl);

            file.close();

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (curl_code == CURLE_OK && response_code == 200) {
                task->finish = true;
            } else {
                remove(file_path);
            }
            task->response_code = response_code;
            task->curl_code = curl_code;
            curl_easy_cleanup(curl);
        } else {
            file.close();
            remove(file_path);
        }

        HttpProxy::Instance().release_task(task->id);
#endif
        return 0;
    }
    HttpProxy::HttpProxy()
    {
    }

    HttpProxy::~HttpProxy()
    {
    }

    void HttpProxy::Init()
    {
#ifndef __EMSCRIPTEN__
        auto code = curl_global_init(CURL_GLOBAL_ALL);
        if (code != CURLE_OK) {
            URHO3D_LOGERRORF("curl_global_init failed : %s\n", GetError(code));
        }
#endif
    }

    void HttpProxy::Clear()
    {
#ifndef __EMSCRIPTEN__
        curl_global_cleanup();
#endif
    }
#ifndef __EMSCRIPTEN__
    void HttpProxy::init_ssl(CURL* curl)
    {
#ifdef _WIN32
        if (enable_ssl)
        {
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            if (update_pen)
            {
                curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
                update_pen = 0;
            }
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }
#endif
#if __APPLE__

#if TARGET_IPHONE_SIMULATOR
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#elif TARGET_OS_IPHONE
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#elif TARGET_OS_MAC
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#else
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif


#endif

#if __ANDROID__
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

#endif
    }
#endif

#if defined(__EMSCRIPTEN__)
    void DownloadAsyncSucceeded(emscripten_fetch_t* fetch)
    {
        auto task = (CurlTask*)fetch->userData;
        const char* file_path = task->path.c_str();
        std::ofstream file;
        file.open(file_path, std::ios::out | std::ios::binary);
        if (!file.good()) {
            URHO3D_LOGERRORF("Create local file %s failed.", file_path);
        }
        else {
            file.write(fetch->data, fetch->numBytes);
            file.close();
            task->finish = true;
        }
        //URHO3D_LOGINFOF("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        URHO3D_LOGINFOF("download file %s.\n", file_path);
        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        emscripten_fetch_close(fetch); // Free data associated with the fetch.
    }

    void DownloadFailed(emscripten_fetch_t* fetch)
    {
        URHO3D_LOGERRORF("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        emscripten_fetch_close(fetch); // Also free data on failure.
    }
#endif
    uint32_t HttpProxy::SaveFileAsync(const char* url, const char* file_path)
    {
        auto task = create_task();
        task->url = url;
        task->path = file_path;
#if defined(__EMSCRIPTEN__)
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.userData = task;
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = DownloadAsyncSucceeded;
        attr.onerror = DownloadFailed;
        emscripten_fetch(&attr, url);
#else
        std::thread td(save_file_func, task);
        td.detach();
#endif
        return task->id;
    }

    uint32_t HttpProxy::SaveFileSync(const char* url, const char* file_path)
    {
        auto task = create_task();
        task->url = url;
        task->path = file_path;
        save_file_func(task);
        return task->id;
    }

    int HttpProxy::post(const char* url, const char* params, char** body)
    {
#ifndef __EMSCRIPTEN__
        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);

            curl_slist* plist = curl_slist_append(nullptr,
                "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);
            init_ssl(curl);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            chunk_contain contain;
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contain);
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "curlposttest.cookie");
            auto curl_code = curl_easy_perform(curl);

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            *body = (char*)malloc(sizeof(char) * contain.total_size + 1);
            char* dst = *body;
            auto size = contain.blocks.size();
            for (size_t i = 0; i < size; i++) {
                auto& block = contain.blocks[i];
                memcpy(dst, block->data.get(), block->size);
                dst = dst + block->size;
            }
            curl_easy_cleanup(curl);
            if (0 == response_code)
                return (int)curl_code;
            return (int)response_code;
        }
#endif
        return -1;
    }

    int HttpProxy::post_func(void* arg)
    {
#ifndef __EMSCRIPTEN__
        auto task = (CurlTask*)arg;
        const char* url = task->url.c_str();
        const char* params = task->path.c_str();
        auto curl = curl_easy_init();
        task->curl = curl;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_slist* plist = curl_slist_append(nullptr,
                "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);
            init_ssl(curl);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            chunk_contain contain;
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contain);
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "curlposttest.cookie");
            auto curl_code = curl_easy_perform(curl);

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            task->result.resize((size_t)contain.total_size + 1);// = std::make_unique<char[]>((size_t)contain.total_size + 1);
            auto dst = task->result.data();
            auto size = contain.blocks.size();
            for (size_t i = 0; i < size; i++) {
                auto& block = contain.blocks[i];
                memcpy(dst, block->data.get(), block->size);
                dst = dst + block->size;
            }
            task->curl_code = curl_code;
            task->response_code = response_code;
            curl_easy_cleanup(curl);
        }
        HttpProxy::Instance().release_task(task->id);
#endif
        return 0;
    }

    int HttpProxy::PostAsync(const char* url, const char* params)
    {
        auto task = create_task();
        task->url = url;
        task->path = params;
        std::thread td(post_func, task);
        td.detach();
        return task->id;
    }
#if defined(__EMSCRIPTEN__)
    void FetchSucceeded(emscripten_fetch_t* fetch)
    {
        auto task = ((CurlTask*)fetch->userData);
        task->result.resize(fetch->numBytes);// = std::make_unique<char[]>(fetch->numBytes);
        memcpy(task->result.data(), fetch->data, fetch->numBytes);
        task->size = fetch->numBytes;
        task->finish = true;
        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        URHO3D_LOGINFOF("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        emscripten_fetch_close(fetch); // Free data associated with the fetch.
    }
#endif

    uint32_t HttpProxy::FetchAsync(const char* url)
    {
        auto task = create_task();
        task->url = url;
#if defined(__EMSCRIPTEN__)
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.userData = task;
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = FetchSucceeded;
        attr.onerror = DownloadFailed;
        emscripten_fetch(&attr, url);
#else
        std::thread td([this](CurlTask* task) {
            request_get(task->url.c_str(), task->result);
            task->finish = true;
            }, task);
        td.detach();
#endif
        return task->id;
    }

    int HttpProxy::request_get(const char* url, std::vector<char>& body)
    {
#ifndef __EMSCRIPTEN__
        auto curl = curl_easy_init();
        int error_code = -1;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            init_ssl(curl);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            chunk_contain contain;
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contain);
            auto curl_code = curl_easy_perform(curl);
            if (curl_code != CURLE_OK) {
                URHO3D_LOGERRORF("curl_easy_perform failed : %s", GetError(curl_code));
            }
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200) {
                URHO3D_LOGERRORF("server error, response code : %d", response_code);
            }
            if (curl_code == CURLE_OK && response_code == 200) {
                body.resize(contain.total_size);
                auto dst = body.data();
                auto size = contain.blocks.size();
                for (size_t i = 0; i < size; i++) {
                    auto& block = contain.blocks[i];
                    memcpy(dst, block->data.get(), block->size);
                    dst = dst + block->size;
                }
                error_code = 0;
            }
            curl_easy_cleanup(curl);
        }
        return error_code;
#else
        return 0;
#endif
    }

    int HttpProxy::SaveFile(const char* url, const char* file_path)
    {
        int error_code = -1;
#ifndef __EMSCRIPTEN__
        std::ofstream file;
        file.open(file_path, std::ios::out | std::ios::binary);
        if (!file.good()) {
            return error_code;
        }
        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            init_ssl(curl);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_file);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
            auto curl_code = curl_easy_perform(curl);
            if (curl_code != CURLE_OK) {
                URHO3D_LOGERRORF("curl_easy_perform failed : %s", GetError(curl_code));
            }
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200) {
                URHO3D_LOGERRORF("server error, response_code : %d", response_code);
            }
            curl_easy_cleanup(curl);
            if (curl_code == CURLE_OK && response_code == 200) {
                error_code = 0;
            }
        }
        file.close();
#endif
        return error_code;
    }

    const char* HttpProxy::GetError(int c)
    {
#ifndef __EMSCRIPTEN__
        return curl_easy_strerror((CURLcode)c);
#else
        return "";
#endif
    }

    int HttpProxy::Download(const char* url, const char* path)
    {
        return SaveFile(url, path);
    }

    int HttpProxy::Fetch(const char* url, std::vector<char>& body)
    {
        return request_get(url, body);
    }
    int HttpProxy::Post(const char* url, const char* params, char** body)
    {
        return post(url, params, body);
    }
    // void CurlProxy::curl_enable_ssl(int b)
    // {
    // 	curl_manager::enable_ssl = b;
    // }
    int HttpProxy::FileLength(const char* url)
    {
        return GetDownloadFileLength(url);
    }

    uint32_t HttpProxy::DownloadAsync(const char* url, const char* file_path)
    {
        return SaveFileAsync(url, file_path);
    }

    int HttpProxy::DownloadSync(const char* url, const char* file_path)
    {
        return SaveFileSync(url, file_path);
    }

    void HttpProxy::ReleaseTask(uint32_t tid)
    {
        return release_task(tid);
    }

    bool HttpProxy::IsTaskFinish(uint32_t tid)
    {
        auto task = get_task(tid);
        if (!task) {
            return false;
        }
        return task->finish;
    }

    void HttpProxy::GetState(uint32_t tid, int& curl_code, int& response_code, int& download_size, int& size)
    {
        auto task = get_task(tid);
        if (!task) {
            return;
        }
        curl_code       = task->curl_code;
        response_code   = task->response_code;
        download_size   = task->download_size;
        size            = task->size;
    }

    std::vector<char> HttpProxy::GetResult(uint32_t tid)
    {
        auto task = get_task(tid);
        if (!task->finish) {
            return {};
        }
        return std::move(task->result);
    }

    int HttpProxy::GetDownloadFileLength(const char* url)
    {
        double downloadFileLenth = 0;
#ifndef __EMSCRIPTEN__
        CURL* curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
            CURLcode ret = curl_easy_perform(curl);
            if (ret == CURLE_OK)
            {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if (response_code == 200)
                {
                    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
                }
                else
                {
                    downloadFileLenth = -2;
                }

            }
            else
            {
                downloadFileLenth = -1;
            }
            curl_easy_cleanup(curl);
        }
#endif
        return (int)downloadFileLenth;
    }

    size_t HttpProxy::write_data(char* buffer, size_t size, size_t nmemb, chunk_contain* contain)
    {
        auto newBlock = std::make_unique<chunk>();
        auto segsize = size * nmemb;
        newBlock->data = std::make_unique<char[]>(segsize + 1);
        memcpy(newBlock->data.get(), buffer, segsize);
        newBlock->size = segsize;
        contain->blocks.push_back(std::move(newBlock));
        contain->total_size += segsize;
        return segsize;
    }
    size_t HttpProxy::write_data_to_file(char* buffer, size_t size, size_t nmemb, std::ofstream* file)
    {
        int segsize = size * nmemb;
        file->write(buffer, segsize);
        return segsize;
    }
    size_t HttpProxy::write_data_to_file_handle(char* buffer, size_t size, size_t nmemb, CurlTask* handle)
    {
        int segsize = size * nmemb;
        handle->file->write(buffer, segsize);
        return segsize;
    }
#ifndef __EMSCRIPTEN__
    int HttpProxy::progress_func_new(CurlTask* task,
        curl_off_t dltotal, curl_off_t dlnow,
        curl_off_t ultotal, curl_off_t ulnow)
    {

        task->download_size = task->last_download_size + (int)dlnow;
        if (dltotal > 0) {
            task->size = task->last_download_size + (int)dltotal;
        }
        return CURLE_OK;

    }
#endif
    int HttpProxy::progress_func(CurlTask* task, double dltotal, double dlnow, double ultotal, double ulnow)
    {
#ifndef __EMSCRIPTEN__
        return progress_func_new(task, (curl_off_t)dltotal, (curl_off_t)dlnow, (curl_off_t)ultotal, (curl_off_t)ulnow);
#else
        return 0;
#endif
    }

    CurlTask* HttpProxy::create_task()
    {
        std::lock_guard<std::mutex> lock(tasks_mtx_);
        int tid = counter_++;
        tasks_[tid] = std::make_unique<CurlTask>(tid);
        return tasks_[tid].get();
    }

    void HttpProxy::release_task(uint32_t tid)
    {
        std::lock_guard<std::mutex> lock(tasks_mtx_);
        auto it = tasks_.find(tid);
        if (it == tasks_.end())
            return;

        auto task = it->second.get();
        task->ref_cound--;
        if (task->ref_cound <= 0) {
            tasks_.erase(tid);
        }
    }

    CurlTask* HttpProxy::get_task(uint32_t tid)
    {
        std::lock_guard<std::mutex> lock(tasks_mtx_);
        auto it = tasks_.find(tid);
        return (it != tasks_.end()) ? it->second.get() : nullptr;
    }
#ifndef __EMSCRIPTEN__
    // TODO : Refactor DownloadDirectory/UploadDirectory
    static std::string g_download_path;
    static std::string g_current_path;
    static std::queue<std::string> g_remote_paths;
    static const char* g_user_password = "simon:cpper3.14";
    struct callback_data {
        FILE* output;
    };
    static std::function<void(const std::string&)>* g_download_callback = nullptr;
    static std::string g_current_download_path;
    static long file_is_coming(struct curl_fileinfo* finfo, struct callback_data* data, int remains)
    {
        if (finfo->filetype == CURLFILETYPE_DIRECTORY) {
            g_remote_paths.emplace(g_current_path.empty() ? finfo->filename : g_current_path + "/" + finfo->filename);
        }
        else if (finfo->filetype == CURLFILETYPE_FILE) {
            g_current_download_path = g_current_path + "/" + finfo->filename;
            const std::string path = g_download_path + "/" + g_current_download_path;
            data->output = fopen(path.c_str(), "wb");
            if (!data->output) {
                return CURL_CHUNK_BGN_FUNC_FAIL;
            }
        }

        return CURL_CHUNK_BGN_FUNC_OK;
    }

    static long file_is_downloaded(struct callback_data* data)
    {
        if (data->output) {
            fclose(data->output);
            data->output = 0x0;
            if (g_download_callback) {
                (*g_download_callback)(g_current_download_path);
            }
        }
        return CURL_CHUNK_END_FUNC_OK;
    }

    static size_t write_it(char* buff, size_t size, size_t nmemb, void* cb_data)
    {
        struct callback_data* data = (callback_data*)cb_data;
        size_t written = 0;
        if (data->output)
            written = fwrite(buff, size, nmemb, data->output);
        else
            /* listing output */
            written = fwrite(buff, size, nmemb, stdout);
        return written;
    }

    static void download(const std::string& url)
    {
        /* initialization of easy handle */
        auto handle = curl_easy_init();
        if (!handle) {
            curl_global_cleanup();
            return;
        }
        /* User and password for the FTP login */
        curl_easy_setopt(handle, CURLOPT_USERPWD, g_user_password);

        /* turn on wildcard matching */
        curl_easy_setopt(handle, CURLOPT_WILDCARDMATCH, 1L);

        /* callback is called before download of concrete file started */
        curl_easy_setopt(handle, CURLOPT_CHUNK_BGN_FUNCTION, file_is_coming);

        /* callback is called after data from the file have been transferred */
        curl_easy_setopt(handle, CURLOPT_CHUNK_END_FUNCTION, file_is_downloaded);

        /* this callback will write contents into files */
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_it);

        /* help data */
        callback_data data = { 0 };

        /* put transfer data into callbacks */
        curl_easy_setopt(handle, CURLOPT_CHUNK_DATA, &data);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);

        /* curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L); */

        /* set a URL containing wildcard pattern (only in the last part) */
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

        /* and start transfer! */
        auto res = curl_easy_perform(handle);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(handle);
    }

    void DownloadDirectory(const std::string& from_url, const std::string& to_path, std::function<void(const std::string&)>* callback)
    {
        g_download_callback = callback;
        g_download_path = to_path;
        std::string current_url(from_url + "/*");
        download(current_url);

        while (!g_remote_paths.empty()) {
            const auto& path = g_remote_paths.front();
            g_current_path = path;
            auto local_path = g_download_path + "/" + path;
            if (!std::filesystem::exists(local_path)) {
                std::filesystem::create_directories(local_path);
            }
            current_url = from_url + "/" + path + "/*";
            download(current_url);
            g_remote_paths.pop();
        }
        g_download_callback = nullptr;
        g_current_download_path = "";
    }

    /* NOTE: if you want this example to work on Windows with libcurl as a
       DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
       Failing to do so will give you a crash since a DLL may not use the
       variable's memory when passed in to it from an app like this. */
    static size_t read_callback(char* ptr, size_t size, size_t nmemb, void* stream)
    {
        unsigned long nread;
        /* in real-world cases, this would probably get this data differently
           as this fread() stuff is exactly what the library already would do
           by default internally */
        size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

        if (retcode > 0) {
            nread = (unsigned long)retcode;
            //fprintf(stderr, "*** We read %lu bytes from file\n", nread);
        }

        return retcode;
    }

    static void upload(const std::string& filename, const std::string& to_url)
    {
        struct stat file_info;
        unsigned long fsize;

        struct curl_slist* headerlist = NULL;
        //     static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;
        //     static const char buf_2[] = "RNTO " RENAME_FILE_TO;

            /* get the file size of the local file */
        if (stat(filename.c_str(), &file_info)) {
            printf("Couldn't open '%s': %s\n", filename.c_str(), strerror(errno));
            return;
        }
        fsize = (unsigned long)file_info.st_size;

        /* get a FILE * of the same file */
        auto hd_src = fopen(filename.c_str(), "rb");

        /* get a curl handle */
        auto curl = curl_easy_init();
        if (curl) {
            /* build a list of commands to pass to libcurl */
    //        headerlist = curl_slist_append(headerlist, buf_1);
    //        headerlist = curl_slist_append(headerlist, buf_2);

            /* User and password for the FTP login */
            curl_easy_setopt(curl, CURLOPT_USERPWD, g_user_password);
            curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);
            /* we want to use our own read function */
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

            /* enable uploading */
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            /* specify target */
            curl_easy_setopt(curl, CURLOPT_URL, to_url.c_str());

            /* pass in that last of FTP commands to run after the transfer */
            curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

            /* now specify which file to upload */
            curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

            /* Set the size of the file to upload (optional).  If you give a *_LARGE
               option you MUST make sure that the type of the passed-in argument is a
               curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
               make sure that to pass in a type 'long' argument. */
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);

            /* Now run off and do what you have been told! */
            auto res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            /* clean up the FTP commands list */
            curl_slist_free_all(headerlist);

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        fclose(hd_src); /* close the local file */
    }

    void UploadDirectory(const std::string& from_path, const std::string& to_url, const std::vector<std::filesystem::path>& pathlist, const std::vector<std::string>& exclude, std::function<void(const std::string&)>* callback)
    {
        std::filesystem::path root_path(from_path);
        for (auto& path : pathlist) {
            auto relative = std::filesystem::relative(path, root_path).string();
            std::replace(relative.begin(), relative.end(), '\\', '/');
            bool skip = false;
            for (const auto& ex : exclude) {
                if (relative.find(ex) != std::string::npos) {
                    skip = true;
                    break;
                }
            }
            if (skip) {
                continue;
            }
            upload(path.string(), to_url + "/" + relative);
            if (callback) {
                (*callback)(path.string());
            }
        }
    }

    struct MemoryStruct
    {
        char* memory;
        size_t size;
    };

    static size_t write_memory_callback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct* mem = (struct MemoryStruct*)userp;

        char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
        if (!ptr)
        {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }
    size_t Post(const std::string& url, const std::string& params, char** body)
    {
        struct MemoryStruct chunk;
        chunk.memory = (char*)malloc(1); /* will be grown as needed by realloc above */
        chunk.size = 0; /* no data at this point */

        /* get a curl handle */
        auto curl = curl_easy_init();
        if (curl) {
            struct curl_slist* headers = NULL;

            /* default type with postfields is application/x-www-form-urlencoded,
               change it if you want */
            //headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            /* send all data to this function  */
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "curlposttest.cookie");

            /* pass on content in request body. When CURLOPT_POSTFIELDSIZE is not used,
               curl does strlen to get the size. */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());

            /* if we do not provide POSTFIELDSIZE, libcurl will strlen() by itself */
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

            /* specify target URL, and note that this URL should include a file
               name, not only a directory */
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            /* Now run off and do what you have been told! */
            auto res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            /* always cleanup */
            curl_easy_cleanup(curl);

            /* free headers */
            curl_slist_free_all(headers);
        }
        *body = chunk.memory;
        return chunk.size;
        //free(chunk.memory);
    }
    void CurlInit()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    void CurlCleanup()
    {
        curl_global_cleanup();
    }
#endif
}
