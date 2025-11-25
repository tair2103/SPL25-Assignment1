#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    if(cache.contains(track.get_title())){
        cache.get(track.get_title());
        return 1;
    }
    else{
        PointerWrapper<AudioTrack> curr = track.clone();
        if(!curr){
            std::cerr << "Failed to clone track: " << track.get_title() << std::endl;
            throw std::runtime_error("object in null");
        }
        curr->load();
        curr->analyze_beatgrid();

        bool evicted = cache.put(std::move(curr));

        if (evicted) {
            return -1;
        } 
        else {
            return 0; 
        }
    }
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {

    AudioTrack* curr = cache.get(track_title);
    if (curr == nullptr) {
        return nullptr; 
    }
    PointerWrapper<AudioTrack> cloned_track = curr->clone();
    if (!cloned_track) {
        std::cerr << "Track " << track_title << " failed to clone\n";
        return nullptr; 
    }

    return cloned_track.release();
}
