#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist) {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (const auto& track_info : library_tracks) {
        AudioTrack* new_track = nullptr;
        if (track_info.type == "MP3") {
            new_track = new MP3Track(
                track_info.title, 
                track_info.artists, 
                track_info.duration_seconds, 
                track_info.bpm,
                track_info.extra_param1, 
                track_info.extra_param2 
            );
            std::cerr << "MP3Track created: " << track_info.extra_param1 << " kbps\n";
            
        } else if (track_info.type == "WAV") {
            new_track = new WAVTrack(
                track_info.title, 
                track_info.artists, 
                track_info.duration_seconds, 
                track_info.bpm,
                track_info.extra_param1, 
                track_info.extra_param2
            );
            std::cerr << "WAVTrack created: " << track_info.extra_param1 << " Hz/" << track_info.extra_param2 << " bit\n";
            
        }

        if (new_track) { 
            library.push_back(new_track);
        }
    }
    std::cerr << "[INFO] Track library built: " << library.size() << " tracks loaded\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, const std::vector<int>& track_indices) {
    std::cerr << "[INFO] Loading playlist: " << playlist_name << "\n";
    Playlist new_list(playlist_name);
    playlist = new_list;

    for(int index : track_indices){
        if(index > library.size() || index <= 0){
            std::cerr << "[WARNING] Invalid track index: " << index << "\n";
        }
        else{
            AudioTrack* track = library[index-1];
            PointerWrapper<AudioTrack> cloned_track = track->clone();
            if(cloned_track){
                cloned_track->load();
                cloned_track->analyze_beatgrid();
                AudioTrack* cloned = cloned_track.release();
                playlist.add_track(cloned);
                std::cerr << "[INFO] Added: " << cloned->get_title() << " to playlist "<< playlist_name << "\n";
            }
            else{
                std::cerr << "[WARNING] Track: " << track->get_title() << " failed to clone during playlist load.\n";
            }
        }
    }
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for(AudioTrack* track : tracks){
        titles.push_back(track->get_title());
    }
    
    return titles; 
}
