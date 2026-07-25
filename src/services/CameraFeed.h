#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>


// Un frame decodificato, pronto per essere passato a LVGL come
// lv_image_dsc_t (formato ARGB8888, 4 byte per pixel: B,G,R,A).
struct CameraFrame
{
    std::vector<uint8_t> pixels;
    int width = 0;
    int height = 0;
};


// Scarica periodicamente lo snapshot JPEG di una telecamera Home Assistant
// (endpoint /api/camera_proxy/{entity_id}) su un thread dedicato, lo
// decodifica in pixel grezzi e lo rende disponibile in modo thread-safe.
//
// Il thread principale (LVGL) chiama solo getLatestFrame(), che è
// un'operazione economica (copia di uno shared_ptr) e non fa mai I/O.
class CameraFeed
{

public:

    static CameraFeed& instance();

    // Avvia il thread di download per una specifica entità.
    // Se già avviato per la stessa entità, non fa nulla.
    void start(const std::string& entityId, int intervalMs = 3000);

    void stop();

    // Ritorna l'ultimo frame disponibile (nullptr se non ancora pronto,
    // es. nei primi istanti dopo l'avvio).
    std::shared_ptr<CameraFrame> getLatestFrame() const;


    CameraFeed(const CameraFeed&) = delete;
    CameraFeed& operator=(const CameraFeed&) = delete;


private:

    CameraFeed() = default;
    ~CameraFeed();

    void loop();

    std::string entityId;
    int intervalMs = 3000;

    std::atomic<bool> running{false};
    std::thread worker;

    mutable std::mutex frameMutex;
    std::shared_ptr<CameraFrame> latestFrame;

};
