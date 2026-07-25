#include "CameraFeed.h"
#include "../config/secrets.h"

#include <curl/curl.h>
#include <jpeglib.h>

#include <csetjmp>
#include <chrono>
#include <iostream>


// --- Fetch binario via curl -------------------------------------------

static size_t WriteBinaryCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
)
{
    size_t totalSize = size * nmemb;

    auto* buffer = static_cast<std::vector<uint8_t>*>(userp);
    auto* data = static_cast<uint8_t*>(contents);

    buffer->insert(buffer->end(), data, data + totalSize);

    return totalSize;
}


static bool fetchCameraJpeg(
    const std::string& entityId,
    std::vector<uint8_t>& outBytes
)
{
    CURL* curl = curl_easy_init();

    if(!curl)
        return false;

    std::string url =
        std::string(HA_URL) +
        "/api/camera_proxy/" +
        entityId;

    struct curl_slist* headers = nullptr;

    std::string auth = "Authorization: Bearer " + std::string(HA_TOKEN);
    headers = curl_slist_append(headers, auth.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBinaryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outBytes);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode result = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result == CURLE_OK && !outBytes.empty();
}


// --- Decodifica JPEG -> ARGB8888 (via libjpeg) --------------------------

struct JpegErrorMgr
{
    jpeg_error_mgr pub;
    jmp_buf setjmpBuffer;
};

static void jpegErrorExit(j_common_ptr cinfo)
{
    JpegErrorMgr* err = reinterpret_cast<JpegErrorMgr*>(cinfo->err);
    longjmp(err->setjmpBuffer, 1);
}


static bool decodeJpegToArgb(
    const std::vector<uint8_t>& jpegData,
    std::vector<uint8_t>& outPixels,
    int& outWidth,
    int& outHeight
)
{
    jpeg_decompress_struct cinfo;
    JpegErrorMgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = jpegErrorExit;

    if(setjmp(jerr.setjmpBuffer))
    {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(
        &cinfo,
        jpegData.data(),
        static_cast<unsigned long>(jpegData.size())
    );

    if(jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
    {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);

    outWidth = static_cast<int>(cinfo.output_width);
    outHeight = static_cast<int>(cinfo.output_height);

    std::vector<uint8_t> rgbRow(static_cast<size_t>(outWidth) * 3);

    outPixels.resize(static_cast<size_t>(outWidth) * outHeight * 4);

    int y = 0;

    while(cinfo.output_scanline < cinfo.output_height)
    {
        JSAMPROW rowPointer[1] = { rgbRow.data() };

        jpeg_read_scanlines(&cinfo, rowPointer, 1);

        uint8_t* dst = outPixels.data() + static_cast<size_t>(y) * outWidth * 4;

        for(int x = 0; x < outWidth; x++)
        {
            uint8_t r = rgbRow[x * 3 + 0];
            uint8_t g = rgbRow[x * 3 + 1];
            uint8_t b = rgbRow[x * 3 + 2];

            // Stesso ordine byte usato da energy_bg.c: B, G, R, A
            dst[x * 4 + 0] = b;
            dst[x * 4 + 1] = g;
            dst[x * 4 + 2] = r;
            dst[x * 4 + 3] = 0xFF;
        }

        y++;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return true;
}


// --- CameraFeed ----------------------------------------------------------

CameraFeed& CameraFeed::instance()
{
    static CameraFeed feed;
    return feed;
}


CameraFeed::~CameraFeed()
{
    stop();
}


void CameraFeed::start(const std::string& id, int interval)
{
    if(running)
        return; // già avviato

    entityId = id;
    intervalMs = interval;
    running = true;

    worker = std::thread(&CameraFeed::loop, this);
}


void CameraFeed::stop()
{
    if(!running)
        return;

    running = false;

    if(worker.joinable())
        worker.join();
}


void CameraFeed::loop()
{
    while(running)
    {
        std::vector<uint8_t> jpegBytes;

        if(fetchCameraJpeg(entityId, jpegBytes))
        {
            auto frame = std::make_shared<CameraFrame>();

            if(decodeJpegToArgb(jpegBytes, frame->pixels, frame->width, frame->height))
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                latestFrame = frame;
            }
            else
            {
                std::cout << "CameraFeed: decodifica JPEG fallita per " << entityId << std::endl;
            }
        }
        else
        {
            std::cout << "CameraFeed: richiesta fallita per " << entityId << std::endl;
        }

        int waited = 0;

        while(waited < intervalMs && running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waited += 100;
        }
    }
}


std::shared_ptr<CameraFrame> CameraFeed::getLatestFrame() const
{
    std::lock_guard<std::mutex> lock(frameMutex);
    return latestFrame;
}
